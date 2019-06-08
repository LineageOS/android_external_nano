/**************************************************************************
 *   nano.c  --  This file is part of GNU nano.                           *
 *                                                                        *
 *   Copyright (C) 1999-2011, 2013-2019 Free Software Foundation, Inc.    *
 *   Copyright (C) 2014-2019 Benno Schulenberg                            *
 *                                                                        *
 *   GNU nano is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published    *
 *   by the Free Software Foundation, either version 3 of the License,    *
 *   or (at your option) any later version.                               *
 *                                                                        *
 *   GNU nano is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "proto.h"
#include "revision.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#if defined(__linux__) || !defined(NANO_TINY)
#include <sys/ioctl.h>
#endif
#ifdef ENABLE_UTF8
#include <langinfo.h>
#endif
#include <locale.h>
#include <string.h>
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif
#include <unistd.h>
#ifdef __linux__
#include <sys/vt.h>
#endif

#ifdef ENABLE_MULTIBUFFER
#define read_them_all TRUE
#else
#define read_them_all FALSE
#endif

#ifdef ENABLE_MOUSE
static int oldinterval = -1;
		/* Used to store the user's original mouse click interval. */
#endif
#ifdef HAVE_TERMIOS_H
static struct termios oldterm;
		/* The user's original terminal settings. */
#else
# define tcsetattr(...)
# define tcgetattr(...)
#endif
static struct sigaction act;
		/* Used to set up all our fun signal handlers. */

static bool input_was_aborted = FALSE;
		/* Whether reading from standard input was aborted via ^C. */

/* Create a new linestruct node.  Note that we do not set prevnode->next. */
linestruct *make_new_node(linestruct *prevnode)
{
	linestruct *newnode = nmalloc(sizeof(linestruct));

	newnode->data = NULL;
	newnode->prev = prevnode;
	newnode->next = NULL;
	newnode->lineno = (prevnode != NULL) ? prevnode->lineno + 1 : 1;

#ifdef ENABLE_COLOR
	newnode->multidata = NULL;
#endif

	return newnode;
}

/* Make a copy of a linestruct node. */
linestruct *copy_node(const linestruct *src)
{
	linestruct *dst = nmalloc(sizeof(linestruct));

	dst->data = mallocstrcpy(NULL, src->data);
	dst->next = src->next;
	dst->prev = src->prev;
	dst->lineno = src->lineno;

#ifdef ENABLE_COLOR
	dst->multidata = NULL;
#endif

	return dst;
}

/* Splice a new node into an existing linked list of linestructs. */
void splice_node(linestruct *afterthis, linestruct *newnode)
{
	newnode->next = afterthis->next;
	newnode->prev = afterthis;
	if (afterthis->next != NULL)
		afterthis->next->prev = newnode;
	afterthis->next = newnode;

	/* Update filebot when inserting a node at the end of file. */
	if (openfile && openfile->filebot == afterthis)
		openfile->filebot = newnode;
}

/* Disconnect a node from a linked list of linestructs and delete it. */
void unlink_node(linestruct *fileptr)
{
	if (fileptr->prev != NULL)
		fileptr->prev->next = fileptr->next;
	if (fileptr->next != NULL)
		fileptr->next->prev = fileptr->prev;

	/* Update filebot when removing a node at the end of file. */
	if (openfile && openfile->filebot == fileptr)
		openfile->filebot = fileptr->prev;

	delete_node(fileptr);
}

/* Free the data structures in the given node. */
void delete_node(linestruct *fileptr)
{
#ifdef ENABLE_WRAPPING
	/* If the spill-over line for hard-wrapping is deleted... */
	if (fileptr == openfile->spillage_line)
		openfile->spillage_line = NULL;
#endif
	free(fileptr->data);
#ifdef ENABLE_COLOR
	free(fileptr->multidata);
#endif
	free(fileptr);
}

/* Duplicate an entire linked list of linestructs. */
linestruct *copy_buffer(const linestruct *src)
{
	linestruct *head, *copy;

	copy = copy_node(src);
	copy->prev = NULL;
	head = copy;
	src = src->next;

	while (src != NULL) {
		copy->next = copy_node(src);
		copy->next->prev = copy;
		copy = copy->next;

		src = src->next;
	}

	copy->next = NULL;

	return head;
}

/* Free an entire linked list of linestructs. */
void free_lines(linestruct *src)
{
	if (src == NULL)
		return;

	while (src->next != NULL) {
		src = src->next;
		delete_node(src->prev);
	}

	delete_node(src);
}

/* Renumber the lines in a buffer, starting with the given line. */
void renumber(linestruct *line)
{
	ssize_t number = (line->prev == NULL) ? 0 : line->prev->lineno;

	while (line != NULL) {
		line->lineno = ++number;
		line = line->next;
	}
}

/* Partition the current buffer so that it appears to begin at (top, top_x)
 * and appears to end at (bot, bot_x). */
partition *partition_buffer(linestruct *top, size_t top_x,
		linestruct *bot, size_t bot_x)
{
	partition *p = nmalloc(sizeof(partition));

	/* If the top and bottom of the partition are different from the top
	 * and bottom of the buffer, save the latter and then set them
	 * to top and bot. */
	if (top != openfile->filetop) {
		p->filetop = openfile->filetop;
		openfile->filetop = top;
	} else
		p->filetop = NULL;
	if (bot != openfile->filebot) {
		p->filebot = openfile->filebot;
		openfile->filebot = bot;
	} else
		p->filebot = NULL;

	/* Remember which line is above the top of the partition, detach the
	 * top of the partition from it, and save the text before top_x. */
	p->top_prev = top->prev;
	top->prev = NULL;
	p->top_data = mallocstrncpy(NULL, top->data, top_x + 1);
	p->top_data[top_x] = '\0';

	/* Remember which line is below the bottom of the partition, detach the
	 * bottom of the partition from it, and save the text after bot_x. */
	p->bot_next = bot->next;
	bot->next = NULL;
	p->bot_data = mallocstrcpy(NULL, bot->data + bot_x);

	/* Remove all text after bot_x at the bottom of the partition. */
	bot->data[bot_x] = '\0';

	/* Remove all text before top_x at the top of the partition. */
	charmove(top->data, top->data + top_x, strlen(top->data) - top_x + 1);

	/* Return the partition. */
	return p;
}

/* Unpartition the current buffer so that it stretches from (filetop, 0)
 * to (filebot, $) again. */
void unpartition_buffer(partition **p)
{
	/* Reattach the line above the top of the partition, and restore the
	 * text before top_x from top_data.  Free top_data when we're done
	 * with it. */
	openfile->filetop->prev = (*p)->top_prev;
	if (openfile->filetop->prev != NULL)
		openfile->filetop->prev->next = openfile->filetop;
	openfile->filetop->data = charealloc(openfile->filetop->data,
				strlen((*p)->top_data) + strlen(openfile->filetop->data) + 1);
	charmove(openfile->filetop->data + strlen((*p)->top_data),
				openfile->filetop->data, strlen(openfile->filetop->data) + 1);
	strncpy(openfile->filetop->data, (*p)->top_data, strlen((*p)->top_data));
	free((*p)->top_data);

	/* Reattach the line below the bottom of the partition, and restore
	 * the text after bot_x from bot_data.  Free bot_data when we're
	 * done with it. */
	openfile->filebot->next = (*p)->bot_next;
	if (openfile->filebot->next != NULL)
		openfile->filebot->next->prev = openfile->filebot;
	openfile->filebot->data = charealloc(openfile->filebot->data,
				strlen(openfile->filebot->data) + strlen((*p)->bot_data) + 1);
	strcat(openfile->filebot->data, (*p)->bot_data);
	free((*p)->bot_data);

	/* Restore the top and bottom of the buffer, if they were
	 * different from the top and bottom of the partition. */
	if ((*p)->filetop != NULL)
		openfile->filetop = (*p)->filetop;
	if ((*p)->filebot != NULL)
		openfile->filebot = (*p)->filebot;

	/* Uninitialize the partition. */
	free(*p);
	*p = NULL;
}

/* Move all the text between (top, top_x) and (bot, bot_x) in the
 * current buffer to a new buffer beginning with file_top and ending
 * with file_bot.  If no text is between (top, top_x) and (bot, bot_x),
 * don't do anything. */
void extract_buffer(linestruct **file_top, linestruct **file_bot,
		linestruct *top, size_t top_x, linestruct *bot, size_t bot_x)
{
	linestruct *top_save;
	bool edittop_inside;
#ifndef NANO_TINY
	bool mark_inside = FALSE;
	bool same_line = FALSE;
#endif

	/* If (top, top_x)-(bot, bot_x) doesn't cover any text, get out. */
	if (top == bot && top_x == bot_x)
		return;

	/* Partition the buffer so that it contains only the text from
	 * (top, top_x) to (bot, bot_x), keep track of whether the top of
	 * the edit window is inside the partition, and keep track of
	 * whether the mark begins inside the partition. */
	filepart = partition_buffer(top, top_x, bot, bot_x);
	edittop_inside = (openfile->edittop->lineno >= openfile->filetop->lineno &&
						openfile->edittop->lineno <= openfile->filebot->lineno);
#ifndef NANO_TINY
	if (openfile->mark) {
		mark_inside = (openfile->mark->lineno >= openfile->filetop->lineno &&
						openfile->mark->lineno <= openfile->filebot->lineno &&
						(openfile->mark != openfile->filetop ||
												openfile->mark_x >= top_x) &&
						(openfile->mark != openfile->filebot ||
												openfile->mark_x <= bot_x));
		same_line = (openfile->mark == openfile->filetop);
	}
#endif

	/* Subtract the number of characters in the text from the file size. */
	openfile->totsize -= get_totsize(top, bot);

	if (*file_top == NULL) {
		/* If file_top is empty, just move all the text directly into
		 * it.  This is equivalent to tacking the text in top onto the
		 * (lack of) text at the end of file_top. */
		*file_top = openfile->filetop;
		*file_bot = openfile->filebot;

		/* Renumber, starting with file_top. */
		renumber(*file_top);
	} else {
		linestruct *file_bot_save = *file_bot;

		/* Otherwise, tack the text in top onto the text at the end of
		 * file_bot. */
		(*file_bot)->data = charealloc((*file_bot)->data,
				strlen((*file_bot)->data) +
				strlen(openfile->filetop->data) + 1);
		strcat((*file_bot)->data, openfile->filetop->data);

		/* Attach the line after top to the line after file_bot.  Then,
		 * if there's more than one line after top, move file_bot down
		 * to bot. */
		(*file_bot)->next = openfile->filetop->next;
		if ((*file_bot)->next != NULL) {
			(*file_bot)->next->prev = *file_bot;
			*file_bot = openfile->filebot;
		}

		delete_node(openfile->filetop);

		/* Renumber, starting at the last line of the original buffer. */
		renumber(file_bot_save);
	}

	/* Since the text has now been saved, remove it from the buffer. */
	openfile->filetop = make_new_node(NULL);
	openfile->filetop->data = mallocstrcpy(NULL, "");
	openfile->filebot = openfile->filetop;

	/* Restore the current line and cursor position.  If the mark begins
	 * inside the partition, set the beginning of the mark to where the
	 * saved text used to start. */
	openfile->current = openfile->filetop;
	openfile->current_x = top_x;
#ifndef NANO_TINY
	if (mark_inside) {
		openfile->mark = openfile->current;
		openfile->mark_x = openfile->current_x;
	} else if (same_line)
		/* Update the pointer to this partially cut line. */
		openfile->mark = openfile->current;
#endif

	top_save = openfile->filetop;

	/* Unpartition the buffer so that it contains all the text
	 * again, minus the saved text. */
	unpartition_buffer(&filepart);

	/* If the top of the edit window was inside the old partition, put
	 * it in range of current. */
	if (edittop_inside) {
		adjust_viewport(STATIONARY);
		refresh_needed = TRUE;
	}

	/* Renumber, starting with the beginning line of the old partition. */
	renumber(top_save);

	/* If the text doesn't end with a newline, and it should, add one. */
	if (!ISSET(NO_NEWLINES) && openfile->filebot->data[0] != '\0')
		new_magicline();
}

/* Meld the given buffer into the current file buffer
 * at the current cursor position. */
void ingraft_buffer(linestruct *somebuffer)
{
	linestruct *top_save;
	size_t current_x_save = openfile->current_x;
	bool edittop_inside;
#ifndef NANO_TINY
	bool right_side_up = FALSE, single_line = FALSE;
#endif

#ifndef NANO_TINY
	/* Keep track of whether the mark begins inside the partition and
	 * will need adjustment. */
	if (openfile->mark) {
		linestruct *top, *bot;
		size_t top_x, bot_x;

		mark_order((const linestruct **)&top, &top_x,
						(const linestruct **)&bot, &bot_x, &right_side_up);

		single_line = (top == bot);
	}
#endif

	/* Partition the buffer so that it contains no text, and remember
	 * whether the current line is at the top of the edit window. */
	filepart = partition_buffer(openfile->current, openfile->current_x,
								openfile->current, openfile->current_x);
	edittop_inside = (openfile->edittop == openfile->filetop);
	free_lines(openfile->filetop);

	/* Put the top and bottom of the current buffer at the top and
	 * bottom of the passed buffer. */
	openfile->filetop = somebuffer;
	openfile->filebot = openfile->filetop;
	while (openfile->filebot->next != NULL)
		openfile->filebot = openfile->filebot->next;

	/* Put the cursor at the end of the pasted text. */
	openfile->current = openfile->filebot;
	openfile->current_x = strlen(openfile->filebot->data);

	/* Refresh the mark's pointer, and compensate the mark's
	 * x coordinate for the change in the current line. */
	if (openfile->filetop == openfile->filebot) {
#ifndef NANO_TINY
		if (openfile->mark && single_line) {
			openfile->mark = openfile->current;
			if (!right_side_up)
				openfile->mark_x += openfile->current_x;
		}
#endif
		/* When the pasted stuff contains no newline, adjust the cursor's
		 * x coordinate for the text that is before the pasted stuff. */
		openfile->current_x += current_x_save;
	}
#ifndef NANO_TINY
	else if (openfile->mark && single_line) {
		if (right_side_up)
			openfile->mark = openfile->filetop;
		else {
			openfile->mark = openfile->current;
			openfile->mark_x += openfile->current_x - current_x_save;
		}
	}
#endif

#ifdef DEBUG
#include <time.h>
	clock_t start = clock();
#endif
	/* Add the number of characters in the copied text to the file size. */
	openfile->totsize += get_totsize(openfile->filetop, openfile->filebot);
#ifdef DEBUG
	statusline(ALERT, "Took: %.2f", (double)(clock() - start) / CLOCKS_PER_SEC);
#endif

	/* If we pasted onto the first line of the edit window, the corresponding
	 * record has been freed, so... point at the start of the copied text. */
	if (edittop_inside)
		openfile->edittop = openfile->filetop;

	top_save = openfile->filetop;

	/* Unpartition the buffer so that it contains all the text
	 * again, plus the copied text. */
	unpartition_buffer(&filepart);

	/* Renumber, starting with the beginning line of the old partition. */
	renumber(top_save);

	/* If the text doesn't end with a newline, and it should, add one. */
	if (!ISSET(NO_NEWLINES) && openfile->filebot->data[0] != '\0')
		new_magicline();
}

/* Meld a copy of the given buffer into the current file buffer. */
void copy_from_buffer(linestruct *somebuffer)
{
	linestruct *the_copy = copy_buffer(somebuffer);

	ingraft_buffer(the_copy);
}

#ifdef ENABLE_MULTIBUFFER
/* Unlink a node from the rest of the circular list, and delete it. */
void unlink_opennode(openfilestruct *fileptr)
{
	if (fileptr == startfile)
		startfile = startfile->next;

	fileptr->prev->next = fileptr->next;
	fileptr->next->prev = fileptr->prev;

	delete_opennode(fileptr);
}

/* Free all the memory in the given open-file node. */
void delete_opennode(openfilestruct *fileptr)
{
	free(fileptr->filename);
	free_lines(fileptr->filetop);
#ifndef NANO_TINY
	free(fileptr->current_stat);
	free(fileptr->lock_filename);
	/* Free the undo stack. */
	discard_until(NULL, fileptr, TRUE);
#endif
	free(fileptr);
}
#endif /* ENABLE_MULTIBUFFER */

/* Display a warning about a key disabled in view mode. */
void print_view_warning(void)
{
	statusbar(_("Key is invalid in view mode"));
}

/* Indicate that something is disabled in restricted mode. */
void show_restricted_warning(void)
{
	statusbar(_("This function is disabled in restricted mode"));
	beep();
}

#ifndef ENABLE_HELP
/* Indicate that help texts are unavailable. */
void say_there_is_no_help(void)
{
	statusbar(_("Help is not available"));
}
#endif

/* Exit normally: restore the terminal state and save history files. */
void finish(void)
{
	/* Blank the statusbar and (if applicable) the shortcut list. */
	blank_statusbar();
	blank_bottombars();
	wrefresh(bottomwin);

#ifndef NANO_TINY
	/* Deallocate the two or three subwindows. */
	if (topwin != NULL)
		delwin(topwin);
	delwin(edit);
	delwin(bottomwin);
#endif
	/* Switch on the cursor and exit from curses mode. */
	curs_set(1);
	endwin();

	/* Restore the old terminal settings. */
	tcsetattr(0, TCSANOW, &oldterm);

#ifdef ENABLE_HISTORIES
	/* If the user wants history persistence, write the relevant files. */
	if (ISSET(HISTORYLOG))
		save_history();
	if (ISSET(POSITIONLOG)) {
		update_poshistory(openfile->filename, openfile->current->lineno, xplustabs() + 1);
	}
#endif

	/* Get out. */
	exit(0);
}

/* Die gracefully -- by restoring the terminal state and saving any buffers
 * that were modified. */
void die(const char *msg, ...)
{
	va_list ap;
	openfilestruct *firstone = openfile;

	/* Switch on the cursor and leave curses mode. */
	curs_set(1);
	endwin();

	/* Restore the old terminal settings. */
	tcsetattr(0, TCSANOW, &oldterm);

	/* Display the dying message. */
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);

	while (openfile) {
#ifndef NANO_TINY
		/* If the current buffer has a lockfile, remove it. */
		if (ISSET(LOCKING) && openfile->lock_filename)
			delete_lockfile(openfile->lock_filename);
#endif
		/* If the current buffer was modified, ensure it is unpartitioned,
		 * then save it.  When in restricted mode, we don't save anything,
		 * because it would write files not mentioned on the command line. */
		if (openfile->modified && !ISSET(RESTRICTED)) {
			if (filepart != NULL)
				unpartition_buffer(&filepart);

			emergency_save(openfile->filename, openfile->current_stat);
		}

		filepart = NULL;
#ifdef ENABLE_MULTIBUFFER
		openfile = openfile->next;
#endif
		if (openfile == firstone)
			break;
	}

	/* Abandon the building. */
	exit(1);
}

/* Save the current buffer under the given name.
 * If necessary, the name is modified to be unique. */
void emergency_save(const char *die_filename, struct stat *die_stat)
{
	char *targetname;
	bool failed = TRUE;

	/* If the buffer has no name, simply call it "nano". */
	if (*die_filename == '\0')
		die_filename = "nano";

	targetname = get_next_filename(die_filename, ".save");

	if (*targetname != '\0')
		failed = !write_file(targetname, NULL, TRUE, OVERWRITE, FALSE);

	if (!failed)
		fprintf(stderr, _("\nBuffer written to %s\n"), targetname);
	else if (*targetname != '\0')
		fprintf(stderr, _("\nBuffer not written to %s: %s\n"), targetname,
				strerror(errno));
	else
		fprintf(stderr, _("\nBuffer not written: %s\n"),
				_("Too many backup files?"));

#ifndef NANO_TINY
	/* Try to chmod/chown the saved file to the values of the original file,
	 * but ignore any failure as we are in a hurry to get out. */
	if (die_stat) {
		IGNORE_CALL_RESULT(chmod(targetname, die_stat->st_mode));
		IGNORE_CALL_RESULT(chown(targetname, die_stat->st_uid,
												die_stat->st_gid));
	}
#endif

	free(targetname);
}

/* Initialize the three window portions nano uses. */
void window_init(void)
{
	/* When resizing, first delete the existing windows. */
	if (edit != NULL) {
		if (topwin != NULL)
			delwin(topwin);
		delwin(edit);
		delwin(bottomwin);
	}

	/* If the terminal is very flat, don't set up a titlebar. */
	if (LINES < 3) {
		topwin = NULL;
		editwinrows = 1;
		/* Set up two subwindows.  If the terminal is just one line,
		 * edit window and statusbar window will cover each other. */
		edit = newwin(1, COLS, 0, 0);
		bottomwin = newwin(1, COLS, LINES - 1, 0);
	} else {
		int toprows = (!ISSET(EMPTY_LINE) ? 1 : (LINES < 6) ? 1 : 2);
		int bottomrows = (ISSET(NO_HELP) ? 1 : (LINES < 5) ? 1 : 3);

		editwinrows = LINES - toprows - bottomrows;

		/* Set up the normal three subwindows. */
		topwin = newwin(toprows, COLS, 0, 0);
		edit = newwin(editwinrows, COLS, toprows, 0);
		bottomwin = newwin(bottomrows, COLS, toprows + editwinrows, 0);
	}

	/* In case the terminal shrunk, make sure the status line is clear. */
	wipe_statusbar();

	/* When not disabled, turn escape-sequence translation on. */
	if (!ISSET(RAW_SEQUENCES)) {
		keypad(topwin, TRUE);
		keypad(edit, TRUE);
		keypad(bottomwin, TRUE);
	}

#ifdef ENABLED_WRAPORJUSTIFY
	/* Set up the wrapping point, accounting for screen width when negative. */
	wrap_at = fill;
	if (wrap_at <= 0)
		wrap_at += COLS;
	if (wrap_at < 0)
		wrap_at = 0;
#endif
}

#ifdef ENABLE_MOUSE
void disable_mouse_support(void)
{
	mousemask(0, NULL);
	mouseinterval(oldinterval);
}

void enable_mouse_support(void)
{
	mousemask(ALL_MOUSE_EVENTS, NULL);
	oldinterval = mouseinterval(50);
}

/* Switch mouse support on or off, as needed. */
void mouse_init(void)
{
	if (ISSET(USE_MOUSE))
		enable_mouse_support();
	else
		disable_mouse_support();
}
#endif /* ENABLE_MOUSE */

/* Print the usage line for the given option to the screen. */
void print_opt(const char *shortflag, const char *longflag, const char *desc)
{
	printf(" %s\t", shortflag);
	if (breadth(shortflag) < 8)
		printf("\t");

	printf("%s\t", longflag);
	if (breadth(longflag) < 8)
		printf("\t\t");
	else if (breadth(longflag) < 16)
		printf("\t");

	printf("%s\n", _(desc));
}

/* Explain how to properly use nano and its command-line options. */
void usage(void)
{
	printf(_("Usage: nano [OPTIONS] [[+LINE[,COLUMN]] FILE]...\n\n"));
	/* TRANSLATORS: The next two strings are part of the --help output.
	 * It's best to keep its lines within 80 characters. */
	printf(_("To place the cursor on a specific line of a file, put the line number with\n"
				"a '+' before the filename.  The column number can be added after a comma.\n"));
	printf(_("When a filename is '-', nano reads data from standard input.\n\n"));
	printf(_("Option\t\tGNU long option\t\tMeaning\n"));
#ifndef NANO_TINY
	/* TRANSLATORS: The next forty or so strings are option descriptions
	 * for the --help output.  Try to keep them at most 40 characters. */
	print_opt("-A", "--smarthome", N_("Enable smart home key"));
	if (!ISSET(RESTRICTED)) {
		print_opt("-B", "--backup", N_("Save backups of existing files"));
		print_opt(_("-C <dir>"), _("--backupdir=<dir>"),
					N_("Directory for saving unique backup files"));
	}
#endif
	print_opt("-D", "--boldtext", N_("Use bold instead of reverse video text"));
#ifndef NANO_TINY
	print_opt("-E", "--tabstospaces", N_("Convert typed tabs to spaces"));
#endif
#ifdef ENABLE_MULTIBUFFER
	if (!ISSET(RESTRICTED))
		print_opt("-F", "--multibuffer",
					N_("Read a file into a new buffer by default"));
#endif
#ifndef NANO_TINY
	print_opt("-G", "--locking", N_("Use (vim-style) lock files"));
#endif
#ifdef ENABLE_HISTORIES
	if (!ISSET(RESTRICTED))
		print_opt("-H", "--historylog",
					N_("Log & read search/replace string history"));
#endif
#ifdef ENABLE_NANORC
	print_opt("-I", "--ignorercfiles", N_("Don't look at nanorc files"));
#endif
#ifndef NANO_TINY
	print_opt("-J <number>", "--guidestripe=<number>",
					N_("Show a guiding bar at this column"));
#endif
	print_opt("-K", "--rawsequences",
					N_("Fix numeric keypad key confusion problem"));
#ifndef NANO_TINY
	print_opt("-L", "--nonewlines",
					N_("Don't add an automatic newline"));
#endif
#ifdef ENABLED_WRAPORJUSTIFY
	print_opt("-M", "--trimblanks",
					N_("Trim tail spaces when hard-wrapping"));
#endif
#ifndef NANO_TINY
	print_opt("-N", "--noconvert",
					N_("Don't convert files from DOS/Mac format"));
#endif
#ifdef ENABLE_HISTORIES
	if (!ISSET(RESTRICTED))
		print_opt("-P", "--positionlog",
					N_("Log & read location of cursor position"));
#endif
#ifdef ENABLE_JUSTIFY
	print_opt(_("-Q <regex>"), _("--quotestr=<regex>"),
					 N_("Regular expression to match quoting"));
#endif
	if (!ISSET(RESTRICTED))
		print_opt("-R", "--restricted", N_("Restricted mode"));
	print_opt(_("-T <#cols>"), _("--tabsize=<#cols>"),
					N_("Set width of a tab to #cols columns"));
	print_opt("-U", "--quickblank", N_("Do quick statusbar blanking"));
	print_opt("-V", "--version", N_("Print version information and exit"));
#ifndef NANO_TINY
	print_opt("-W", "--wordbounds",
					N_("Detect word boundaries more accurately"));
	print_opt(_("-X <str>"), _("--wordchars=<str>"),
					N_("Which other characters are word parts"));
#endif
#ifdef ENABLE_COLOR
	if (!ISSET(RESTRICTED))
		print_opt(_("-Y <name>"), _("--syntax=<name>"),
					N_("Syntax definition to use for coloring"));
#endif
#ifndef NANO_TINY
	print_opt("-Z", "--zap", N_("Let Bsp and Del erase a marked region"));
	print_opt("-a", "--atblanks", N_("When soft-wrapping, do it at whitespace"));
#endif
#ifdef ENABLE_WRAPPING
	print_opt("-b", "--breaklonglines", N_("Automatically hard-wrap overlong lines"));
#endif
	print_opt("-c", "--constantshow", N_("Constantly show cursor position"));
	print_opt("-d", "--rebinddelete",
					N_("Fix Backspace/Delete confusion problem"));
	print_opt("-e", "--emptyline", N_("Keep the line below the title bar empty"));
#ifdef ENABLE_BROWSER
	if (!ISSET(RESTRICTED))
		print_opt("-g", "--showcursor", N_("Show cursor in file browser & help text"));
#endif
	print_opt("-h", "--help", N_("Show this help text and exit"));
#ifndef NANO_TINY
	print_opt("-i", "--autoindent", N_("Automatically indent new lines"));
#endif
	print_opt("-j", "--jumpyscrolling", N_("Scroll per half-screen, not per line"));
#ifndef NANO_TINY
	print_opt("-k", "--cutfromcursor", N_("Cut from cursor to end of line"));
#endif
#ifdef ENABLE_LINENUMBERS
	print_opt("-l", "--linenumbers", N_("Show line numbers in front of the text"));
#endif
#ifdef ENABLE_MOUSE
	print_opt("-m", "--mouse", N_("Enable the use of the mouse"));
#endif
	print_opt("-n", "--noread", N_("Do not read the file (only write it)"));
#ifdef ENABLE_OPERATINGDIR
	print_opt(_("-o <dir>"), _("--operatingdir=<dir>"),
					N_("Set operating directory"));
#endif
	print_opt("-p", "--preserve", N_("Preserve XON (^Q) and XOFF (^S) keys"));
#ifdef ENABLED_WRAPORJUSTIFY
	print_opt(_("-r <#cols>"), _("--fill=<#cols>"),
					N_("Set width for hard-wrap and justify"));
#endif
#ifdef ENABLE_SPELLER
	if (!ISSET(RESTRICTED))
		print_opt(_("-s <prog>"), _("--speller=<prog>"),
					N_("Enable alternate speller"));
#endif
	print_opt("-t", "--tempfile", N_("Auto save on exit, don't prompt"));
#ifndef NANO_TINY
	print_opt("-u", "--unix", N_("Save a file by default in Unix format"));
#endif
	print_opt("-v", "--view", N_("View mode (read-only)"));
#ifdef ENABLE_WRAPPING
	print_opt("-w", "--nowrap", N_("Don't hard-wrap long lines [default]"));
#endif
	print_opt("-x", "--nohelp", N_("Don't show the two help lines"));
#ifndef NANO_TINY
	print_opt("-y", "--afterends", N_("Make Ctrl+Right stop at word ends"));
#endif
	if (!ISSET(RESTRICTED))
		print_opt("-z", "--suspend", N_("Enable suspension"));
#ifndef NANO_TINY
	print_opt("-$", "--softwrap", N_("Enable soft line wrapping"));
#endif
}

/* Display the current version of nano, the date and time it was
 * compiled, contact information for it, and the configuration options
 * it was compiled with. */
void version(void)
{
#ifdef REVISION
	printf(" GNU nano from git, %s\n", REVISION);
#else
	printf(_(" GNU nano, version %s\n"), VERSION);
#endif
	printf(" (C) 1999-2011, 2013-2019 Free Software Foundation, Inc.\n");
	printf(_(" (C) 2014-%s the contributors to nano\n"), "2019");
	printf(_(" Email: nano@nano-editor.org	Web: https://nano-editor.org/"));
	printf(_("\n Compiled options:"));

#ifdef NANO_TINY
	printf(" --enable-tiny");
#ifdef ENABLE_BROWSER
	printf(" --enable-browser");
#endif
#ifdef ENABLE_COLOR
	printf(" --enable-color");
#endif
#ifdef ENABLE_EXTRA
	printf(" --enable-extra");
#endif
#ifdef ENABLE_HELP
	printf(" --enable-help");
#endif
#ifdef ENABLE_HISTORIES
	printf(" --enable-histories");
#endif
#ifdef ENABLE_JUSTIFY
	printf(" --enable-justify");
#endif
#ifdef HAVE_LIBMAGIC
	printf(" --enable-libmagic");
#endif
#ifdef ENABLE_LINENUMBERS
	printf(" --enable-linenumbers");
#endif
#ifdef ENABLE_MOUSE
	printf(" --enable-mouse");
#endif
#ifdef ENABLE_NANORC
	printf(" --enable-nanorc");
#endif
#ifdef ENABLE_MULTIBUFFER
	printf(" --enable-multibuffer");
#endif
#ifdef ENABLE_OPERATINGDIR
	printf(" --enable-operatingdir");
#endif
#ifdef ENABLE_SPELLER
	printf(" --enable-speller");
#endif
#ifdef ENABLE_TABCOMP
	printf(" --enable-tabcomp");
#endif
#ifdef ENABLE_WRAPPING
	printf(" --enable-wrapping");
#endif
#else /* !NANO_TINY */
#ifndef ENABLE_BROWSER
	printf(" --disable-browser");
#endif
#ifndef ENABLE_COLOR
	printf(" --disable-color");
#endif
#ifndef ENABLE_COMMENT
	printf(" --disable-comment");
#endif
#ifndef ENABLE_EXTRA
	printf(" --disable-extra");
#endif
#ifndef ENABLE_HELP
	printf(" --disable-help");
#endif
#ifndef ENABLE_HISTORIES
	printf(" --disable-histories");
#endif
#ifndef ENABLE_JUSTIFY
	printf(" --disable-justify");
#endif
#ifndef HAVE_LIBMAGIC
	printf(" --disable-libmagic");
#endif
#ifndef ENABLE_LINENUMBERS
	printf(" --disable-linenumbers");
#endif
#ifndef ENABLE_MOUSE
	printf(" --disable-mouse");
#endif
#ifndef ENABLE_MULTIBUFFER
	printf(" --disable-multibuffer");
#endif
#ifndef ENABLE_NANORC
	printf(" --disable-nanorc");
#endif
#ifndef ENABLE_OPERATINGDIR
	printf(" --disable-operatingdir");
#endif
#ifndef ENABLE_SPELLER
	printf(" --disable-speller");
#endif
#ifndef ENABLE_TABCOMP
	printf(" --disable-tabcomp");
#endif
#ifndef ENABLE_WORDCOMPLETION
	printf(" --disable-wordcomp");
#endif
#ifndef ENABLE_WRAPPING
	printf(" --disable-wrapping");
#endif
#endif /* !NANO_TINY */

#ifdef DEBUG
	printf(" --enable-debug");
#endif
#ifndef ENABLE_NLS
	printf(" --disable-nls");
#endif
#ifdef ENABLE_UTF8
	printf(" --enable-utf8");
#else
	printf(" --disable-utf8");
#endif
#ifdef USE_SLANG
	printf(" --with-slang");
#endif
	printf("\n");
}

/* If the current file buffer has been modified, and the TEMP_FILE flag
 * isn't set, ask whether or not to save the file buffer.  If the
 * TEMP_FILE flag is set and the current file has a name, save it
 * unconditionally.  Then, if more than one file buffer is open, close
 * the current file buffer and switch to the next one.  If only one file
 * buffer is open, exit from nano. */
void do_exit(void)
{
	int choice;

	/* If the file hasn't been modified, pretend the user chose not to
	 * save. */
	if (!openfile->modified)
		choice = 0;
	/* If the TEMP_FILE flag is set and the current file has a name,
	 * pretend the user chose to save. */
	else if (openfile->filename[0] != '\0' && ISSET(TEMP_FILE))
		choice = 1;
	/* Otherwise, ask the user whether or not to save. */
	else {
		/* If the TEMP_FILE flag is set, and the current file doesn't
		 * have a name, warn the user before prompting for a name. */
		if (ISSET(TEMP_FILE))
			warn_and_shortly_pause(_("No file name"));

		choice = do_yesno_prompt(FALSE, _("Save modified buffer? "));
	}

	/* If the user chose not to save, or if the user chose to save and
	 * the save succeeded, we're ready to exit. */
	if (choice == 0 || (choice == 1 && do_writeout(TRUE, TRUE) > 0))
		close_and_go();
	else if (choice != 1)
		statusbar(_("Cancelled"));
}

/* Close the current buffer, and terminate nano if it was the last. */
void close_and_go(void)
{
#ifndef NANO_TINY
	/* If there is a lockfile, remove it. */
	if (ISSET(LOCKING) && openfile->lock_filename)
		delete_lockfile(openfile->lock_filename);
#endif
#ifdef ENABLE_MULTIBUFFER
	/* If there are no more open file buffers, jump off a cliff. */
	if (!close_buffer())
#endif
		finish();
}

/* Make a note that reading from stdin was concluded with ^C. */
RETSIGTYPE make_a_note(int signal)
{
	input_was_aborted = TRUE;
}

/* Read whatever comes from standard input into a new buffer. */
bool scoop_stdin(void)
{
	struct sigaction oldaction, newaction;
		/* Original and temporary handlers for SIGINT. */
	bool setup_failed = FALSE;
		/* Whether setting up the temporary SIGINT handler failed. */
	FILE *stream;
	int thetty;

	/* Exit from curses mode and put the terminal into its original state. */
	endwin();
	tcsetattr(0, TCSANOW, &oldterm);

	fprintf(stderr, _("Reading from standard input; type ^D or ^D^D to finish.\n"));

#ifndef NANO_TINY
	/* Enable interpretation of the special control keys so that
	 * we get SIGINT when Ctrl-C is pressed. */
	enable_signals();
#endif

	/* Set things up so that SIGINT will cancel the reading. */
	if (sigaction(SIGINT, NULL, &newaction) == -1) {
		setup_failed = TRUE;
		perror("sigaction");
	} else {
		newaction.sa_handler = make_a_note;
		if (sigaction(SIGINT, &newaction, &oldaction) == -1) {
			setup_failed = TRUE;
			perror("sigaction");
		}
	}

	/* Open standard input. */
	stream = fopen("/dev/stdin", "rb");
	if (stream == NULL) {
		int errnumber = errno;

		terminal_init();
		doupdate();
		statusline(ALERT, _("Failed to open stdin: %s"), strerror(errnumber));
		return FALSE;
	}

	/* Read the input into a new buffer. */
	open_buffer("", TRUE);
	read_file(stream, 0, "stdin", TRUE);
	openfile->edittop = openfile->filetop;
	fprintf(stderr, ".\n");

	/* Reconnect the tty as the input source. */
	thetty = open("/dev/tty", O_RDONLY);
	if (!thetty)
		die(_("Couldn't reopen stdin from keyboard, sorry\n"));
	dup2(thetty, 0);
	close(thetty);

	/* If things went well, store the current state of the terminal. */
	if (!input_was_aborted)
		tcgetattr(0, &oldterm);

	/* If it was changed, restore the handler for SIGINT. */
	if (!setup_failed && sigaction(SIGINT, &oldaction, NULL) == -1)
		perror("sigaction");

	terminal_init();
	doupdate();

	if (!ISSET(VIEW_MODE) && openfile->totsize > 0)
		set_modified();

	return TRUE;
}

/* Register half a dozen signal handlers. */
void signal_init(void)
{
	/* Trap SIGINT and SIGQUIT because we want them to do useful things. */
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = SIG_IGN;
	sigaction(SIGINT, &act, NULL);
#ifdef SIGQUIT
	sigaction(SIGQUIT, &act, NULL);
#endif

	/* Trap SIGHUP and SIGTERM because we want to write the file out. */
	act.sa_handler = handle_hupterm;
#ifdef SIGHUP
	sigaction(SIGHUP, &act, NULL);
#endif
	sigaction(SIGTERM, &act, NULL);

#ifndef NANO_TINY
	/* Trap SIGWINCH because we want to handle window resizes. */
	act.sa_handler = handle_sigwinch;
	sigaction(SIGWINCH, &act, NULL);
#endif

	if (ISSET(SUSPEND)) {
		/* Block all other signals in the suspend and continue handlers.
		 * If we don't do this, other stuff interrupts them! */
		sigfillset(&act.sa_mask);
#ifdef SIGTSTP
		/* Trap a normal suspend (^Z) so we can handle it ourselves. */
		act.sa_handler = do_suspend;
		sigaction(SIGTSTP, &act, NULL);
#endif
#ifdef SIGCONT
		act.sa_handler = do_continue;
		sigaction(SIGCONT, &act, NULL);
#endif
	} else {
#ifdef SIGTSTP
		act.sa_handler = SIG_IGN;
		sigaction(SIGTSTP, &act, NULL);
#endif
	}

#if !defined(NANO_TINY) && !defined(DEBUG)
	if (getenv("NANO_NOCATCH") == NULL) {
		/* Trap SIGSEGV and SIGABRT to save any changed buffers and reset
		 * the terminal to a usable state.  Reset these handlers to their
		 * defaults as soon as their signal fires. */
		act.sa_handler = handle_crash;
		act.sa_flags |= SA_RESETHAND;
		sigaction(SIGSEGV, &act, NULL);
		sigaction(SIGABRT, &act, NULL);
	}
#endif
}

/* Handler for SIGHUP (hangup) and SIGTERM (terminate). */
RETSIGTYPE handle_hupterm(int signal)
{
	die(_("Received SIGHUP or SIGTERM\n"));
}

#if !defined(NANO_TINY) && !defined(DEBUG)
/* Handler for SIGSEGV (segfault) and SIGABRT (abort). */
RETSIGTYPE handle_crash(int signal)
{
	die(_("Sorry! Nano crashed!  Code: %d.  Please report a bug.\n"), signal);
}
#endif

/* Handler for SIGTSTP (suspend). */
RETSIGTYPE do_suspend(int signal)
{
#ifdef ENABLE_MOUSE
	disable_mouse_support();
#endif

	/* Move the cursor to the last line of the screen. */
	move(LINES - 1, 0);
	endwin();

	/* Display our helpful message. */
	printf(_("Use \"fg\" to return to nano.\n"));
	fflush(stdout);

	/* Restore the old terminal settings. */
	tcsetattr(0, TCSANOW, &oldterm);

	/* The suspend keystroke must not elicit cursor-position display. */
	suppress_cursorpos=TRUE;

#ifdef SIGSTOP
	/* Do what mutt does: send ourselves a SIGSTOP. */
	kill(0, SIGSTOP);
#endif
}

/* Put nano to sleep (if suspension is enabled). */
void do_suspend_void(void)
{
	if (ISSET(SUSPEND))
		do_suspend(0);
	else {
		statusbar(_("Suspension is not enabled"));
		beep();
	}
}

/* Handler for SIGCONT (continue after suspend). */
RETSIGTYPE do_continue(int signal)
{
#ifdef ENABLE_MOUSE
	if (ISSET(USE_MOUSE))
		enable_mouse_support();
#endif

#ifndef NANO_TINY
	/* Perhaps the user resized the window while we slept. */
	the_window_resized = TRUE;
#else
	/* Put the terminal in the desired state again. */
	terminal_init();
#endif
	/* Tickle the input routine so it will update the screen. */
	ungetch(KEY_FLUSH);
}

#if !defined(NANO_TINY) || defined(ENABLE_SPELLER)
/* Block or unblock the SIGWINCH signal, depending on the blockit parameter. */
void block_sigwinch(bool blockit)
{
	sigset_t winch;

	sigemptyset(&winch);
	sigaddset(&winch, SIGWINCH);
	sigprocmask(blockit ? SIG_BLOCK : SIG_UNBLOCK, &winch, NULL);

#ifndef NANO_TINY
	if (the_window_resized)
		regenerate_screen();
#endif
}
#endif

#ifndef NANO_TINY
/* Handler for SIGWINCH (window size change). */
RETSIGTYPE handle_sigwinch(int signal)
{
	/* Let the input routine know that a SIGWINCH has occurred. */
	the_window_resized = TRUE;
}

/* Reinitialize and redraw the screen completely. */
void regenerate_screen(void)
{
	const char *tty = ttyname(0);
	int fd, result = 0;
	struct winsize win;

	/* Reset the trigger. */
	the_window_resized = FALSE;

	if (tty == NULL)
		return;
	fd = open(tty, O_RDWR);
	if (fd == -1)
		return;
	result = ioctl(fd, TIOCGWINSZ, &win);
	close(fd);
	if (result == -1)
		return;

	/* We could check whether the COLS or LINES changed, and return
	 * otherwise.  However, COLS and LINES are curses global variables,
	 * and in some cases curses has already updated them.  But not in
	 * all cases.  Argh. */
#ifdef REDEFINING_MACROS_OK
	COLS = win.ws_col;
	LINES = win.ws_row;
#endif
	editwincols = COLS - margin;

	/* Ensure that firstcolumn is the starting column of its chunk. */
	ensure_firstcolumn_is_aligned();

#ifdef USE_SLANG
	/* Slang curses emulation brain damage, part 1: If we just do what
	 * curses does here, it'll only work properly if the resize made the
	 * window smaller.  Do what mutt does: Leave and immediately reenter
	 * Slang screen management mode. */
	SLsmg_reset_smg();
	SLsmg_init_smg();
#else
	/* Do the equivalent of what Minimum Profit does: leave and immediately
	 * reenter curses mode. */
	endwin();
	doupdate();
#endif

	/* Put the terminal in the desired state again, recreate the subwindows
	 * with their (new) sizes, and redraw the contents of these windows. */
	terminal_init();
	window_init();
	total_refresh();
}

/* Handle the global toggle specified in flag. */
void do_toggle(int flag)
{
	bool enabled;

	if (flag == SUSPEND && ISSET(RESTRICTED)) {
		show_restricted_warning();
		return;
	}

	TOGGLE(flag);

	switch (flag) {
#ifdef ENABLE_MOUSE
		case USE_MOUSE:
			mouse_init();
			break;
#endif
		case NO_HELP:
			window_init();
			focusing = FALSE;
			total_refresh();
			break;
		case SUSPEND:
			signal_init();
			break;
		case SOFTWRAP:
			if (!ISSET(SOFTWRAP))
				openfile->firstcolumn = 0;
			refresh_needed = TRUE;
			break;
		case WHITESPACE_DISPLAY:
			titlebar(NULL);  /* Fall through. */
#ifdef ENABLE_COLOR
		case NO_COLOR_SYNTAX:
#endif
			refresh_needed = TRUE;
			break;
	}

	enabled = ISSET(flag);

	if (flag == NO_HELP || flag == NO_COLOR_SYNTAX)
		enabled = !enabled;

	statusline(HUSH, "%s %s", _(flagtostr(flag)),
						enabled ? _("enabled") : _("disabled"));
}
#endif /* !NANO_TINY */

/* Disable extended input and output processing in our terminal
 * settings. */
void disable_extended_io(void)
{
#ifdef HAVE_TERMIOS_H
	struct termios term = {0};

	tcgetattr(0, &term);
	term.c_lflag &= ~IEXTEN;
	term.c_oflag &= ~OPOST;
	tcsetattr(0, TCSANOW, &term);
#endif
}

/* Disable interpretation of the special control keys in our terminal
 * settings. */
void disable_signals(void)
{
#ifdef HAVE_TERMIOS_H
	struct termios term = {0};

	tcgetattr(0, &term);
	term.c_lflag &= ~ISIG;
	tcsetattr(0, TCSANOW, &term);
#endif
}

#ifndef NANO_TINY
/* Enable interpretation of the special control keys in our terminal
 * settings. */
void enable_signals(void)
{
#ifdef HAVE_TERMIOS_H
	struct termios term = {0};

	tcgetattr(0, &term);
	term.c_lflag |= ISIG;
	tcsetattr(0, TCSANOW, &term);
#endif
}
#endif

/* Disable interpretation of the flow control characters in our terminal
 * settings. */
void disable_flow_control(void)
{
#ifdef HAVE_TERMIOS_H
	struct termios term;

	tcgetattr(0, &term);
	term.c_iflag &= ~IXON;
	tcsetattr(0, TCSANOW, &term);
#endif
}

/* Enable interpretation of the flow control characters in our terminal
 * settings. */
void enable_flow_control(void)
{
#ifdef HAVE_TERMIOS_H
	struct termios term;

	tcgetattr(0, &term);
	term.c_iflag |= IXON;
	tcsetattr(0, TCSANOW, &term);
#endif
}

/* Set up the terminal state.  Put the terminal in raw mode (read one
 * character at a time, disable the special control keys, and disable
 * the flow control characters), disable translation of carriage return
 * (^M) into newline (^J) so that we can tell the difference between the
 * Enter key and Ctrl-J, and disable echoing of characters as they're
 * typed.  Finally, disable extended input and output processing, and,
 * if we're not in preserve mode, reenable interpretation of the flow
 * control characters. */
void terminal_init(void)
{
#ifdef USE_SLANG
	/* Slang curses emulation brain damage, part 2: Slang doesn't
	 * implement raw(), nonl(), or noecho() properly, so there's no way
	 * to properly reinitialize the terminal using them.  We have to
	 * disable the special control keys and interpretation of the flow
	 * control characters using termios, save the terminal state after
	 * the first call, and restore it on subsequent calls. */
	static struct termios newterm;
	static bool newterm_set = FALSE;

	if (!newterm_set) {
#endif
		raw();
		nonl();
		noecho();
		disable_extended_io();
		if (ISSET(PRESERVE))
			enable_flow_control();

		disable_signals();
#ifdef USE_SLANG
		if (!ISSET(PRESERVE))
			disable_flow_control();

		tcgetattr(0, &newterm);
		newterm_set = TRUE;
	} else
		tcsetattr(0, TCSANOW, &newterm);
#endif
}

/* Ask ncurses for a keycode, or assign a default one. */
int get_keycode(const char *keyname, const int standard)
{
#ifdef HAVE_KEY_DEFINED
	const char *keyvalue = tigetstr(keyname);

	if (keyvalue != 0 && keyvalue != (char *)-1 && key_defined(keyvalue))
		return key_defined(keyvalue);
#endif
#ifdef DEBUG
	if (!ISSET(RAW_SEQUENCES))
		fprintf(stderr, "Using fallback keycode for %s\n", keyname);
#endif
	return standard;
}

#ifdef ENABLE_LINENUMBERS
/* Ensure that the margin can accomodate the buffer's highest line number. */
void confirm_margin(void)
{
	int needed_margin = digits(openfile->filebot->lineno) + 1;

	/* When not requested or space is too tight, suppress line numbers. */
	if (!ISSET(LINE_NUMBERS) || needed_margin > COLS - 4)
		needed_margin = 0;

	if (needed_margin != margin) {
		margin = needed_margin;
		editwincols = COLS - margin;

#ifndef NANO_TINY
		/* Ensure that firstcolumn is the starting column of its chunk. */
		ensure_firstcolumn_is_aligned();
#endif
		/* The margin has changed -- schedule a full refresh. */
		refresh_needed = TRUE;
	}
}
#endif /* ENABLE_LINENUMBERS */

/* Say that an unbound key was struck, and if possible which one. */
void unbound_key(int code)
{
	if (!is_byte(code))
		statusline(ALERT, _("Unbound key"));
	else if (meta_key) {
		if (code == '[')
			statusline(ALERT, _("Unbindable key: M-["));
		else
			statusline(ALERT, _("Unbound key: M-%c"), toupper(code));
	} else if (code == ESC_CODE)
		statusline(ALERT, _("Unbindable key: ^["));
	else if (code < 0x20)
		statusline(ALERT, _("Unbound key: ^%c"), code + 0x40);
	else
		statusline(ALERT, _("Unbound key: %c"), code);
}

#ifdef ENABLE_MOUSE
/* Handle a mouse click on the edit window or the shortcut list. */
int do_mouse(void)
{
	int click_row, click_col;
	int retval = get_mouseinput(&click_row, &click_col, TRUE);

	/* If the click is wrong or already handled, we're done. */
	if (retval != 0)
		return retval;

	/* If the click was in the edit window, put the cursor in that spot. */
	if (wmouse_trafo(edit, &click_row, &click_col, FALSE)) {
		linestruct *current_save = openfile->current;
		ssize_t row_count = click_row - openfile->current_y;
		size_t leftedge;
#ifndef NANO_TINY
		size_t current_x_save = openfile->current_x;
		bool sameline = (click_row == openfile->current_y);
			/* Whether the click was on the row where the cursor is. */

		if (ISSET(SOFTWRAP))
			leftedge = leftedge_for(xplustabs(), openfile->current);
		else
#endif
			leftedge = get_page_start(xplustabs());

		/* Move current up or down to the row that was clicked on. */
		if (row_count < 0)
			go_back_chunks(-row_count, &openfile->current, &leftedge);
		else
			go_forward_chunks(row_count, &openfile->current, &leftedge);

		openfile->current_x = actual_x(openfile->current->data,
								actual_last_column(leftedge, click_col));

#ifndef NANO_TINY
		/* Clicking where the cursor is toggles the mark, as does clicking
		 * beyond the line length with the cursor at the end of the line. */
		if (sameline && openfile->current_x == current_x_save)
			do_mark();
		else
#endif
			/* The cursor moved; clean the cutbuffer on the next cut. */
			keep_cutbuffer = FALSE;

		edit_redraw(current_save, CENTERING);
	}

	/* No more handling is needed. */
	return 2;
}
#endif /* ENABLE_MOUSE */

/* Return TRUE when the given function is a cursor-moving command. */
bool wanted_to_move(void (*func)(void))
{
	return func == do_left || func == do_right ||
			func == do_up || func == do_down ||
			func == do_home || func == do_end ||
			func == do_prev_word_void || func == do_next_word_void ||
#ifdef ENABLE_JUSTIFY
			func == do_para_begin_void || func == do_para_end_void ||
#endif
			func == do_prev_block || func == do_next_block ||
			func == do_page_up || func == do_page_down ||
			func == to_first_line || func == to_last_line;
}

/* Return TRUE when the given shortcut is admissible in view mode. */
bool okay_for_view(const keystruct *shortcut)
{
	const funcstruct *func = sctofunc(shortcut);

	return (func == NULL || func->viewok);
}

/* Read in a keystroke.  Act on the keystroke if it is a shortcut or a toggle;
 * otherwise, insert it into the edit buffer. */
void do_input(void)
{
	int input;
		/* The keystroke we read in: a character or a shortcut. */
	static char *puddle = NULL;
		/* The input buffer for actual characters. */
	static size_t depth = 0;
		/* The length of the input buffer. */
	bool retain_cuts = FALSE;
		/* Whether to conserve the current contents of the cutbuffer. */
	const keystruct *shortcut;

	/* Read in a keystroke, and show the cursor while waiting. */
	input = get_kbinput(edit, VISIBLE);

#ifndef NANO_TINY
	if (input == KEY_WINCH)
		return;
#endif

#ifdef ENABLE_MOUSE
	if (input == KEY_MOUSE) {
		/* We received a mouse click. */
		if (do_mouse() == 1)
			/* The click was on a shortcut -- read in the character
			 * that it was converted into. */
			input = get_kbinput(edit, BLIND);
		else
			/* The click was invalid or has been handled -- get out. */
			return;
	}
#endif

	/* Check for a shortcut in the main list. */
	shortcut = get_shortcut(&input);

	/* If we got a non-high-bit control key, a meta key sequence, or a
	 * function key, and it's not a shortcut or toggle, throw it out. */
	if (shortcut == NULL) {
		if (is_ascii_cntrl_char(input) || meta_key || !is_byte(input)) {
			unbound_key(input);
			input = ERR;
		}
	}

	/* If the keystroke isn't a shortcut nor a toggle, it's a normal text
	 * character: add the character to the input buffer -- or display a
	 * warning when we're in view mode. */
	if (input != ERR && shortcut == NULL) {
		if (ISSET(VIEW_MODE))
			print_view_warning();
		else {
			/* Store the byte, and leave room for a terminating zero. */
			puddle = charealloc(puddle, depth + 2);
			puddle[depth++] = (char)input;
		}
#ifndef NANO_TINY
		if (openfile->mark && openfile->kind_of_mark == SOFTMARK) {
			openfile->mark = NULL;
			refresh_needed = TRUE;
		}
#endif
	}

	/* If we got a shortcut or toggle, or if there aren't any other
	 * characters waiting after the one we read in, we need to output
	 * all available characters in the input puddle.  Note that this
	 * puddle will be empty if we're in view mode. */
	if (shortcut || get_key_buffer_len() == 0) {
		if (puddle != NULL) {
			/* Insert all bytes in the input buffer into the edit buffer
			 * at once, filtering out any low control codes. */
			puddle[depth] = '\0';
			do_output(puddle, depth, FALSE);

			/* Empty the input buffer. */
			free(puddle);
			puddle = NULL;
			depth = 0;
		}
	}

	if (shortcut == NULL)
		pletion_line = NULL;
	else {
		if (ISSET(VIEW_MODE) && !okay_for_view(shortcut)) {
			print_view_warning();
			return;
		}

		/* If the function associated with this shortcut is
		 * cutting or copying text, remember this. */
		if (shortcut->func == do_cut_text_void
#ifndef NANO_TINY
				|| shortcut->func == do_copy_text
#endif
				)
			retain_cuts = TRUE;

#ifdef ENABLE_WORDCOMPLETION
		if (shortcut->func != complete_a_word)
			pletion_line = NULL;
#endif
#ifdef ENABLE_NANORC
		if (shortcut->func == (functionptrtype)implant) {
			implant(shortcut->expansion);
			return;
		}
#endif
#ifndef NANO_TINY
		if (shortcut->func == do_toggle_void) {
			do_toggle(shortcut->toggle);
			if (shortcut->toggle != CUT_FROM_CURSOR)
				retain_cuts = TRUE;
		} else
#endif
		{
#ifndef NANO_TINY
			linestruct *was_current = openfile->current;
			size_t was_x = openfile->current_x;

			/* If Shifted movement occurs, set the mark. */
			if (shift_held && !openfile->mark) {
				openfile->mark = openfile->current;
				openfile->mark_x = openfile->current_x;
				openfile->kind_of_mark = SOFTMARK;
			}
#endif
			/* Execute the function of the shortcut. */
			shortcut->func();

#ifndef NANO_TINY
			/* When the marked region changes without Shift being held,
			 * discard a soft mark.  And when the marked region covers a
			 * different set of lines, reset  the "last line too" flag. */
			if (openfile->mark) {
				if (!shift_held && openfile->kind_of_mark == SOFTMARK &&
									(openfile->current != was_current ||
									openfile->current_x != was_x ||
									wanted_to_move(shortcut->func))) {
					openfile->mark = NULL;
					refresh_needed = TRUE;
				} else if (openfile->current != was_current)
					also_the_last = FALSE;
			}
#endif
#ifdef ENABLE_COLOR
			if (!refresh_needed && !okay_for_view(shortcut))
				check_the_multis(openfile->current);
#endif
			if (!refresh_needed && (shortcut->func == do_delete ||
									shortcut->func == do_backspace))
				update_line(openfile->current, openfile->current_x);
		}
	}

	/* If we aren't cutting or copying text, and the key wasn't a toggle,
	 * blow away the text in the cutbuffer upon the next cutting action. */
	if (!retain_cuts)
		keep_cutbuffer = FALSE;
}

/* The user typed output_len multibyte characters.  Add them to the edit
 * buffer, filtering out all ASCII control characters if allow_cntrls is
 * TRUE. */
void do_output(char *output, size_t output_len, bool allow_cntrls)
{
	char onechar[MAXCHARLEN];
	int char_len;
	size_t current_len = strlen(openfile->current->data);
	size_t i = 0;
#ifndef NANO_TINY
	size_t original_row = 0, old_amount = 0;

	if (ISSET(SOFTWRAP)) {
		if (openfile->current_y == editwinrows - 1)
			original_row = chunk_for(xplustabs(), openfile->current);
		old_amount = number_of_chunks_in(openfile->current);
	}
#endif

	while (i < output_len) {
		/* Encode an embedded NUL byte as 0x0A. */
		if (output[i] == '\0')
			output[i] = '\n';

		/* Get the next multibyte character. */
		char_len = parse_mbchar(output + i, onechar, NULL);

		i += char_len;

		/* If controls are not allowed, ignore an ASCII control character. */
		if (!allow_cntrls && is_ascii_cntrl_char(*(output + i - char_len)))
			continue;

		/* Make room for the new character and copy it into the line. */
		openfile->current->data = charealloc(openfile->current->data,
										current_len + char_len + 1);
		charmove(openfile->current->data + openfile->current_x + char_len,
						openfile->current->data + openfile->current_x,
						current_len - openfile->current_x + 1);
		strncpy(openfile->current->data + openfile->current_x, onechar,
						char_len);
		current_len += char_len;
		openfile->totsize++;
		set_modified();

#ifndef NANO_TINY
		/* Only add a new undo item when the current item is not an ADD or when
		 * the current typing is not contiguous with the previous typing. */
		if (openfile->last_action != ADD || openfile->current_undo == NULL ||
				openfile->current_undo->mark_begin_lineno != openfile->current->lineno ||
				openfile->current_undo->mark_begin_x != openfile->current_x)
			add_undo(ADD);

		/* Note that current_x has not yet been incremented. */
		if (openfile->current == openfile->mark &&
						openfile->current_x < openfile->mark_x)
			openfile->mark_x += char_len;

		/* When the cursor is on the top row and not on the first chunk
		 * of a line, adding text there might change the preceding chunk
		 * and thus require an adjustment of firstcolumn. */
		if (openfile->current == openfile->edittop &&
						openfile->firstcolumn > 0) {
			ensure_firstcolumn_is_aligned();
			refresh_needed = TRUE;
		}
#endif

		openfile->current_x += char_len;

#ifndef NANO_TINY
		update_undo(ADD);
#endif

		/* If we've added text to the magic line, create a new magic line. */
		if (openfile->filebot == openfile->current && !ISSET(NO_NEWLINES)) {
			new_magicline();
			if (margin > 0)
				refresh_needed = TRUE;
		}

#ifdef ENABLE_WRAPPING
		/* If text gets wrapped, the edit window needs a refresh. */
		if (ISSET(BREAK_LONG_LINES) && do_wrap())
			refresh_needed = TRUE;
#endif
	}

#ifndef NANO_TINY
	/* If the number of screen rows that a softwrapped line occupies has
	 * changed, we need a full refresh.  And if we were on the last line
	 * of the edit window, and we moved one screen row, we're now below
	 * the last line of the edit window, so we need a full refresh too. */
	if (ISSET(SOFTWRAP) && refresh_needed == FALSE &&
				(number_of_chunks_in(openfile->current) != old_amount ||
				(openfile->current_y == editwinrows - 1 &&
				chunk_for(xplustabs(), openfile->current) != original_row)))
		refresh_needed = TRUE;
#endif

	openfile->placewewant = xplustabs();

#ifdef ENABLE_COLOR
	if (!refresh_needed)
		check_the_multis(openfile->current);
#endif

	if (!refresh_needed)
		update_line(openfile->current, openfile->current_x);
}

int main(int argc, char **argv)
{
	int stdin_flags, optchr;
#ifdef ENABLE_NANORC
	bool ignore_rcfiles = FALSE;
		/* Whether to ignore the nanorc files. */
#endif
#if defined(ENABLED_WRAPORJUSTIFY) && defined(ENABLE_NANORC)
	bool fill_used = FALSE;
		/* Was the fill option used on the command line? */
#endif
	int hardwrap = -2;
		/* Becomes 0 when --nowrap and 1 when --breaklonglines is used. */
#ifdef ENABLE_JUSTIFY
	int quoterc;
		/* Whether the quoting regex was compiled successfully. */
#endif
	const struct option long_options[] = {
		{"boldtext", 0, NULL, 'D'},
#ifdef ENABLE_MULTIBUFFER
		{"multibuffer", 0, NULL, 'F'},
#endif
#ifdef ENABLE_NANORC
		{"ignorercfiles", 0, NULL, 'I'},
#endif
		{"rawsequences", 0, NULL, 'K'},
#ifdef ENABLED_WRAPORJUSTIFY
		{"trimblanks", 0, NULL, 'M'},
#endif
#ifdef ENABLE_JUSTIFY
		{"quotestr", 1, NULL, 'Q'},
#endif
		{"restricted", 0, NULL, 'R'},
		{"tabsize", 1, NULL, 'T'},
		{"quickblank", 0, NULL, 'U'},
		{"version", 0, NULL, 'V'},
#ifdef ENABLE_COLOR
		{"syntax", 1, NULL, 'Y'},
#endif
#ifdef ENABLE_WRAPPING
		{"breaklonglines", 0, NULL, 'b'},
#endif
		{"constantshow", 0, NULL, 'c'},
		{"rebinddelete", 0, NULL, 'd'},
		{"emptyline", 0, NULL, 'e'},
#ifdef ENABLE_BROWSER
		{"showcursor", 0, NULL, 'g'},
#endif
		{"help", 0, NULL, 'h'},
		{"jumpyscrolling", 0, NULL, 'j'},
#ifdef ENABLE_LINENUMBERS
		{"linenumbers", 0, NULL, 'l'},
#endif
#ifdef ENABLE_MOUSE
		{"mouse", 0, NULL, 'm'},
#endif
		{"noread", 0, NULL, 'n'},
#ifdef ENABLE_OPERATINGDIR
		{"operatingdir", 1, NULL, 'o'},
#endif
		{"preserve", 0, NULL, 'p'},
#ifdef ENABLED_WRAPORJUSTIFY
		{"fill", 1, NULL, 'r'},
#endif
#ifdef ENABLE_SPELLER
		{"speller", 1, NULL, 's'},
#endif
		{"tempfile", 0, NULL, 't'},
		{"view", 0, NULL, 'v'},
#ifdef ENABLE_WRAPPING
		{"nowrap", 0, NULL, 'w'},
#endif
		{"nohelp", 0, NULL, 'x'},
		{"suspend", 0, NULL, 'z'},
#ifndef NANO_TINY
		{"smarthome", 0, NULL, 'A'},
		{"backup", 0, NULL, 'B'},
		{"backupdir", 1, NULL, 'C'},
		{"tabstospaces", 0, NULL, 'E'},
		{"locking", 0, NULL, 'G'},
		{"historylog", 0, NULL, 'H'},
		{"guidestripe", 1, NULL, 'J'},
		{"nonewlines", 0, NULL, 'L'},
		{"noconvert", 0, NULL, 'N'},
		{"morespace", 0, NULL, 'O'},
		{"positionlog", 0, NULL, 'P'},
		{"smooth", 0, NULL, 'S'},
		{"wordbounds", 0, NULL, 'W'},
		{"wordchars", 1, NULL, 'X'},
		{"zap", 0, NULL, 'Z'},
		{"atblanks", 0, NULL, 'a'},
		{"autoindent", 0, NULL, 'i'},
		{"cutfromcursor", 0, NULL, 'k'},
		{"unix", 0, NULL, 'u'},
		{"afterends", 0, NULL, 'y'},
		{"softwrap", 0, NULL, '$'},
#endif
		{NULL, 0, NULL, 0}
	};

#ifdef __linux__
	struct vt_stat dummy;

	/* Check whether we're running on a Linux console. */
	on_a_vt = (ioctl(0, VT_GETSTATE, &dummy) == 0);
#endif

	/* Back up the terminal settings so that they can be restored. */
	tcgetattr(0, &oldterm);

	/* Get the state of standard input and ensure it uses blocking mode. */
	stdin_flags = fcntl(0, F_GETFL, 0);
	if (stdin_flags != -1)
		fcntl(0, F_SETFL, stdin_flags & ~O_NONBLOCK);

#ifdef ENABLE_UTF8
	/* If setting the locale is successful and it uses UTF-8, we need
	 * to use the multibyte functions for text processing. */
	if (setlocale(LC_ALL, "") != NULL &&
				strcmp(nl_langinfo(CODESET), "UTF-8") == 0) {
#ifdef USE_SLANG
		SLutf8_enable(1);
#endif
		utf8_init();
	}
#else
	setlocale(LC_ALL, "");
#endif

#ifdef ENABLE_NLS
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif

#if defined(ENABLE_UTF8) && !defined(NANO_TINY)
	if (MB_CUR_MAX > MAXCHARLEN)
		fprintf(stderr, "Unexpected large character size: %i bytes"
						" -- please report a bug\n", (int)MB_CUR_MAX);
#endif

	/* Set sensible defaults, different from what Pico does. */
	SET(NO_WRAP);
	SET(SMOOTH_SCROLL);

	/* Give a small visual hint that nano has changed. */
	SET(MORE_SPACE);

	/* If the executable's name starts with 'r', activate restricted mode. */
	if (*(tail(argv[0])) == 'r')
		SET(RESTRICTED);

	while ((optchr =
		getopt_long(argc, argv,
				"ABC:DEFGHIJ:KLMNOPQ:RST:UVWX:Y:Zabcdeghijklmno:pr:s:tuvwxyz$",
				long_options, NULL)) != -1) {
		switch (optchr) {
#ifndef NANO_TINY
			case 'A':
				SET(SMART_HOME);
				break;
			case 'B':
				SET(BACKUP_FILE);
				break;
			case 'C':
				backup_dir = mallocstrcpy(backup_dir, optarg);
				break;
#endif
			case 'D':
				SET(BOLD_TEXT);
				break;
#ifndef NANO_TINY
			case 'E':
				SET(TABS_TO_SPACES);
				break;
#endif
#ifdef ENABLE_MULTIBUFFER
			case 'F':
				SET(MULTIBUFFER);
				break;
#endif
#ifndef NANO_TINY
			case 'G':
				SET(LOCKING);
				break;
#endif
#ifdef ENABLE_HISTORIES
			case 'H':
				SET(HISTORYLOG);
				break;
#endif
#ifdef ENABLE_NANORC
			case 'I':
				ignore_rcfiles = TRUE;
				break;
#endif
#ifndef NANO_TINY
			case 'J':
				if (!parse_num(optarg, &stripe_column) || stripe_column <= 0) {
					fprintf(stderr, _("Guide column \"%s\" is invalid"), optarg);
					fprintf(stderr, "\n");
					exit(1);
				}
				break;
#endif
			case 'K':
				SET(RAW_SEQUENCES);
				break;
#ifndef NANO_TINY
			case 'L':
				SET(NO_NEWLINES);
				break;
#endif
#ifdef ENABLED_WRAPORJUSTIFY
			case 'M':
				SET(TRIM_BLANKS);
				break;
#endif
#ifndef NANO_TINY
			case 'N':
				SET(NO_CONVERT);
				break;
			case 'O':
				fprintf(stderr, N_("Option %s is ignored; it is the default\n"),
										"morespace");
				break;
#endif
#ifdef ENABLE_HISTORIES
			case 'P':
				SET(POSITIONLOG);
				break;
#endif
#ifdef ENABLE_JUSTIFY
			case 'Q':
				quotestr = mallocstrcpy(quotestr, optarg);
				break;
#endif
			case 'R':
				SET(RESTRICTED);
				break;
#ifndef NANO_TINY
			case 'S':
				fprintf(stderr, N_("Option %s is ignored; it is the default\n"),
										"smooth");
				break;
#endif
			case 'T':
				if (!parse_num(optarg, &tabsize) || tabsize <= 0) {
					fprintf(stderr, _("Requested tab size \"%s\" is invalid"), optarg);
					fprintf(stderr, "\n");
					exit(1);
				}
				break;
			case 'U':
				SET(QUICK_BLANK);
				break;
			case 'V':
				version();
				exit(0);
#ifndef NANO_TINY
			case 'W':
				SET(WORD_BOUNDS);
				break;
			case 'X':
				word_chars = mallocstrcpy(word_chars, optarg);
				break;
#endif
#ifdef ENABLE_COLOR
			case 'Y':
				syntaxstr = mallocstrcpy(syntaxstr, optarg);
				break;
#endif
#ifndef NANO_TINY
			case 'Z':
				SET(LET_THEM_ZAP);
				break;
			case 'a':
				SET(AT_BLANKS);
				break;
#endif
#ifdef ENABLE_WRAPPING
			case 'b':
				hardwrap = 1;
				break;
#endif
			case 'c':
				SET(CONSTANT_SHOW);
				break;
			case 'd':
				SET(REBIND_DELETE);
				break;
			case 'e':
				SET(EMPTY_LINE);
				break;
			case 'g':
				SET(SHOW_CURSOR);
				break;
			case 'h':
				usage();
				exit(0);
#ifndef NANO_TINY
			case 'i':
				SET(AUTOINDENT);
				break;
#endif
			case 'j':
				SET(JUMPY_SCROLLING);
				break;
#ifndef NANO_TINY
			case 'k':
				SET(CUT_FROM_CURSOR);
				break;
#endif
#ifdef ENABLE_LINENUMBERS
			case 'l':
				SET(LINE_NUMBERS);
				break;
#endif
#ifdef ENABLE_MOUSE
			case 'm':
				SET(USE_MOUSE);
				break;
#endif
			case 'n':
				SET(NOREAD_MODE);
				break;
#ifdef ENABLE_OPERATINGDIR
			case 'o':
				operating_dir = mallocstrcpy(operating_dir, optarg);
				break;
#endif
			case 'p':
				SET(PRESERVE);
				break;
#ifdef ENABLED_WRAPORJUSTIFY
			case 'r':
				if (!parse_num(optarg, &fill)) {
					fprintf(stderr, _("Requested fill size \"%s\" is invalid"), optarg);
					fprintf(stderr, "\n");
					exit(1);
				}
#ifdef ENABLE_NANORC
				fill_used = TRUE;
#endif
				break;
#endif
#ifdef ENABLE_SPELLER
			case 's':
				alt_speller = mallocstrcpy(alt_speller, optarg);
				break;
#endif
			case 't':
				SET(TEMP_FILE);
				break;
#ifndef NANO_TINY
			case 'u':
				SET(MAKE_IT_UNIX);
				break;
#endif
			case 'v':
				SET(VIEW_MODE);
				break;
#ifdef ENABLE_WRAPPING
			case 'w':
				hardwrap = 0;
				break;
#endif
			case 'x':
				SET(NO_HELP);
				break;
#ifndef NANO_TINY
			case 'y':
				SET(AFTER_ENDS);
				break;
#endif
			case 'z':
				SET(SUSPEND);
				break;
#ifndef NANO_TINY
			case '$':
				SET(SOFTWRAP);
				break;
#endif
			default:
				printf(_("Type '%s -h' for a list of available options.\n"), argv[0]);
				exit(1);
		}
	}

	/* Set up the function and shortcut lists.  This needs to be done
	 * before reading the rcfile, to be able to rebind/unbind keys. */
	shortcut_init();

#ifdef ENABLE_NANORC
	if (!ignore_rcfiles) {
		/* Back up the command-line options that take an argument. */
#ifdef ENABLED_WRAPORJUSTIFY
		ssize_t fill_cmdline = fill;
#endif
#ifndef NANO_TINY
		size_t stripeclm_cmdline = stripe_column;
		char *backup_dir_cmdline = backup_dir;
		char *word_chars_cmdline = word_chars;
#endif
#ifdef ENABLE_OPERATINGDIR
		char *operating_dir_cmdline = operating_dir;
#endif
#ifdef ENABLE_JUSTIFY
		char *quotestr_cmdline = quotestr;
#endif
#ifdef ENABLE_SPELLER
		char *alt_speller_cmdline = alt_speller;
#endif
		ssize_t tabsize_cmdline = tabsize;

		/* Back up the command-line flags. */
		unsigned flags_cmdline[sizeof(flags) / sizeof(flags[0])];
		memcpy(flags_cmdline, flags, sizeof(flags_cmdline));

		/* Clear the string options, to not overwrite the specified ones. */
#ifndef NANO_TINY
		backup_dir = NULL;
		word_chars = NULL;
#endif
#ifdef ENABLE_OPERATINGDIR
		operating_dir = NULL;
#endif
#ifdef ENABLE_JUSTIFY
		quotestr = NULL;
#endif
#ifdef ENABLE_SPELLER
		alt_speller = NULL;
#endif
		/* Now process the system's and the user's nanorc file, if any. */
		do_rcfiles();

#ifdef DEBUG
		fprintf(stderr, "After rebinding keys...\n");
		print_sclist();
#endif

		/* If the backed-up command-line options have a value, restore them. */
#ifdef ENABLED_WRAPORJUSTIFY
		if (fill_used)
			fill = fill_cmdline;
#endif
#ifndef NANO_TINY
		if (stripeclm_cmdline > 0)
			stripe_column = stripeclm_cmdline;
		if (backup_dir_cmdline != NULL) {
			free(backup_dir);
			backup_dir = backup_dir_cmdline;
		}
		if (word_chars_cmdline != NULL) {
			free(word_chars);
			word_chars = word_chars_cmdline;
		}
#endif
#ifdef ENABLE_OPERATINGDIR
		if (operating_dir_cmdline != NULL || ISSET(RESTRICTED)) {
			free(operating_dir);
			operating_dir = operating_dir_cmdline;
		}
#endif
#ifdef ENABLE_JUSTIFY
		if (quotestr_cmdline != NULL) {
			free(quotestr);
			quotestr = quotestr_cmdline;
		}
#endif
#ifdef ENABLE_SPELLER
		if (alt_speller_cmdline != NULL) {
			free(alt_speller);
			alt_speller = alt_speller_cmdline;
		}
#endif
		if (tabsize_cmdline != -1)
			tabsize = tabsize_cmdline;

		/* If an rcfile undid the default settings, copy it to the new flags. */
		if (!ISSET(NO_WRAP))
			SET(BREAK_LONG_LINES);
		if (!ISSET(SMOOTH_SCROLL))
			SET(JUMPY_SCROLLING);
		if (!ISSET(MORE_SPACE))
			SET(EMPTY_LINE);

		/* Simply OR the boolean flags from rcfile and command line. */
		for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); i++)
			flags[i] |= flags_cmdline[i];
	}
#endif /* ENABLE_NANORC */

	if (hardwrap == 0)
		UNSET(BREAK_LONG_LINES);
	else if (hardwrap == 1)
		SET(BREAK_LONG_LINES);

	/* If the user wants bold instead of reverse video for hilited text... */
	if (ISSET(BOLD_TEXT))
		hilite_attribute = A_BOLD;

	/* When in restricted mode, disable backups, suspending, and history files,
	 * since they allow writing to files not specified on the command line. */
	if (ISSET(RESTRICTED)) {
		UNSET(BACKUP_FILE);
		UNSET(SUSPEND);
#ifdef ENABLE_NANORC
		UNSET(HISTORYLOG);
		UNSET(POSITIONLOG);
#endif
	}

	/* When getting untranslated escape sequences, the mouse cannot be used. */
	if (ISSET(RAW_SEQUENCES))
		UNSET(USE_MOUSE);

#ifdef ENABLE_HISTORIES
	/* Initialize the pointers for the Search/Replace/Execute histories. */
	history_init();

	/* If we need history files, verify that we have a directory for them,
	 * and when not, cancel the options. */
	if ((ISSET(HISTORYLOG) || ISSET(POSITIONLOG)) && !have_statedir()) {
		UNSET(HISTORYLOG);
		UNSET(POSITIONLOG);
	}

	/* If the user wants history persistence, read the relevant files. */
	if (ISSET(HISTORYLOG))
		load_history();
	if (ISSET(POSITIONLOG))
		load_poshistory();
#endif /* ENABLE_HISTORIES */

#ifndef NANO_TINY
	/* If a backup directory was specified and we're not in restricted mode,
	 * verify it is an existing folder, so backup files can be saved there. */
	if (backup_dir != NULL && !ISSET(RESTRICTED))
		init_backup_dir();
#endif
#ifdef ENABLE_OPERATINGDIR
	/* Set up the operating directory.  This entails chdir()ing there,
	 * so that file reads and writes will be based there. */
	if (operating_dir != NULL)
		init_operating_dir();
#endif

#ifdef ENABLE_JUSTIFY
	/* Set the default value for things that weren't specified. */
	if (punct == NULL)
		punct = mallocstrcpy(NULL, "!.?");
	if (brackets == NULL)
		brackets = mallocstrcpy(NULL, "\"')>]}");
	if (quotestr == NULL)
		quotestr = mallocstrcpy(NULL, "^([ \t]*([!#%:;>|}]|/{2}|--))+");

	/* Compile the quoting regex, and exit when it's invalid. */
	quoterc = regcomp(&quotereg, quotestr, NANO_REG_EXTENDED);
	if (quoterc != 0) {
		size_t size = regerror(quoterc, &quotereg, NULL, 0);
		char *message = charalloc(size);

		regerror(quoterc, &quotereg, message, size);
		die(_("Bad quoting regex \"%s\": %s\n"), quotestr, message);
	} else
		free(quotestr);
#endif /* ENABLE_JUSTIFY */

#ifdef ENABLE_SPELLER
	/* If we don't have an alternative spell checker after reading the
	 * command line and/or rcfile(s), check $SPELL for one, as Pico
	 * does (unless we're using restricted mode, in which case spell
	 * checking is disabled, since it would allow reading from or
	 * writing to files not specified on the command line). */
	if (alt_speller == NULL && !ISSET(RESTRICTED)) {
		const char *spellenv = getenv("SPELL");

		if (spellenv != NULL)
			alt_speller = mallocstrcpy(NULL, spellenv);
	}
#endif

#ifndef NANO_TINY
	/* If matchbrackets wasn't specified, set its default value. */
	if (matchbrackets == NULL)
		matchbrackets = mallocstrcpy(NULL, "(<[{)>]}");

	/* If the whitespace option wasn't specified, set its default value. */
	if (whitespace == NULL) {
#ifdef ENABLE_UTF8
		if (using_utf8()) {
			/* A tab is shown as a Right-Pointing Double Angle Quotation Mark
			 * (U+00BB), and a space as a Middle Dot (U+00B7). */
			whitespace = mallocstrcpy(NULL, "\xC2\xBB\xC2\xB7");
			whitelen[0] = 2;
			whitelen[1] = 2;
		} else
#endif
		{
			whitespace = mallocstrcpy(NULL, ">.");
			whitelen[0] = 1;
			whitelen[1] = 1;
		}
	}
#endif /* !NANO_TINY */

	/* Initialize the search string. */
	last_search = mallocstrcpy(NULL, "");
	UNSET(BACKWARDS_SEARCH);

	/* If tabsize wasn't specified, set its default value. */
	if (tabsize == -1)
		tabsize = WIDTH_OF_TAB;

	/* Initialize curses mode.  If this fails, get out. */
	if (initscr() == NULL)
		exit(1);

#ifdef ENABLE_COLOR
	set_colorpairs();
#else
	interface_color_pair[TITLE_BAR] = hilite_attribute;
	interface_color_pair[LINE_NUMBER] = hilite_attribute;
	interface_color_pair[GUIDE_STRIPE] = A_REVERSE;
	interface_color_pair[SELECTED_TEXT] = hilite_attribute;
	interface_color_pair[STATUS_BAR] = hilite_attribute;
	interface_color_pair[ERROR_MESSAGE] = hilite_attribute;
	interface_color_pair[KEY_COMBO] = hilite_attribute;
	interface_color_pair[FUNCTION_TAG] = A_NORMAL;
#endif

	/* Set up the terminal state. */
	terminal_init();

#ifdef DEBUG
	fprintf(stderr, "Main: set up windows\n");
#endif

	/* Create the three subwindows, based on the current screen dimensions. */
	window_init();
	curs_set(0);

	editwincols = COLS;

	/* Set up the signal handlers. */
	signal_init();

#ifdef ENABLE_MOUSE
	/* Initialize mouse support. */
	mouse_init();
#endif

	/* Ask ncurses for the key codes for most modified editing keys. */
	controlleft = get_keycode("kLFT5", CONTROL_LEFT);
	controlright = get_keycode("kRIT5", CONTROL_RIGHT);
	controlup = get_keycode("kUP5", CONTROL_UP);
	controldown = get_keycode("kDN5", CONTROL_DOWN);

	controlhome = get_keycode("kHOM5", CONTROL_HOME);
	controlend = get_keycode("kEND5", CONTROL_END);
#ifndef NANO_TINY
	controldelete = get_keycode("kDC5", CONTROL_DELETE);
	controlshiftdelete = get_keycode("kDC6", CONTROL_SHIFT_DELETE);

	shiftup = get_keycode("kUP", SHIFT_UP);
	shiftdown = get_keycode("kDN", SHIFT_DOWN);

	shiftcontrolleft = get_keycode("kLFT6", SHIFT_CONTROL_LEFT);
	shiftcontrolright = get_keycode("kRIT6", SHIFT_CONTROL_RIGHT);
	shiftcontrolup = get_keycode("kUP6", SHIFT_CONTROL_UP);
	shiftcontroldown = get_keycode("kDN6", SHIFT_CONTROL_DOWN);

	shiftcontrolhome = get_keycode("kHOM6", SHIFT_CONTROL_HOME);
	shiftcontrolend = get_keycode("kEND6", SHIFT_CONTROL_END);

	altleft = get_keycode("kLFT3", ALT_LEFT);
	altright = get_keycode("kRIT3", ALT_RIGHT);
	altup = get_keycode("kUP3", ALT_UP);
	altdown = get_keycode("kDN3", ALT_DOWN);
	altdelete = get_keycode("kDC3", ALT_DELETE);

	shiftaltleft = get_keycode("kLFT4", SHIFT_ALT_LEFT);
	shiftaltright = get_keycode("kRIT4", SHIFT_ALT_RIGHT);
	shiftaltup = get_keycode("kUP4", SHIFT_ALT_UP);
	shiftaltdown = get_keycode("kDN4", SHIFT_ALT_DOWN);
#endif

#ifdef HAVE_SET_ESCDELAY
	/* Tell ncurses to pass the Esc key quickly. */
	set_escdelay(50);
#endif

#ifdef DEBUG
	fprintf(stderr, "Main: open file\n");
#endif

	/* Read the files mentioned on the command line into new buffers. */
	while (optind < argc && (!openfile || read_them_all)) {
		ssize_t givenline = 0, givencol = 0;

		/* If there's a +LINE[,COLUMN] argument here, eat it up. */
		if (optind < argc - 1 && argv[optind][0] == '+') {
			if (!parse_line_column(&argv[optind++][1], &givenline, &givencol))
				statusline(ALERT, _("Invalid line or column number"));
		}

		/* If the filename is a dash, read from standard input; otherwise,
		 * open the file; skip positioning the cursor if either failed. */
		if (strcmp(argv[optind], "-") == 0) {
			optind++;
			if (!scoop_stdin())
				continue;
		} else if (!open_buffer(argv[optind++], TRUE))
			continue;

		/* If a position was given on the command line, go there. */
		if (givenline != 0 || givencol != 0)
			do_gotolinecolumn(givenline, givencol, FALSE, FALSE);
#ifdef ENABLE_HISTORIES
		else if (ISSET(POSITIONLOG) && openfile->filename[0] != '\0') {
			ssize_t savedline, savedcol;
			/* If edited before, restore the last cursor position. */
			if (has_old_position(argv[optind - 1], &savedline, &savedcol))
				do_gotolinecolumn(savedline, savedcol, FALSE, FALSE);
		}
#endif
	}

	/* If no filenames were given, or all of them were invalid things like
	 * directories, then open a blank buffer and allow editing.  Otherwise,
	 * switch from the last opened file to the next, that is: the first. */
	if (openfile == NULL) {
		open_buffer("", TRUE);
		UNSET(VIEW_MODE);
	}
#ifdef ENABLE_MULTIBUFFER
	else {
		openfile = openfile->next;
		if (more_than_one)
			mention_name_and_linecount();
		if (ISSET(VIEW_MODE))
			SET(MULTIBUFFER);
	}
#endif

#ifdef __linux__
	/* Check again whether we're running on a Linux console. */
	on_a_vt = (ioctl(0, VT_GETSTATE, &dummy) == 0);
#endif

#ifdef DEBUG
	fprintf(stderr, "Main: show title bar, and enter main loop\n");
#endif

	prepare_for_display();

#ifdef ENABLE_NANORC
	if (rcfile_with_errors != NULL)
		statusline(ALERT, _("Mistakes in '%s'"), rcfile_with_errors);
#endif

#ifdef ENABLE_HELP
	if (*openfile->filename == '\0' && openfile->totsize == 0 &&
				openfile->next == openfile && !ISSET(NO_HELP))
		statusbar(_("Welcome to nano.  For basic help, type Ctrl+G."));
#endif

	while (TRUE) {
#ifdef ENABLE_LINENUMBERS
		confirm_margin();
#endif
		if (currmenu != MMAIN)
			bottombars(MMAIN);

		lastmessage = HUSH;
		as_an_at = TRUE;

		/* Update the displayed current cursor position only when there
		 * are no keys waiting in the input buffer. */
		if (ISSET(CONSTANT_SHOW) && get_key_buffer_len() == 0)
			do_cursorpos(FALSE);

		/* Refresh just the cursor position or the entire edit window. */
		if (!refresh_needed) {
			place_the_cursor();
			wnoutrefresh(edit);
		} else
			edit_refresh();

		errno = 0;
		focusing = TRUE;

		/* Forget any earlier cursor position at the prompt. */
		put_cursor_at_end_of_answer();

		/* Read in and interpret a single keystroke. */
		do_input();
	}
}
