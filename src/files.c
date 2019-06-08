/**************************************************************************
 *   files.c  --  This file is part of GNU nano.                          *
 *                                                                        *
 *   Copyright (C) 1999-2011, 2013-2019 Free Software Foundation, Inc.    *
 *   Copyright (C) 2015-2019 Benno Schulenberg                            *
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

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#include <string.h>
#include <unistd.h>

#define LOCKBUFSIZE 8192

/* Verify that the containing directory of the given filename exists. */
bool has_valid_path(const char *filename)
{
	char *namecopy = mallocstrcpy(NULL, filename);
	char *parentdir = dirname(namecopy);
	struct stat parentinfo;
	bool validity = FALSE;

	if (stat(parentdir, &parentinfo) == -1) {
		if (errno == ENOENT)
			statusline(ALERT, _("Directory '%s' does not exist"), parentdir);
		else
			statusline(ALERT, _("Path '%s': %s"), parentdir, strerror(errno));
	} else if (!S_ISDIR(parentinfo.st_mode))
		statusline(ALERT, _("Path '%s' is not a directory"), parentdir);
	else if (access(parentdir, X_OK) == -1)
		statusline(ALERT, _("Path '%s' is not accessible"), parentdir);
	else if (ISSET(LOCKING) && access(parentdir, W_OK) == -1)
		statusline(MILD, _("Directory '%s' is not writable"), parentdir);
	else
		validity = TRUE;

	free(namecopy);

	return validity;
}

/* Add an item to the circular list of openfile structs. */
void make_new_buffer(void)
{
	openfilestruct *newnode = nmalloc(sizeof(openfilestruct));

#ifdef ENABLE_MULTIBUFFER
	if (openfile == NULL) {
		/* Make the first open file the only element in the list. */
		newnode->prev = newnode;
		newnode->next = newnode;

		startfile = newnode;
	} else {
		/* Add the new open file after the current one in the list. */
		newnode->prev = openfile;
		newnode->next = openfile->next;
		openfile->next->prev = newnode;
		openfile->next = newnode;

		/* There is more than one file open: show "Close" in help lines. */
		exitfunc->desc = close_tag;
		more_than_one = !inhelp || more_than_one;
	}
#endif
	/* Make the new buffer the current one, and start initializing it. */
	openfile = newnode;

	openfile->filename = mallocstrcpy(NULL, "");

	initialize_buffer_text();

	openfile->placewewant = 0;
	openfile->current_y = 0;

	openfile->modified = FALSE;
#ifdef ENABLE_WRAPPING
	openfile->spillage_line = NULL;
#endif
#ifndef NANO_TINY
	openfile->mark = NULL;

	openfile->fmt = NIX_FILE;

	openfile->undotop = NULL;
	openfile->current_undo = NULL;
	openfile->last_saved = NULL;
	openfile->last_action = OTHER;

	openfile->current_stat = NULL;
	openfile->lock_filename = NULL;
#endif
#ifdef ENABLE_COLOR
	openfile->syntax = NULL;
	openfile->colorstrings = NULL;
#endif
}

/* Initialize the text and pointers of the current openfile struct. */
void initialize_buffer_text(void)
{
	openfile->filetop = make_new_node(NULL);
	openfile->filetop->data = mallocstrcpy(NULL, "");

	openfile->filebot = openfile->filetop;
	openfile->edittop = openfile->filetop;
	openfile->current = openfile->filetop;

	openfile->firstcolumn = 0;
	openfile->current_x = 0;
	openfile->totsize = 0;
}

/* Mark the current file as modified if it isn't already, and then
 * update the titlebar to display the file's new status. */
void set_modified(void)
{
	if (openfile->modified)
		return;

	openfile->modified = TRUE;
	titlebar(NULL);

#ifndef NANO_TINY
	if (!ISSET(LOCKING) || openfile->filename[0] == '\0')
		return;

	if (openfile->lock_filename != NULL) {
		char *fullname = get_full_path(openfile->filename);

		write_lockfile(openfile->lock_filename, fullname, TRUE);
		free(fullname);
	}
#endif
}

#ifndef NANO_TINY
/* Actually write the lockfile.  This function will ALWAYS annihilate
 * any previous version of the file.  We'll borrow INSECURE_BACKUP here
 * to decide about lockfile paranoia here as well...
 *
 * Args:
 *     lockfilename: file name for lock
 *     origfilename: name of the file the lock is for
 *     modified: whether to set the modified bit in the file
 *
 * Returns 1 on success, and 0 on failure (but continue anyway). */
int write_lockfile(const char *lockfilename, const char *origfilename, bool modified)
{
#ifdef HAVE_PWD_H
	int cflags, fd;
	FILE *filestream;
	pid_t mypid;
	uid_t myuid;
	struct passwd *mypwuid;
	struct stat fileinfo;
	char *lockdata = charalloc(1024);
	char myhostname[32];
	size_t lockdatalen = 1024;
	size_t wroteamt;

	mypid = getpid();
	myuid = geteuid();

	/* First run things that might fail before blowing away the old state. */
	if ((mypwuid = getpwuid(myuid)) == NULL) {
		/* TRANSLATORS: Keep the next eight messages at most 76 characters. */
		statusline(MILD, _("Couldn't determine my identity for lock file "
								"(getpwuid() failed)"));
		goto free_the_data;
	}

	if (gethostname(myhostname, 31) < 0) {
		if (errno == ENAMETOOLONG)
			myhostname[31] = '\0';
		else {
			statusline(MILD, _("Couldn't determine hostname for lock file: %s"),
						strerror(errno));
			goto free_the_data;
		}
	}

	/* If the lockfile exists, try to delete it. */
	if (stat(lockfilename, &fileinfo) != -1)
		if (delete_lockfile(lockfilename) < 0)
			goto free_the_data;

	if (ISSET(INSECURE_BACKUP))
		cflags = O_WRONLY | O_CREAT | O_APPEND;
	else
		cflags = O_WRONLY | O_CREAT | O_EXCL | O_APPEND;

	/* Try to create the lockfile. */
	fd = open(lockfilename, cflags,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fd < 0) {
		statusline(MILD, _("Error writing lock file %s: %s"),
						lockfilename, strerror(errno));
		goto free_the_data;
	}

	/* Try to associate a stream with the now open lockfile. */
	filestream = fdopen(fd, "wb");

	if (filestream == NULL) {
		statusline(MILD, _("Error writing lock file %s: %s"), lockfilename,
						strerror(errno));
		goto free_the_data;
	}

	/* This is the lock data we will store:
	 *
	 * byte 0        - 0x62
	 * byte 1        - 0x30
	 * bytes 2-12    - program name which created the lock
	 * bytes 24-27   - PID (little endian) of creator process
	 * bytes 28-44   - username of who created the lock
	 * bytes 68-100  - hostname of where the lock was created
	 * bytes 108-876 - filename the lock is for
	 * byte 1007     - 0x55 if file is modified
	 *
	 * Looks like VIM also stores undo state in this file, so we're
	 * gonna have to figure out how to slap a 'OMG don't use recover on
	 * our lockfile' message in here...
	 *
	 * This is likely very wrong, so this is a WIP. */
	memset(lockdata, 0, lockdatalen);
	lockdata[0] = 0x62;
	lockdata[1] = 0x30;
	lockdata[24] = mypid % 256;
	lockdata[25] = (mypid / 256) % 256;
	lockdata[26] = (mypid / (256 * 256)) % 256;
	lockdata[27] = mypid / (256 * 256 * 256);
	snprintf(&lockdata[2], 11, "nano %s", VERSION);
	strncpy(&lockdata[28], mypwuid->pw_name, 16);
	strncpy(&lockdata[68], myhostname, 31);
	if (origfilename != NULL)
		strncpy(&lockdata[108], origfilename, 768);
	if (modified == TRUE)
		lockdata[1007] = 0x55;

	wroteamt = fwrite(lockdata, sizeof(char), lockdatalen, filestream);
	if (wroteamt < lockdatalen) {
		statusline(MILD, _("Error writing lock file %s: %s"),
						lockfilename, ferror(filestream));
		fclose(filestream);
		goto free_the_data;
	}

	if (fclose(filestream) == EOF) {
		statusline(MILD, _("Error writing lock file %s: %s"),
						lockfilename, strerror(errno));
		goto free_the_data;
	}

	openfile->lock_filename = (char *) lockfilename;

	free(lockdata);
	return 1;

  free_the_data:
	free(lockdata);
	return 0;
#else
	return 1;
#endif
}

/* Delete the lockfile.  Return -1 if unsuccessful, and 1 otherwise. */
int delete_lockfile(const char *lockfilename)
{
	if (unlink(lockfilename) < 0 && errno != ENOENT) {
		statusline(MILD, _("Error deleting lock file %s: %s"), lockfilename,
						strerror(errno));
		return -1;
	}
	return 1;
}

/* Deal with lockfiles.  Return -1 on refusing to override the lockfile,
 * and 1 on successfully creating it; 0 means we were not successful in
 * creating the lockfile but we should continue to load the file. */
int do_lockfile(const char *filename)
{
	char *namecopy = (char *) mallocstrcpy(NULL, filename);
	char *secondcopy = (char *) mallocstrcpy(NULL, filename);
	size_t locknamesize = strlen(filename) + strlen(locking_prefix)
				+ strlen(locking_suffix) + 3;
	char *lockfilename = charalloc(locknamesize);
	static char lockprog[11], lockuser[17];
	struct stat fileinfo;
	int lockfd, lockpid, retval = -1;

	snprintf(lockfilename, locknamesize, "%s/%s%s%s", dirname(namecopy),
				locking_prefix, basename(secondcopy), locking_suffix);

	if (stat(lockfilename, &fileinfo) != -1) {
		size_t readtot = 0;
		size_t readamt = 0;
		char *lockbuf, *question, *pidstring, *postedname, *promptstr;
		int room, choice;

		if ((lockfd = open(lockfilename, O_RDONLY)) < 0) {
			statusline(MILD, _("Error opening lock file %s: %s"),
						lockfilename, strerror(errno));
			goto free_the_name;
		}

		lockbuf = charalloc(LOCKBUFSIZE);
		do {
			readamt = read(lockfd, &lockbuf[readtot], LOCKBUFSIZE - readtot);
			readtot += readamt;
		} while (readamt > 0 && readtot < LOCKBUFSIZE);

		close(lockfd);

		if (readtot < 48) {
			statusline(MILD, _("Error reading lock file %s: "
						"Not enough data read"), lockfilename);
			free(lockbuf);
			goto free_the_name;
		}

		strncpy(lockprog, &lockbuf[2], 10);
		lockpid = (((unsigned char)lockbuf[27] * 256 + (unsigned char)lockbuf[26]) * 256 +
						(unsigned char)lockbuf[25]) * 256 + (unsigned char)lockbuf[24];
		strncpy(lockuser, &lockbuf[28], 16);
		free(lockbuf);

		pidstring = charalloc(11);
		sprintf (pidstring, "%u", (unsigned int)lockpid);

		/* Display newlines in filenames as ^J. */
		as_an_at = FALSE;

		/* TRANSLATORS: The second %s is the name of the user, the third that of the editor. */
		question = _("File %s is being edited (by %s with %s, PID %s); continue?");
		room = COLS - breadth(question) + 7 - breadth(lockuser) -
								breadth(lockprog) - breadth(pidstring);
		if (room < 4)
			postedname = mallocstrcpy(NULL, "_");
		else if (room < breadth(filename)) {
			char *fragment = display_string(filename,
								breadth(filename) - room + 3, room, FALSE, FALSE);
			postedname = charalloc(strlen(fragment) + 4);
			strcpy(postedname, "...");
			strcat(postedname, fragment);
			free(fragment);
		} else
			postedname = display_string(filename, 0, room, FALSE, FALSE);

		/* Allow extra space for username (14), program name (8), PID (8),
		 * and terminating \0 (1), minus the %s (2) for the file name. */
		promptstr = charalloc(strlen(question) + 29 + strlen(postedname));
		sprintf(promptstr, question, postedname, lockuser, lockprog, pidstring);
		free(postedname);
		free(pidstring);

		choice = do_yesno_prompt(FALSE, promptstr);
		free(promptstr);

		if (choice < 1) {
			wipe_statusbar();
			goto free_the_name;
		}
	}

	retval = write_lockfile(lockfilename, filename, FALSE);

  free_the_name:
	free(namecopy);
	free(secondcopy);
	if (retval < 1)
		free(lockfilename);

	return retval;
}

/* Perform a stat call on the given filename, allocating a stat struct
 * if necessary.  On success, *pstat points to the stat's result.  On
 * failure, *pstat is freed and made NULL. */
void stat_with_alloc(const char *filename, struct stat **pstat)
{
	if (*pstat == NULL)
		*pstat = (struct stat *)nmalloc(sizeof(struct stat));

	if (stat(filename, *pstat) != 0) {
		free(*pstat);
		*pstat = NULL;
	}
}
#endif /* !NANO_TINY */

/* This does one of three things.  If the filename is "", it just creates
 * a new empty buffer.  When the filename is not empty, it reads that file
 * into a new buffer when requested, otherwise into the existing buffer. */
bool open_buffer(const char *filename, bool new_buffer)
{
	char *realname;
		/* The filename after tilde expansion. */
	FILE *f;
	int rc;
		/* rc == -2 means that we have a new file.  -1 means that the
		 * open() failed.  0 means that the open() succeeded. */

	/* Display newlines in filenames as ^J. */
	as_an_at = FALSE;

#ifdef ENABLE_OPERATINGDIR
	if (outside_of_confinement(filename, FALSE)) {
		statusline(ALERT, _("Can't read file from outside of %s"),
								operating_dir);
		return FALSE;
	}
#endif

	realname = real_dir_from_tilde(filename);

	/* When the specified filename is not empty, and the corresponding
	 * file exists, verify that it is a normal file. */
	if (*filename != '\0') {
		struct stat fileinfo;

		if (stat(realname, &fileinfo) == 0 && !(S_ISREG(fileinfo.st_mode) ||
					(ISSET(NOREAD_MODE) && S_ISFIFO(fileinfo.st_mode)))) {
			if (S_ISDIR(fileinfo.st_mode))
				statusline(ALERT, _("\"%s\" is a directory"), realname);
			else
				statusline(ALERT, _("\"%s\" is not a normal file"), realname);
			free(realname);
			return FALSE;
		}
	}

	/* If we're going to load into a new buffer, first create the new
	 * buffer and (if possible) lock the corresponding file. */
	if (new_buffer) {
		make_new_buffer();

		if (!inhelp && has_valid_path(realname)) {
#ifndef NANO_TINY
			if (ISSET(LOCKING) && filename[0] != '\0') {
				/* When not overriding an existing lock, discard the buffer. */
				if (do_lockfile(realname) < 0) {
#ifdef ENABLE_MULTIBUFFER
					close_buffer();
#endif
					free(realname);
					return FALSE;
				}
			}
#endif /* !NANO_TINY */
		}
	}

	/* If the filename isn't blank, and we are not in NOREAD_MODE,
	 * open the file.  Otherwise, treat it as a new file. */
	rc = (filename[0] != '\0' && !ISSET(NOREAD_MODE)) ?
				open_file(realname, new_buffer, inhelp, &f) : -2;

	/* If we have a non-new file, read it in.  Then, if the buffer has
	 * no stat, update the stat, if applicable. */
	if (rc > 0) {
		read_file(f, rc, realname, !new_buffer);
#ifndef NANO_TINY
		if (openfile->current_stat == NULL)
			stat_with_alloc(realname, &openfile->current_stat);
#endif
	}

	/* If we have a file, and we've loaded it into a new buffer, set
	 * the filename and put the cursor at the start of the buffer. */
	if (rc != -1 && new_buffer) {
		openfile->filename = mallocstrcpy(openfile->filename, realname);
		openfile->current = openfile->filetop;
		openfile->current_x = 0;
		openfile->placewewant = 0;
	}

#ifdef ENABLE_COLOR
	/* If we're loading into a new buffer, update the colors to account
	 * for it, if applicable. */
	if (new_buffer)
		color_update();
#endif
	free(realname);
	return TRUE;
}

#ifdef ENABLE_SPELLER
/* Open the specified file, and if that succeeds, blow away the text of
 * the current buffer and read the file contents into its place. */
void replace_buffer(const char *filename)
{
	FILE *f;
	int descriptor;
	linestruct *was_cutbuffer = cutbuffer;

	/* Open the file quietly. */
	descriptor = open_file(filename, FALSE, TRUE, &f);

	/* If opening failed, forget it. */
	if (descriptor < 0)
		return;

#ifndef NANO_TINY
	add_undo(COUPLE_BEGIN);
	openfile->undotop->strdata = mallocstrcpy(NULL, _("spelling correction"));
#endif

	/* Throw away the text of the file. */
	cutbuffer = NULL;
	openfile->current = openfile->filetop;
	openfile->current_x = 0;
#ifndef NANO_TINY
	add_undo(CUT_TO_EOF);
#endif
	do_cut_text(FALSE, FALSE, TRUE, FALSE);
#ifndef NANO_TINY
	update_undo(CUT_TO_EOF);
#endif
	free_lines(cutbuffer);
	cutbuffer = was_cutbuffer;

	/* Insert the processed file into its place. */
	read_file(f, descriptor, filename, TRUE);

#ifndef NANO_TINY
	add_undo(COUPLE_END);
	openfile->undotop->strdata = mallocstrcpy(NULL, _("spelling correction"));
#endif
}

#ifndef NANO_TINY
/* Open the specified file, and if that succeeds, blow away the text of
 * the current buffer covered by the mark and read the file
 * contents into its place. */
void replace_marked_buffer(const char *filename)
{
	FILE *f;
	int descriptor;
	bool using_magicline = !ISSET(NO_NEWLINES);
	linestruct *was_cutbuffer = cutbuffer;

	descriptor = open_file(filename, FALSE, TRUE, &f);

	if (descriptor < 0)
		return;

	/* Don't add a magic line when replacing text in the buffer. */
	SET(NO_NEWLINES);

	add_undo(COUPLE_BEGIN);
	openfile->undotop->strdata = mallocstrcpy(NULL, _("spelling correction"));

	/* Throw away the text under the mark. */
	cutbuffer = NULL;
	add_undo(CUT);
	do_cut_text(FALSE, TRUE, FALSE, FALSE);
	update_undo(CUT);
	free_lines(cutbuffer);
	cutbuffer = was_cutbuffer;

	/* Insert the processed file where the marked text was. */
	read_file(f, descriptor, filename, TRUE);

	/* Restore the magic-line behavior now that we're done fiddling. */
	if (using_magicline)
		UNSET(NO_NEWLINES);

	add_undo(COUPLE_END);
	openfile->undotop->strdata = mallocstrcpy(NULL, _("spelling correction"));
}
#endif /* !NANO_TINY */
#endif /* ENABLE_SPELLER */

/* Update the titlebar and the multiline cache to match the current buffer. */
void prepare_for_display(void)
{
	/* Update the titlebar, since the filename may have changed. */
	if (!inhelp)
		titlebar(NULL);

#ifdef ENABLE_COLOR
	/* If there are multiline coloring regexes, and there is no
	 * multiline cache data yet, precalculate it now. */
	if (openfile->syntax && openfile->syntax->nmultis > 0 &&
				openfile->filetop->multidata == NULL)
		precalc_multicolorinfo();

	have_palette = FALSE;
#endif
	refresh_needed = TRUE;
}

#ifdef ENABLE_MULTIBUFFER
/* Show name of current buffer and its number of lines on the status bar. */
void mention_name_and_linecount(void)
{
	size_t count = openfile->filebot->lineno -
						(openfile->filebot->data[0] == '\0' ? 1 : 0);
#ifndef NANO_TINY
	if (openfile->fmt != NIX_FILE)
		/* TRANSLATORS: first %s is file name, second %s is file format. */
		statusline(HUSH, P_("%s -- %zu line (%s)", "%s -- %zu lines (%s)", count),
						openfile->filename[0] == '\0' ?
						_("New Buffer") : tail(openfile->filename), count,
						openfile->fmt == DOS_FILE ? _("DOS") : _("Mac"));
	else
#endif
		statusline(HUSH, P_("%s -- %zu line", "%s -- %zu lines", count),
						openfile->filename[0] == '\0' ?
						_("New Buffer") : tail(openfile->filename), count);
}

/* Switch to a neighbouring file buffer; to the next if to_next is TRUE;
 * otherwise, to the previous one. */
void switch_to_adjacent_buffer(bool to_next)
{
	/* If only one file buffer is open, say so and get out. */
	if (openfile == openfile->next) {
		statusbar(_("No more open file buffers"));
		return;
	}

	/* Switch to the next or previous file buffer. */
	openfile = to_next ? openfile->next : openfile->prev;

#ifndef NANO_TINY
	/* When not in softwrap mode, make sure firstcolumn is zero.  It might
	 * be nonzero if we had softwrap mode on while in this buffer, and then
	 * turned softwrap mode off while in a different buffer. */
	if (!ISSET(SOFTWRAP))
		openfile->firstcolumn = 0;
#endif

	/* Update titlebar and multiline info to match the current buffer. */
	prepare_for_display();

	if (inhelp)
		return;

	/* Ensure that the main loop will redraw the help lines. */
	currmenu = MMOST;

	/* Prevent a possible Shift selection from getting cancelled. */
	shift_held = TRUE;

	/* Indicate on the status bar where we switched to. */
	mention_name_and_linecount();
}

/* Switch to the previous entry in the list of open files. */
void switch_to_prev_buffer(void)
{
	switch_to_adjacent_buffer(BACKWARD);
}

/* Switch to the next entry in the list of open files. */
void switch_to_next_buffer(void)
{
	switch_to_adjacent_buffer(FORWARD);
}

/* Delete an entry from the circular list of open files, and switch to the
 * one after it.  Return FALSE if just one buffer is open, otherwise TRUE. */
bool close_buffer(void)
{
	if (openfile == openfile->next)
		return FALSE;

#ifdef ENABLE_HISTORIES
	if (ISSET(POSITIONLOG))
		update_poshistory(openfile->filename,
						openfile->current->lineno, xplustabs() + 1);
#endif

	switch_to_next_buffer();

	/* Delete the old file buffer, and adjust the count in the top bar. */
	unlink_opennode(openfile->prev);
	if (!inhelp)
		titlebar(NULL);

	/* If now just one buffer remains open, show "Exit" in the help lines. */
	if (openfile == openfile->next)
		exitfunc->desc = exit_tag;

	return TRUE;
}
#endif /* ENABLE_MULTIBUFFER */

/* Do a quick permissions check by verifying whether the file is appendable.
 * Err on the side of permissiveness (reporting TRUE when it might be wrong)
 * to not fluster users editing on odd filesystems by printing incorrect
 * warnings. */
int is_file_writable(const char *filename)
{
	char *full_filename;
	struct stat fileinfo;
	int fd;
	FILE *f;
	bool result = TRUE;

	if (ISSET(VIEW_MODE))
		return TRUE;

	full_filename = get_full_path(filename);

	/* If the absolute path is unusable, use the given relative one. */
	if (full_filename == NULL || stat(full_filename, &fileinfo) == -1)
		full_filename = mallocstrcpy(NULL, filename);

	if ((fd = open(full_filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR |
				S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1)
		result = FALSE;
	else if ((f = fdopen(fd, "a")) == NULL) {
		result = FALSE;
		close(fd);
	} else
		fclose(f);

	free(full_filename);

	return result;
}

/* Encode any NUL bytes in the given line of text, which is of length buf_len,
 * and return a dynamically allocated copy of the resultant string. */
char *encode_data(char *buf, size_t buf_len)
{
	unsunder(buf, buf_len);
	buf[buf_len] = '\0';

	return mallocstrcpy(NULL, buf);
}

/* Read the given open file f into the current buffer.  filename should be
 * set to the name of the file.  undoable means that undo records should be
 * created and that the file does not need to be checked for writability. */
void read_file(FILE *f, int fd, const char *filename, bool undoable)
{
	ssize_t was_lineno = openfile->current->lineno;
		/* The line number where we start the insertion. */
	size_t was_leftedge = 0;
		/* The leftedge where we start the insertion. */
	size_t num_lines = 0;
		/* The number of lines in the file. */
	size_t len = 0;
		/* The length of the current line of the file. */
	char input = '\0';
		/* The current input character. */
	char *buf;
		/* The buffer in which we assemble each line of the file. */
	size_t bufx = MAX_BUF_SIZE;
		/* The allocated size of the line buffer; increased as needed. */
	linestruct *topline;
		/* The top of the new buffer where we store the read file. */
	linestruct *bottomline;
		/* The bottom of the new buffer. */
	int input_int;
		/* The current value we read from the file, whether an input
		 * character or EOF. */
	bool writable = TRUE;
		/* Whether the file is writable (in case we care). */
#ifndef NANO_TINY
	int format = 0;
		/* 0 = *nix, 1 = DOS, 2 = Mac, 3 = both DOS and Mac. */
#endif

	buf = charalloc(bufx);

#ifndef NANO_TINY
	if (undoable)
		add_undo(INSERT);

	if (ISSET(SOFTWRAP))
		was_leftedge = leftedge_for(xplustabs(), openfile->current);
#endif

	/* Create an empty buffer. */
	topline = make_new_node(NULL);
	bottomline = topline;

#ifndef NANO_TINY
	block_sigwinch(TRUE);
#endif

	/* Lock the file before starting to read it, to avoid the overhead
	 * of locking it for each single byte that we read from it. */
	flockfile(f);

	/* Read the entire file into the new buffer. */
	while ((input_int = getc_unlocked(f)) != EOF) {
		input = (char)input_int;

		/* If it's a *nix file ("\n") or a DOS file ("\r\n"), and file
		 * conversion isn't disabled, handle it! */
		if (input == '\n') {
#ifndef NANO_TINY
			/* If it's a DOS file or a DOS/Mac file ('\r' before '\n' on
			 * the first line if we think it's a *nix file, or on any
			 * line otherwise), and file conversion isn't disabled,
			 * handle it! */
			if ((num_lines == 0 || format != 0) && !ISSET(NO_CONVERT) &&
						len > 0 && buf[len - 1] == '\r') {
				if (format == 0 || format == 2)
					format++;
			}
		/* If it's a Mac file ('\r' without '\n' on the first line if we
		 * think it's a *nix file, or on any line otherwise), and file
		 * conversion isn't disabled, handle it! */
		} else if ((num_lines == 0 || format != 0) && !ISSET(NO_CONVERT) &&
						len > 0 && buf[len - 1] == '\r') {
			/* If we currently think the file is a *nix file, set format
			 * to Mac.  If we currently think the file is a DOS file,
			 * set format to both DOS and Mac. */
			if (format == 0 || format == 1)
				format += 2;
#endif
		} else {
			/* Store the character. */
			buf[len] = input;

			/* Keep track of the total length of the line.  It might have
			 * nulls in it, so we can't just use strlen() later. */
			len++;

			/* If needed, increase the buffer size, MAX_BUF_SIZE characters at
			 * a time.  Don't bother decreasing it; it is freed at the end. */
			if (len == bufx) {
				bufx += MAX_BUF_SIZE;
				buf = charealloc(buf, bufx);
			}
			continue;
		}

#ifndef NANO_TINY
		/* If it's a DOS or Mac line, strip the '\r' from it. */
		if (len > 0 && buf[len - 1] == '\r' && !ISSET(NO_CONVERT))
			buf[--len] = '\0';
#endif

		/* Store the data and make a new line. */
		bottomline->data = encode_data(buf, len);
		bottomline->next = make_new_node(bottomline);
		bottomline = bottomline->next;
		num_lines++;

		/* Reset the length in preparation for the next line. */
		len = 0;

#ifndef NANO_TINY
		/* If it happens to be a Mac line, store the character after the \r
		 * as the first character of the next line. */
		if (input != '\n')
			buf[len++] = input;
#endif
	}

	/* We are done with the file, unlock it. */
	funlockfile(f);

#ifndef NANO_TINY
	block_sigwinch(FALSE);
#endif

	/* Perhaps this could use some better handling. */
	if (ferror(f))
		nperror(filename);
	fclose(f);
	if (fd > 0 && !undoable) {
		close(fd);
		writable = is_file_writable(filename);
	}

	/* If the file ended with newline, or it was entirely empty, make the
	 * last line blank.  Otherwise, put the last read data in. */
	if (len == 0)
		bottomline->data = mallocstrcpy(NULL, "");
	else {
		bool mac_line_needs_newline = FALSE;

#ifndef NANO_TINY
		/* If the final character is '\r', and file conversion isn't disabled,
		 * set format to Mac if we currently think the file is a *nix file, or
		 * to DOS-and-Mac if we currently think it is a DOS file. */
		if (buf[len - 1] == '\r' && !ISSET(NO_CONVERT)) {
			if (format < 2)
				format += 2;

			/* Strip the carriage return. */
			buf[--len] = '\0';

			/* Indicate we need to put a blank line in after this one. */
			mac_line_needs_newline = TRUE;
		}
#endif
		/* Store the data of the final line. */
		bottomline->data = encode_data(buf, len);
		num_lines++;

		if (mac_line_needs_newline) {
			bottomline->next = make_new_node(bottomline);
			bottomline = bottomline->next;
			bottomline->data = mallocstrcpy(NULL, "");
		}
	}

	free(buf);

	/* Insert the just read buffer into the current one. */
	ingraft_buffer(topline);

	/* Set the desired x position at the end of what was inserted. */
	openfile->placewewant = xplustabs();

	/* If we've read a help file, don't give any feedback. */
	if (inhelp)
		return;

	if (!writable)
		statusline(ALERT, _("File '%s' is unwritable"), filename);
#ifndef NANO_TINY
	else if (format == 3) {
		/* TRANSLATORS: Keep the next four messages at most 78 characters. */
		statusline(HUSH, P_("Read %zu line (Converted from DOS and Mac format)",
						"Read %zu lines (Converted from DOS and Mac format)",
						num_lines), num_lines);
	} else if (format == 2) {
		openfile->fmt = MAC_FILE;
		statusline(HUSH, P_("Read %zu line (Converted from Mac format)",
						"Read %zu lines (Converted from Mac format)",
						num_lines), num_lines);
	} else if (format == 1) {
		openfile->fmt = DOS_FILE;
		statusline(HUSH, P_("Read %zu line (Converted from DOS format)",
						"Read %zu lines (Converted from DOS format)",
						num_lines), num_lines);
	}
#endif
	else
		statusline(HUSH, P_("Read %zu line", "Read %zu lines",
						num_lines), num_lines);

	/* If we inserted less than a screenful, don't center the cursor. */
	if (undoable && less_than_a_screenful(was_lineno, was_leftedge))
		focusing = FALSE;

#ifndef NANO_TINY
	if (undoable)
		update_undo(INSERT);

	if (ISSET(MAKE_IT_UNIX))
		openfile->fmt = NIX_FILE;
#endif
}

/* Open the file with the given name.  If the file does not exist, display
 * "New File" if newfie is TRUE, and say "File not found" otherwise.
 * Return -2 if we say "New File", -1 if the file isn't opened, and the
 * obtained fd otherwise.  *f is set to the opened file. */
int open_file(const char *filename, bool newfie, bool quiet, FILE **f)
{
	struct stat fileinfo, fileinfo2;
	int fd;
	char *full_filename = get_full_path(filename);

	/* If the full path is unusable (due to some component's permissions),
	 * but the relative path is okay, then just use that one. */
	if (full_filename == NULL || (stat(full_filename, &fileinfo) == -1 &&
										stat(filename, &fileinfo2) != -1))
		full_filename = mallocstrcpy(full_filename, filename);

	if (stat(full_filename, &fileinfo) == -1) {
		if (newfie) {
			if (!quiet)
				statusbar(_("New File"));
			free(full_filename);
			return -2;
		}

		statusline(ALERT, _("File \"%s\" not found"), filename);
		free(full_filename);
		return -1;
	}

	/* Don't open directories, character files, or block files. */
	if (S_ISDIR(fileinfo.st_mode) || S_ISCHR(fileinfo.st_mode) ||
								S_ISBLK(fileinfo.st_mode)) {
		statusline(ALERT, S_ISDIR(fileinfo.st_mode) ?
						_("\"%s\" is a directory") :
						_("\"%s\" is a device file"), filename);
		free(full_filename);
		return -1;
	}

	/* Try opening the file. */
	fd = open(full_filename, O_RDONLY);

	if (fd == -1)
		statusline(ALERT, _("Error reading %s: %s"), filename, strerror(errno));
	else {
		/* The file is A-OK.  Associate a stream with it. */
		*f = fdopen(fd, "rb");

		if (*f == NULL) {
			statusline(ALERT, _("Error reading %s: %s"), filename, strerror(errno));
			close(fd);
		} else if (!inhelp)
			statusbar(_("Reading File"));
	}

	free(full_filename);

	return fd;
}

/* This function will return the name of the first available extension
 * of a filename (starting with [name][suffix], then [name][suffix].1,
 * etc.).  Memory is allocated for the return value.  If no writable
 * extension exists, we return "". */
char *get_next_filename(const char *name, const char *suffix)
{
	unsigned long i = 0;
	char *buf;
	size_t wholenamelen;

	wholenamelen = strlen(name) + strlen(suffix);

	/* Reserve space for: the name plus the suffix plus a dot plus
	 * possibly five digits plus a null byte. */
	buf = charalloc(wholenamelen + 7);
	sprintf(buf, "%s%s", name, suffix);

	while (TRUE) {
		struct stat fs;

		if (stat(buf, &fs) == -1)
			return buf;

		/* Limit the number of backup files to a hundred thousand. */
		if (++i == 100000)
			break;

		sprintf(buf + wholenamelen, ".%lu", i);
	}

	/* There is no possible save file: blank out the filename. */
	*buf = '\0';

	return buf;
}

/* Insert a file into the current buffer, or into a new buffer when
 * the MULTIBUFFER flag is set. */
void do_insertfile(void)
{
	int i;
	const char *msg;
	char *given = mallocstrcpy(NULL, "");
		/* The last answer the user typed at the statusbar prompt. */
#ifndef NANO_TINY
	file_format original_fmt = openfile->fmt;
	bool execute = FALSE;
#endif

	/* Display newlines in filenames as ^J. */
	as_an_at = FALSE;

	while (TRUE) {
#ifndef NANO_TINY
		if (execute) {
#ifdef ENABLE_MULTIBUFFER
			if (ISSET(MULTIBUFFER))
				/* TRANSLATORS: The next six messages are prompts. */
				msg = _("Command to execute in new buffer");
			else
#endif
				msg = _("Command to execute");
		} else
#endif
		{
#ifdef ENABLE_MULTIBUFFER
			if (ISSET(MULTIBUFFER))
#ifndef NANO_TINY
				if ISSET(NO_CONVERT)
					msg = _("File to read unconverted into new buffer [from %s]");
				else
#endif
					msg = _("File to read into new buffer [from %s]");
			else
#endif
#ifndef NANO_TINY
				if ISSET(NO_CONVERT)
					msg = _("File to insert unconverted [from %s]");
				else
#endif
					msg = _("File to insert [from %s]");
		}

		present_path = mallocstrcpy(present_path, "./");

		i = do_prompt(TRUE, TRUE,
#ifndef NANO_TINY
				execute ? MEXTCMD :
#endif
				MINSERTFILE, given,
#ifndef NANO_TINY
				execute ? &execute_history :
#endif
				NULL, edit_refresh, msg,
#ifdef ENABLE_OPERATINGDIR
				operating_dir != NULL ? operating_dir :
#endif
				"./");

		/* If we're in multibuffer mode and the filename or command is
		 * blank, open a new buffer instead of canceling. */
		if (i == -1 || (i == -2 && !ISSET(MULTIBUFFER))) {
			statusbar(_("Cancelled"));
			break;
		} else {
			ssize_t was_current_lineno = openfile->current->lineno;
			size_t was_current_x = openfile->current_x;
#if !defined(NANO_TINY) || defined(ENABLE_BROWSER) || defined(ENABLE_MULTIBUFFER)
			functionptrtype func = func_from_key(&i);
#endif
			given = mallocstrcpy(given, answer);

#ifdef ENABLE_MULTIBUFFER
			if (func == flip_newbuffer) {
				/* Allow toggling only when not in view mode. */
				if (!ISSET(VIEW_MODE))
					TOGGLE(MULTIBUFFER);
				else
					beep();
				continue;
			}
#endif
#ifndef NANO_TINY
			if (func == flip_convert) {
				TOGGLE(NO_CONVERT);
				continue;
			}
			if (func == flip_execute) {
				execute = !execute;
				continue;
			}
#endif
#ifdef ENABLE_BROWSER
			if (func == to_files_void) {
				char *chosen = do_browse_from(answer);

				/* If no file was chosen, go back to the prompt. */
				if (chosen == NULL)
					continue;

				free(answer);
				answer = chosen;
				i = 0;
			}
#endif
#ifndef NANO_TINY
			if (func == flip_pipe) {
				add_or_remove_pipe_symbol_from_answer();
				given = mallocstrcpy(given, answer);
				continue;
			}
#endif
			/* If we don't have a file yet, go back to the prompt. */
			if (i != 0 && (!ISSET(MULTIBUFFER) || i != -2))
				continue;

#ifndef NANO_TINY
			if (execute) {
#ifdef ENABLE_MULTIBUFFER
				/* When in multibuffer mode, first open a blank buffer. */
				if (ISSET(MULTIBUFFER))
					open_buffer("", TRUE);
#endif
				/* If the command is not empty, execute it and read its output
				 * into the buffer, and add the command to the history list. */
				if (*answer != '\0') {
					execute_command(answer);
#ifdef ENABLE_HISTORIES
					update_history(&execute_history, answer);
#endif
				}

#ifdef ENABLE_MULTIBUFFER
				/* If this is a new buffer, put the cursor at the top. */
				if (ISSET(MULTIBUFFER)) {
					openfile->current = openfile->filetop;
					openfile->current_x = 0;
					openfile->placewewant = 0;

					set_modified();
				}
#endif
			} else
#endif /* !NANO_TINY */
			{
				/* Make sure the specified path is tilde-expanded. */
				answer = free_and_assign(answer, real_dir_from_tilde(answer));

				/* Read the file into a new buffer or into current buffer. */
				open_buffer(answer, ISSET(MULTIBUFFER));
			}

#ifdef ENABLE_MULTIBUFFER
			if (ISSET(MULTIBUFFER)) {
#ifdef ENABLE_HISTORIES
				if (ISSET(POSITIONLOG)) {
					ssize_t priorline, priorcol;
#ifndef NANO_TINY
					if (!execute)
#endif
					if (has_old_position(answer, &priorline, &priorcol))
						do_gotolinecolumn(priorline, priorcol, FALSE, FALSE);
				}
#endif
				/* Update title bar and color info for this new buffer. */
				prepare_for_display();
			} else
#endif /* ENABLE_MULTIBUFFER */
			{
				/* If the file actually changed, mark it as modified. */
				if (openfile->current->lineno != was_current_lineno ||
								openfile->current_x != was_current_x)
					set_modified();
#ifndef NANO_TINY
				/* Ensure that the buffer retains the format that it had. */
				openfile->fmt = original_fmt;
#endif
				refresh_needed = TRUE;
			}

			break;
		}
	}

	free(given);
}

/* If the current mode of operation allows it, go insert a file. */
void do_insertfile_void(void)
{
	if (ISSET(RESTRICTED))
		show_restricted_warning();
	else
		do_insertfile();
}

/* When passed "[relative path]" or "[relative path][filename]" in
 * origpath, return "[full path]" or "[full path][filename]" on success,
 * or NULL on error.  Do this if the file doesn't exist but the relative
 * path does, since the file could exist in memory but not yet on disk).
 * Don't do this if the relative path doesn't exist, since we won't be
 * able to go there. */
char *get_full_path(const char *origpath)
{
	int attempts = 0;
		/* How often we've tried climbing back up the tree. */
	struct stat fileinfo;
	char *currentdir, *d_here, *d_there, *d_there_file = NULL;
	char *last_slash;
	bool path_only;

	if (origpath == NULL)
		return NULL;

	/* Get the current directory.  If it doesn't exist, back up and try
	 * again until we get a directory that does, and use that as the
	 * current directory. */
	currentdir = charalloc(PATH_MAX + 1);
	d_here = getcwd(currentdir, PATH_MAX + 1);

	while (d_here == NULL && attempts < 20) {
		IGNORE_CALL_RESULT(chdir(".."));
		d_here = getcwd(currentdir, PATH_MAX + 1);
		attempts++;
	}

	/* If we succeeded, canonicalize it in d_here. */
	if (d_here != NULL) {
		/* If the current directory isn't "/", tack a slash onto the end
		 * of it. */
		if (strcmp(d_here, "/") != 0) {
			d_here = charealloc(d_here, strlen(d_here) + 2);
			strcat(d_here, "/");
		}
	/* Otherwise, set d_here to "". */
	} else {
		d_here = mallocstrcpy(NULL, "");
		free(currentdir);
	}

	d_there = real_dir_from_tilde(origpath);

	/* If stat()ing d_there fails, assume that d_there refers to a new
	 * file that hasn't been saved to disk yet.  Set path_only to TRUE
	 * if d_there refers to a directory, and FALSE otherwise. */
	path_only = (stat(d_there, &fileinfo) != -1 && S_ISDIR(fileinfo.st_mode));

	/* If path_only is TRUE, make sure d_there ends in a slash. */
	if (path_only) {
		size_t d_there_len = strlen(d_there);

		if (d_there[d_there_len - 1] != '/') {
			d_there = charealloc(d_there, d_there_len + 2);
			strcat(d_there, "/");
		}
	}

	/* Search for the last slash in d_there. */
	last_slash = strrchr(d_there, '/');

	/* If we didn't find one, then make sure the answer is in the format
	 * "d_here/d_there". */
	if (last_slash == NULL) {
		assert(!path_only);

		d_there_file = d_there;
		d_there = d_here;
	} else {
		/* If path_only is FALSE, then save the filename portion of the
		 * answer (everything after the last slash) in d_there_file. */
		if (!path_only)
			d_there_file = mallocstrcpy(NULL, last_slash + 1);

		/* Remove the filename portion of the answer from d_there. */
		*(last_slash + 1) = '\0';

		/* Go to the path specified in d_there. */
		if (chdir(d_there) == -1) {
			free(d_there);
			d_there = NULL;
		} else {
			free(d_there);

			/* Get the full path. */
			currentdir = charalloc(PATH_MAX + 1);
			d_there = getcwd(currentdir, PATH_MAX + 1);

			/* If we succeeded, canonicalize it in d_there. */
			if (d_there != NULL) {
				/* If the current directory isn't "/", tack a slash onto
				 * the end of it. */
				if (strcmp(d_there, "/") != 0) {
					d_there = charealloc(d_there, strlen(d_there) + 2);
					strcat(d_there, "/");
				}
			/* Otherwise, make sure that we return NULL. */
			} else {
				path_only = TRUE;
				free(currentdir);
			}

			/* Finally, go back to the path specified in d_here,
			 * where we were before.  We don't check for a chdir()
			 * error, since we can do nothing if we get one. */
			IGNORE_CALL_RESULT(chdir(d_here));
		}

		/* Free d_here, since we're done using it. */
		free(d_here);
	}

	/* At this point, if path_only is FALSE and d_there isn't NULL,
	 * d_there contains the path portion of the answer and d_there_file
	 * contains the filename portion of the answer.  If this is the
	 * case, tack the latter onto the end of the former.  d_there will
	 * then contain the complete answer. */
	if (!path_only && d_there != NULL) {
		d_there = charealloc(d_there, strlen(d_there) +
				strlen(d_there_file) + 1);
		strcat(d_there, d_there_file);
	}

	/* Free d_there_file, since we're done using it. */
	free(d_there_file);

	return d_there;
}

/* Return the full version of path, as returned by get_full_path().  On
 * error, or if path doesn't reference a directory, or if the directory
 * isn't writable, return NULL. */
char *check_writable_directory(const char *path)
{
	char *full_path = get_full_path(path);

	if (full_path == NULL)
		return NULL;

	/* If we can't write to path or path isn't a directory, return NULL. */
	if (access(full_path, W_OK) != 0 ||
				full_path[strlen(full_path) - 1] != '/') {
		free(full_path);
		return NULL;
	}

	return full_path;
}

/* This function calls mkstemp(($TMPDIR|P_tmpdir|/tmp/)"nano.XXXXXX").
 * On success, it returns the malloc()ed filename and corresponding
 * FILE stream, opened in "r+b" mode.  On error, it returns NULL for
 * the filename and leaves the FILE stream unchanged. */
char *safe_tempfile(FILE **f)
{
	const char *tmpdir_env = getenv("TMPDIR");
	char *full_tempdir = NULL, *tempfile_name = NULL;
	mode_t original_umask = 0;
	int fd;

	/* Get the absolute path for the first directory among $TMPDIR
	 * and P_tmpdir that is writable, otherwise use /tmp/. */
	if (tmpdir_env != NULL)
		full_tempdir = check_writable_directory(tmpdir_env);

	if (full_tempdir == NULL)
		full_tempdir = check_writable_directory(P_tmpdir);

	if (full_tempdir == NULL)
		full_tempdir = mallocstrcpy(NULL, "/tmp/");

	tempfile_name = charealloc(full_tempdir, strlen(full_tempdir) + 12);
	strcat(tempfile_name, "nano.XXXXXX");

	original_umask = umask(0);
	umask(S_IRWXG | S_IRWXO);

	fd = mkstemp(tempfile_name);

	if (fd != -1)
		*f = fdopen(fd, "r+b");
	else {
		free(tempfile_name);
		tempfile_name = NULL;
	}

	umask(original_umask);

	return tempfile_name;
}

#ifdef ENABLE_OPERATINGDIR
/* Change to the specified operating directory, when it's valid. */
void init_operating_dir(void)
{
	char *target = get_full_path(operating_dir);

	/* If the operating directory is inaccessible, fail. */
	if (target == NULL || chdir(target) == -1)
		die(_("Invalid operating directory: %s\n"), operating_dir);

	free(operating_dir);
	operating_dir = charealloc(target, strlen(target) + 1);
}

/* Check whether the given path is outside of the operating directory.
 * Return TRUE if it is, and FALSE otherwise.  If allow_tabcomp is TRUE,
 * incomplete names that can grow into matches for the operating directory
 * are considered to be inside, so that tab completion will work. */
bool outside_of_confinement(const char *currpath, bool allow_tabcomp)
{
	char *fullpath;
	bool is_inside, begins_to_be;

	/* If no operating directory is set, there is nothing to check. */
	if (operating_dir == NULL)
		return FALSE;

	fullpath = get_full_path(currpath);

	/* If fullpath is NULL, it means some directory in the path doesn't
	 * exist or is unreadable.  If allow_tabcomp is FALSE, then currpath
	 * is what the user typed somewhere.  We don't want to report a
	 * non-existent directory as being outside the operating directory,
	 * so we return FALSE.  If allow_tabcomp is TRUE, then currpath
	 * exists, but is not executable.  So we say it is outside the
	 * operating directory. */
	if (fullpath == NULL)
		return allow_tabcomp;

	is_inside = (strstr(fullpath, operating_dir) == fullpath);
	begins_to_be = (allow_tabcomp &&
						strstr(operating_dir, fullpath) == operating_dir);
	free(fullpath);

	return (!is_inside && !begins_to_be);
}
#endif

#ifndef NANO_TINY
/* Although this sucks, it sucks less than having a single 'my system is
 * messed up and I'm blanket allowing insecure file writing operations'. */
int prompt_failed_backupwrite(const char *filename)
{
	static int choice;
	static char *prevfile = NULL;
		/* The last filename we were passed, so we don't keep asking this. */

	if (prevfile == NULL || strcmp(filename, prevfile)) {
		choice = do_yesno_prompt(FALSE, _("Failed to write backup file; "
						"continue saving? (Say N if unsure.) "));
		prevfile = mallocstrcpy(prevfile, filename);
	}

	return choice;
}

/* Transform the specified backup directory to an absolute path,
 * and verify that it is usable. */
void init_backup_dir(void)
{
	char *target = get_full_path(backup_dir);

	/* If we can't get an absolute path (which means it doesn't exist or
	 * isn't accessible), or it's not a directory, fail. */
	if (target == NULL || target[strlen(target) - 1] != '/')
		die(_("Invalid backup directory: %s\n"), backup_dir);

	free(backup_dir);
	backup_dir = charealloc(target, strlen(target) + 1);
}
#endif /* !NANO_TINY */

/* Read from inn, write to out.  We assume inn is opened for reading,
 * and out for writing.  We return 0 on success, -1 on read error, or -2
 * on write error.  inn is always closed by this function, out is closed
 * only if close_out is true. */
int copy_file(FILE *inn, FILE *out, bool close_out)
{
	int retval = 0;
	char buf[BUFSIZ];
	size_t charsread;
	int (*flush_out_fnc)(FILE *) = (close_out) ? fclose : fflush;

	assert(inn != NULL && out != NULL && inn != out);

	do {
		charsread = fread(buf, sizeof(char), BUFSIZ, inn);
		if (charsread == 0 && ferror(inn)) {
			retval = -1;
			break;
		}
		if (fwrite(buf, sizeof(char), charsread, out) < charsread) {
			retval = -2;
			break;
		}
	} while (charsread > 0);

	if (fclose(inn) == EOF)
		retval = -1;
	if (flush_out_fnc(out) == EOF)
		retval = -2;

	return retval;
}

/* Write a file out to disk.  If f_open isn't NULL, we assume that it is
 * a stream associated with the file, and we don't try to open it
 * ourselves.  If tmp is TRUE, we set the umask to disallow anyone else
 * from accessing the file, we don't set the filename to its name, and
 * we don't print out how many lines we wrote on the statusbar.
 *
 * tmp means we are writing a temporary file in a secure fashion.  We
 * use it when spell checking or dumping the file on an error.  If
 * method is APPEND, it means we are appending instead of overwriting.
 * If method is PREPEND, it means we are prepending instead of
 * overwriting.  If fullbuffer is TRUE, we set the current filename and
 * stat info.  But fullbuffer is irrelevant when appending or prepending,
 * or when writing a temporary file.
 *
 * Return TRUE on success or FALSE on error. */
bool write_file(const char *name, FILE *f_open, bool tmp,
		kind_of_writing_type method, bool fullbuffer)
{
	bool retval = FALSE;
		/* Instead of returning in this function, you should always
		 * set retval and then goto cleanup_and_exit. */
	size_t lineswritten = 0;
	const linestruct *fileptr = openfile->filetop;
	int fd;
		/* The file descriptor we use. */
	mode_t original_umask = 0;
		/* Our umask, from when nano started. */
#ifndef NANO_TINY
	bool realexists;
		/* The result of stat().  TRUE if the file exists, FALSE
		 * otherwise.  If name is a link that points nowhere, realexists
		 * is FALSE. */
#endif
	struct stat st;
		/* The status fields filled in by stat(). */
	char *realname;
		/* The filename after tilde expansion. */
	FILE *f = f_open;
		/* The actual file, realname, we are writing to. */
	char *tempname = NULL;
		/* The name of the temporary file we write to on prepend. */

	if (*name == '\0')
		return -1;

	if (!tmp)
		titlebar(NULL);

	realname = real_dir_from_tilde(name);

#ifdef ENABLE_OPERATINGDIR
	/* If we're writing a temporary file, we're probably going outside
	 * the operating directory, so skip the operating directory test. */
	if (!tmp && outside_of_confinement(realname, FALSE)) {
		statusline(ALERT, _("Can't write outside of %s"), operating_dir);
		goto cleanup_and_exit;
	}
#endif

	/* If the temp file exists and isn't already open, give up. */
	if (tmp && (lstat(realname, &st) != -1) && f_open == NULL)
		goto cleanup_and_exit;

#ifndef NANO_TINY
	/* Check whether the file (at the end of the symlink) exists. */
	realexists = (stat(realname, &st) != -1);

	/* If we haven't stat()d this file before (say, the user just
	 * specified it interactively), stat and save the value now,
	 * or else we will chase null pointers when we do modtime checks,
	 * preserve file times, and so on, during backup. */
	if (openfile->current_stat == NULL && !tmp && realexists)
		stat_with_alloc(realname, &openfile->current_stat);

	/* We backup only if the backup toggle is set, the file isn't
	 * temporary, and the file already exists.  Furthermore, if we
	 * aren't appending, prepending, or writing a selection, we backup
	 * only if the file has not been modified by someone else since nano
	 * opened it. */
	if (ISSET(BACKUP_FILE) && !tmp && realexists && openfile->current_stat &&
				(method != OVERWRITE || openfile->mark ||
				openfile->current_stat->st_mtime == st.st_mtime)) {
		static struct timespec filetime[2];
		char *backupname;
		int backup_cflags, backup_fd;
		FILE *backup_file = NULL;

		/* Save the original file's access and modification times. */
		filetime[0].tv_sec = openfile->current_stat->st_atime;
		filetime[1].tv_sec = openfile->current_stat->st_mtime;

		if (f_open == NULL) {
			/* Open the original file to copy to the backup. */
			f = fopen(realname, "rb");

			if (f == NULL) {
				statusline(ALERT, _("Error reading %s: %s"), realname,
						strerror(errno));
				/* If we can't read from the original file, go on, since
				 * only saving the current buffer is better than saving
				 * nothing. */
				goto skip_backup;
			}
		}

		/* If backup_dir is set, we set backupname to
		 * backup_dir/backupname~[.number], where backupname is the
		 * canonicalized absolute pathname of realname with every '/'
		 * replaced with a '!'.  This means that /home/foo/file is
		 * backed up in backup_dir/!home!foo!file~[.number]. */
		if (backup_dir != NULL) {
			char *backuptemp = get_full_path(realname);

			/* If we can't get a canonical absolute path, just use the
			 * filename portion of the given path.  Otherwise, replace
			 * slashes with exclamation marks in the full path. */
			if (backuptemp == NULL)
				backuptemp = mallocstrcpy(NULL, tail(realname));
			else {
				size_t i = 0;

				for (; backuptemp[i] != '\0'; i++) {
					if (backuptemp[i] == '/')
						backuptemp[i] = '!';
				}
			}

			backupname = charalloc(strlen(backup_dir) + strlen(backuptemp) + 1);
			sprintf(backupname, "%s%s", backup_dir, backuptemp);
			free(backuptemp);
			backuptemp = get_next_filename(backupname, "~");
			if (*backuptemp == '\0') {
				statusline(HUSH, _("Error writing backup file %s: %s"),
						backupname, _("Too many backup files?"));
				free(backuptemp);
				free(backupname);
				/* If we can't write to the backup, DON'T go on, since
				 * whatever caused the backup-file write to fail (e.g.
				 * disk full) may well cause the real file write to fail
				 * too, which means we could lose the original! */
				goto cleanup_and_exit;
			} else {
				free(backupname);
				backupname = backuptemp;
			}
		} else {
			backupname = charalloc(strlen(realname) + 2);
			sprintf(backupname, "%s~", realname);
		}

		/* First, unlink any existing backups.  Next, open the backup
		 * file with O_CREAT and O_EXCL.  If it succeeds, we have a file
		 * descriptor to a new backup file. */
		if (unlink(backupname) < 0 && errno != ENOENT && !ISSET(INSECURE_BACKUP)) {
			if (prompt_failed_backupwrite(backupname))
				goto skip_backup;
			statusline(HUSH, _("Error writing backup file %s: %s"),
						backupname, strerror(errno));
			free(backupname);
			goto cleanup_and_exit;
		}

		if (ISSET(INSECURE_BACKUP))
			backup_cflags = O_WRONLY | O_CREAT | O_APPEND;
		else
			backup_cflags = O_WRONLY | O_CREAT | O_EXCL | O_APPEND;

		backup_fd = open(backupname, backup_cflags,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

		if (backup_fd >= 0)
			backup_file = fdopen(backup_fd, "wb");

		if (backup_file == NULL) {
			statusline(HUSH, _("Error writing backup file %s: %s"),
						backupname, strerror(errno));
			free(backupname);
			goto cleanup_and_exit;
		}

		/* Only try chowning the backup when we're root. */
		if (geteuid() == NANO_ROOT_UID &&
						fchown(backup_fd, openfile->current_stat->st_uid,
						openfile->current_stat->st_gid) == -1 &&
						!ISSET(INSECURE_BACKUP)) {
			fclose(backup_file);
			if (prompt_failed_backupwrite(backupname))
				goto skip_backup;
			statusline(HUSH, _("Error writing backup file %s: %s"),
						backupname, strerror(errno));
			free(backupname);
			goto cleanup_and_exit;
		}

		/* Set the backup's mode bits. */
		if (fchmod(backup_fd, openfile->current_stat->st_mode) == -1 &&
						!ISSET(INSECURE_BACKUP)) {
			fclose(backup_file);
			if (prompt_failed_backupwrite(backupname))
				goto skip_backup;
			statusline(HUSH, _("Error writing backup file %s: %s"),
						backupname, strerror(errno));
			free(backupname);
			goto cleanup_and_exit;
		}

#ifdef DEBUG
		fprintf(stderr, "Backing up %s to %s\n", realname, backupname);
#endif

		/* Copy the file. */
		if (copy_file(f, backup_file, FALSE) != 0) {
			fclose(backup_file);
			statusline(ALERT, _("Error reading %s: %s"), realname,
						strerror(errno));
			goto cleanup_and_exit;
		}

		/* And set the backup's timestamps. */
		if (futimens(backup_fd, filetime) == -1 && !ISSET(INSECURE_BACKUP)) {
			fclose(backup_file);
			if (prompt_failed_backupwrite(backupname))
				goto skip_backup;
			statusline(HUSH, _("Error writing backup file %s: %s"),
						backupname, strerror(errno));
			goto cleanup_and_exit;
		}

		fclose(backup_file);
		free(backupname);
	}

	skip_backup:
#endif /* !NANO_TINY */

	if (f_open == NULL) {
		original_umask = umask(0);

		/* If we create a temp file, we don't let anyone else access it. */
		if (tmp)
			umask(S_IRWXG | S_IRWXO);
		else
			umask(original_umask);
	}

#ifndef NANO_TINY
	/* If we're prepending, copy the file to a temp file. */
	if (method == PREPEND) {
		int fd_source;
		FILE *f_source = NULL;

		if (f == NULL) {
			f = fopen(realname, "rb");

			if (f == NULL) {
				statusline(ALERT, _("Error reading %s: %s"), realname,
						strerror(errno));
				goto cleanup_and_exit;
			}
		}

		tempname = safe_tempfile(&f);

		if (tempname == NULL) {
			statusline(ALERT, _("Error writing temp file: %s"),
						strerror(errno));
			goto cleanup_and_exit;
		}

		if (f_open == NULL) {
			fd_source = open(realname, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);

			if (fd_source != -1) {
				f_source = fdopen(fd_source, "rb");
				if (f_source == NULL) {
					statusline(ALERT, _("Error reading %s: %s"), realname,
								strerror(errno));
					close(fd_source);
					fclose(f);
					unlink(tempname);
					goto cleanup_and_exit;
				}
			}
		}

		if (f_source == NULL || copy_file(f_source, f, TRUE) != 0) {
			statusline(ALERT, _("Error writing temp file: %s"),
						strerror(errno));
			unlink(tempname);
			goto cleanup_and_exit;
		}
	}
#endif /* !NANO_TINY */

	if (f_open == NULL) {
		/* Now open the file in place.  Use O_EXCL if tmp is TRUE.  This
		 * is copied from joe, because wiggy says so *shrug*. */
		fd = open(realname, O_WRONLY | O_CREAT | ((method == APPEND) ?
				O_APPEND : (tmp ? O_EXCL : O_TRUNC)), S_IRUSR |
				S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

		/* Set the umask back to the user's original value. */
		umask(original_umask);

		/* If we couldn't open the file, give up. */
		if (fd == -1) {
			statusline(ALERT, _("Error writing %s: %s"), realname,
						strerror(errno));
			if (tempname != NULL)
				unlink(tempname);
			goto cleanup_and_exit;
		}

		f = fdopen(fd, (method == APPEND) ? "ab" : "wb");

		if (f == NULL) {
			statusline(ALERT, _("Error writing %s: %s"), realname,
						strerror(errno));
			close(fd);
			goto cleanup_and_exit;
		}
	}

	while (fileptr != NULL) {
		size_t data_len = strlen(fileptr->data), size;

		/* Decode LFs as the NULs that they are, before writing to disk. */
		sunder(fileptr->data);

		size = fwrite(fileptr->data, sizeof(char), data_len, f);

		/* Re-encode any embedded NULs as LFs. */
		unsunder(fileptr->data, data_len);

		if (size < data_len) {
			statusline(ALERT, _("Error writing %s: %s"), realname,
						strerror(errno));
			fclose(f);
			goto cleanup_and_exit;
		}

		/* If we're on the last line of the file, don't write a newline
		 * character after it.  If the last line of the file is blank,
		 * this means that zero bytes are written, in which case we
		 * don't count the last line in the total lines written. */
		if (fileptr == openfile->filebot) {
			if (fileptr->data[0] == '\0')
				lineswritten--;
		} else {
#ifndef NANO_TINY
			if (openfile->fmt == DOS_FILE || openfile->fmt == MAC_FILE) {
				if (putc('\r', f) == EOF) {
					statusline(ALERT, _("Error writing %s: %s"), realname,
								strerror(errno));
					fclose(f);
					goto cleanup_and_exit;
				}
			}

			if (openfile->fmt != MAC_FILE)
#endif
				if (putc('\n', f) == EOF) {
					statusline(ALERT, _("Error writing %s: %s"), realname,
								strerror(errno));
					fclose(f);
					goto cleanup_and_exit;
				}
		}

		fileptr = fileptr->next;
		lineswritten++;
	}

#ifndef NANO_TINY
	/* If we're prepending, open the temp file, and append it to f. */
	if (method == PREPEND) {
		int fd_source;
		FILE *f_source = NULL;

		fd_source = open(tempname, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);

		if (fd_source != -1) {
			f_source = fdopen(fd_source, "rb");
			if (f_source == NULL)
				close(fd_source);
		}

		if (f_source == NULL) {
			statusline(ALERT, _("Error reading %s: %s"), tempname,
						strerror(errno));
			fclose(f);
			goto cleanup_and_exit;
		}

		if (copy_file(f_source, f, TRUE) != 0) {
			statusline(ALERT, _("Error writing %s: %s"), realname,
						strerror(errno));
			goto cleanup_and_exit;
		}

		unlink(tempname);
	} else
#endif
	if (fclose(f) != 0) {
		statusline(ALERT, _("Error writing %s: %s"), realname,
						strerror(errno));
		goto cleanup_and_exit;
	}

	/* When having written an entire buffer, update some administrivia. */
	if (fullbuffer && method == OVERWRITE && !tmp) {
		/* If the filename was changed, check if this means a new syntax. */
		if (strcmp(openfile->filename, realname) != 0) {
#ifdef ENABLE_COLOR
			const char *oldname, *newname;

			oldname = openfile->syntax ? openfile->syntax->name : "";
#endif
			openfile->filename = mallocstrcpy(openfile->filename, realname);
#ifdef ENABLE_COLOR
			/* See if the applicable syntax has changed. */
			color_update();
			color_init();

			newname = openfile->syntax ? openfile->syntax->name : "";

			/* If the syntax changed, discard and recompute the multidata. */
			if (strcmp(oldname, newname) != 0) {
				linestruct *line = openfile->filetop;

				while (line != NULL) {
					free(line->multidata);
					line->multidata = NULL;
					line = line->next;
				}

				precalc_multicolorinfo();
				refresh_needed = TRUE;
			}
#endif
		}
#ifndef NANO_TINY
		/* Get or update the stat info to reflect the current state. */
		stat_with_alloc(realname, &openfile->current_stat);

		/* Record at which point in the undo stack the file was saved. */
		openfile->last_saved = openfile->current_undo;
		openfile->last_action = OTHER;
#endif
		openfile->modified = FALSE;
		titlebar(NULL);
	}

	if (!tmp)
		statusline(HUSH, P_("Wrote %zu line", "Wrote %zu lines",
								lineswritten), lineswritten);
	retval = TRUE;

  cleanup_and_exit:
	free(realname);
	free(tempname);

	return retval;
}

#ifndef NANO_TINY
/* Write a marked selection from a file out to disk.  Return TRUE on
 * success or FALSE on error. */
bool write_marked_file(const char *name, FILE *f_open, bool tmp,
		kind_of_writing_type method)
{
	bool retval;
	bool added_magicline = FALSE;
	linestruct *top, *bot;
	size_t top_x, bot_x;

	/* Partition the buffer so that it contains only the marked text. */
	mark_order((const linestruct **)&top, &top_x,
				(const linestruct **)&bot, &bot_x, NULL);
	filepart = partition_buffer(top, top_x, bot, bot_x);

	/* If we are using a magic line, and the last line of the partition
	 * isn't blank, then add a newline at the end of the buffer. */
	if (!ISSET(NO_NEWLINES) && openfile->filebot->data[0] != '\0') {
		new_magicline();
		added_magicline = TRUE;
	}

	retval = write_file(name, f_open, tmp, method, FALSE);

	if (added_magicline)
		remove_magicline();

	/* Unpartition the buffer so that it contains all the text again. */
	unpartition_buffer(&filepart);

	return retval;
}

#endif /* !NANO_TINY */

/* Write the current file to disk.  If the mark is on, write the current
 * marked selection to disk.  If exiting is TRUE, write the entire file
 * to disk regardless of whether the mark is on.  Do not ask for a name
 * when withprompt is FALSE nor when the TEMP_FILE flag is set and the
 * file already has a name.  Return 0 on error, 1 on success, and 2 when
 * the buffer is to be discarded. */
int do_writeout(bool exiting, bool withprompt)
{
	bool result = FALSE;
	kind_of_writing_type method = OVERWRITE;
	char *given;
		/* The filename we offer, or what the user typed so far. */
	bool maychange = (openfile->filename[0] == '\0');
		/* Whether it's okay to save the file under a different name. */
#ifdef ENABLE_EXTRA
	static bool did_credits = FALSE;
#endif

	/* Display newlines in filenames as ^J. */
	as_an_at = FALSE;

	given = mallocstrcpy(NULL,
#ifndef NANO_TINY
		(openfile->mark && !exiting) ? "" :
#endif
		openfile->filename);

	while (TRUE) {
		const char *msg;
		int response = 0, choice = 0;
		functionptrtype func;
#ifndef NANO_TINY
		const char *formatstr, *backupstr;

		formatstr = (openfile->fmt == DOS_FILE) ? _(" [DOS Format]") :
						(openfile->fmt == MAC_FILE) ? _(" [Mac Format]") : "";
		backupstr = ISSET(BACKUP_FILE) ? _(" [Backup]") : "";

		/* When the mark is on, offer to write the selection to disk, but
		 * not when in restricted mode, because it would allow writing to
		 * a file not specified on the command line. */
		if (openfile->mark && !exiting && !ISSET(RESTRICTED))
			/* TRANSLATORS: The next six strings are prompts. */
			msg = (method == PREPEND) ? _("Prepend Selection to File") :
						(method == APPEND) ? _("Append Selection to File") :
						_("Write Selection to File");
		else if (method != OVERWRITE)
			msg = (method == PREPEND) ? _("File Name to Prepend to") :
										_("File Name to Append to");
		else
#endif /* !NANO_TINY */
			msg = _("File Name to Write");

		present_path = mallocstrcpy(present_path, "./");

		/* When we shouldn't prompt, use the existing filename. */
		if ((!withprompt || (ISSET(TEMP_FILE) && exiting)) &&
								openfile->filename[0] != '\0')
			answer = mallocstrcpy(answer, openfile->filename);
		else {
			/* Ask for (confirmation of) the filename.  Disable tab completion
			 * when using restricted mode and the filename isn't blank. */
			response = do_prompt(!ISSET(RESTRICTED) || openfile->filename[0] == '\0',
						TRUE, MWRITEFILE, given, NULL,
						edit_refresh, "%s%s%s", msg,
#ifndef NANO_TINY
						formatstr, backupstr
#else
						"", ""
#endif
						);
		}

		if (response < 0) {
			statusbar(_("Cancelled"));
			break;
		}

		func = func_from_key(&response);

		/* Upon request, abandon the buffer. */
		if (func == discard_buffer) {
			free(given);
			return 2;
		}

		given = mallocstrcpy(given, answer);

#ifdef ENABLE_BROWSER
		if (func == to_files_void) {
			char *chosen = do_browse_from(answer);

			if (chosen == NULL)
				continue;

			free(answer);
			answer = chosen;
		} else
#endif
#ifndef NANO_TINY
		if (func == dos_format_void) {
			openfile->fmt = (openfile->fmt == DOS_FILE) ? NIX_FILE : DOS_FILE;
			continue;
		} else if (func == mac_format_void) {
			openfile->fmt = (openfile->fmt == MAC_FILE) ? NIX_FILE : MAC_FILE;
			continue;
		} else if (func == backup_file_void) {
			TOGGLE(BACKUP_FILE);
			continue;
		} else if (func == prepend_void) {
			method = (method == PREPEND) ? OVERWRITE : PREPEND;
			continue;
		} else if (func == append_void) {
			method = (method == APPEND) ? OVERWRITE : APPEND;
			continue;
		}
#endif /* !NANO_TINY */
		if (func == do_help_void) {
			continue;
		}
#ifdef ENABLE_EXTRA
		/* If the current file has been modified, we've pressed
		 * Ctrl-X at the edit window to exit, we've pressed "y" at
		 * the "Save modified buffer" prompt to save, we've entered
		 * "zzy" as the filename to save under (hence "xyzzy"), and
		 * this is the first time we've done this, show an Easter
		 * egg.  Display the credits. */
		if (!did_credits && exiting && !ISSET(TEMP_FILE) &&
								strcasecmp(answer, "zzy") == 0) {
			if (LINES > 5 && COLS > 31) {
				do_credits();
				did_credits = TRUE;
			} else
				/* TRANSLATORS: Concisely say the screen is too small. */
				statusbar(_("Too tiny"));
			break;
		}
#endif

		if (method == OVERWRITE) {
			bool name_exists, do_warning;
			char *full_answer, *full_filename;
			struct stat st;

			full_answer = get_full_path(answer);
			full_filename = get_full_path(openfile->filename);
			name_exists = (stat((full_answer == NULL) ?
								answer : full_answer, &st) != -1);
			if (openfile->filename[0] == '\0')
				do_warning = name_exists;
			else
				do_warning = (strcmp((full_answer == NULL) ?
								answer : full_answer, (full_filename == NULL) ?
								openfile->filename : full_filename) != 0);

			free(full_filename);
			free(full_answer);

			if (do_warning) {
				/* When in restricted mode, we aren't allowed to overwrite
				 * an existing file with the current buffer, nor to change
				 * the name of the current file if it already has one. */
				if (ISSET(RESTRICTED)) {
					/* TRANSLATORS: Restricted mode forbids overwriting. */
					warn_and_shortly_pause(_("File exists -- "
												"cannot overwrite"));
					continue;
				}

				if (!maychange) {
#ifndef NANO_TINY
					if (exiting || !openfile->mark)
#endif
					{
						if (do_yesno_prompt(FALSE, _("Save file under "
												"DIFFERENT NAME? ")) < 1)
							continue;
						maychange = TRUE;
					}
				}

				if (name_exists) {
					char *question = _("File \"%s\" exists; OVERWRITE? ");
					char *name = display_string(answer, 0,
										COLS - breadth(question) + 1, FALSE, FALSE);
					char *message = charalloc(strlen(question) +
												strlen(name) + 1);

					sprintf(message, question, name);

					choice = do_yesno_prompt(FALSE, message);

					free(message);
					free(name);

					if (choice < 1)
						continue;
				}
			}
#ifndef NANO_TINY
			/* Complain if the file exists, the name hasn't changed,
			 * and the stat information we had before does not match
			 * what we have now. */
			else if (name_exists && openfile->current_stat &&
						(openfile->current_stat->st_mtime < st.st_mtime ||
						openfile->current_stat->st_dev != st.st_dev ||
						openfile->current_stat->st_ino != st.st_ino)) {

				warn_and_shortly_pause(_("File on disk has changed"));

				choice = do_yesno_prompt(FALSE, _("File was modified "
								"since you opened it; continue saving? "));
				wipe_statusbar();

				/* When in tool mode and not called by 'savefile',
				 * overwrite the file right here when requested. */
				if (ISSET(TEMP_FILE) && withprompt) {
					free(given);
					if (choice == 1)
						return write_file(openfile->filename, NULL,
											FALSE, OVERWRITE, TRUE);
					else if (choice == 0)
						return 2;
					else
						return 0;
				} else if (choice != 1) {
					free(given);
					return 1;
				}
			}
#endif
		}

		/* Here's where we allow the selected text to be written to
		 * a separate file.  If we're using restricted mode, this
		 * function is disabled, since it allows reading from or
		 * writing to files not specified on the command line. */
#ifndef NANO_TINY
		if (openfile->mark && !exiting && withprompt && !ISSET(RESTRICTED))
			result = write_marked_file(answer, NULL, FALSE, method);
		else
#endif
			result = write_file(answer, NULL, FALSE, method, TRUE);

		break;
	}

	free(given);

	return result ? 1 : 0;
}

/* Write the current buffer to disk, or discard it. */
void do_writeout_void(void)
{
	/* If the user chose to discard the buffer, close it. */
	if (do_writeout(FALSE, TRUE) == 2)
		close_and_go();
}

/* If it has a name, write the current file to disk without prompting. */
void do_savefile(void)
{
	if (do_writeout(FALSE, FALSE) == 2)
		close_and_go();
}

/* Return a malloc()ed string containing the actual directory, used to
 * convert ~user/ and ~/ notation. */
char *real_dir_from_tilde(const char *buf)
{
	char *retval;

	if (*buf == '~') {
		size_t i = 1;
		char *tilde_dir;

		/* Figure out how much of the string we need to compare. */
		for (; buf[i] != '/' && buf[i] != '\0'; i++)
			;

		/* Get the home directory. */
		if (i == 1) {
			get_homedir();
			tilde_dir = mallocstrcpy(NULL, homedir);
		} else {
#ifdef HAVE_PWD_H
			const struct passwd *userdata;

			tilde_dir = mallocstrncpy(NULL, buf, i + 1);
			tilde_dir[i] = '\0';

			do {
				userdata = getpwent();
			} while (userdata != NULL &&
						strcmp(userdata->pw_name, tilde_dir + 1) != 0);
			endpwent();
			if (userdata != NULL)
				tilde_dir = mallocstrcpy(tilde_dir, userdata->pw_dir);
#else
			tilde_dir = strdup("");
#endif
		}

		retval = charalloc(strlen(tilde_dir) + strlen(buf + i) + 1);
		sprintf(retval, "%s%s", tilde_dir, buf + i);

		free(tilde_dir);
	} else
		retval = mallocstrcpy(NULL, buf);

	return retval;
}

#if defined(ENABLE_TABCOMP) || defined(ENABLE_BROWSER)
/* Our sort routine for file listings.  Sort alphabetically and
 * case-insensitively, and sort directories before filenames. */
int diralphasort(const void *va, const void *vb)
{
	struct stat fileinfo;
	const char *a = *(const char *const *)va;
	const char *b = *(const char *const *)vb;
	bool aisdir = stat(a, &fileinfo) != -1 && S_ISDIR(fileinfo.st_mode);
	bool bisdir = stat(b, &fileinfo) != -1 && S_ISDIR(fileinfo.st_mode);

	if (aisdir && !bisdir)
		return -1;
	if (!aisdir && bisdir)
		return 1;

	/* Standard function brain damage: We should be sorting
	 * alphabetically and case-insensitively according to the current
	 * locale, but there's no standard strcasecoll() function, so we
	 * have to use multibyte strcasecmp() instead. */
	return mbstrcasecmp(a, b);
}
#endif

#ifdef ENABLE_TABCOMP
/* Return TRUE when the given path is a directory. */
bool is_dir(const char *path)
{
	char *realpath = real_dir_from_tilde(path);
	struct stat fileinfo;
	bool retval;

	retval = (stat(realpath, &fileinfo) != -1 && S_ISDIR(fileinfo.st_mode));

	free(realpath);

	return retval;
}

/* These functions, username_tab_completion(), cwd_tab_completion()
 * (originally exe_n_cwd_tab_completion()), and input_tab(), were
 * adapted from busybox 0.46 (cmdedit.c).  Here is the notice from that
 * file, with the copyright years updated:
 *
 * Termios command line History and Editting, originally
 * intended for NetBSD sh (ash)
 * Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007
 *      Main code:            Adam Rogoyski <rogoyski@cs.utexas.edu>
 *      Etc:                  Dave Cinege <dcinege@psychosis.com>
 *  Majorly adjusted/re-written for busybox:
 *                            Erik Andersen <andersee@debian.org>
 *
 * You may use this code as you wish, so long as the original author(s)
 * are attributed in any redistributions of the source code.
 * This code is 'as is' with no warranty.
 * This code may safely be consumed by a BSD or GPL license. */

/* We consider the first buf_len characters of buf for ~username tab
 * completion. */
char **username_tab_completion(const char *buf, size_t *num_matches,
		size_t buf_len)
{
	char **matches = NULL;
#ifdef HAVE_PWD_H
	const struct passwd *userdata;
#endif

	assert(buf != NULL && num_matches != NULL && buf_len > 0);

	*num_matches = 0;

#ifdef HAVE_PWD_H
	while ((userdata = getpwent()) != NULL) {
		if (strncmp(userdata->pw_name, buf + 1, buf_len - 1) == 0) {
			/* Cool, found a match.  Add it to the list.  This makes a
			 * lot more sense to me (Chris) this way... */

#ifdef ENABLE_OPERATINGDIR
			/* ...unless the match exists outside the operating
			 * directory, in which case just go to the next match. */
			if (outside_of_confinement(userdata->pw_dir, TRUE))
				continue;
#endif

			matches = (char **)nrealloc(matches, (*num_matches + 1) *
										sizeof(char *));
			matches[*num_matches] = charalloc(strlen(userdata->pw_name) + 2);
			sprintf(matches[*num_matches], "~%s", userdata->pw_name);
			++(*num_matches);
		}
	}
	endpwent();
#endif

	return matches;
}

/* We consider the first buf_len characters of buf for filename tab
 * completion. */
char **cwd_tab_completion(const char *buf, bool allow_files,
		size_t *num_matches, size_t buf_len)
{
	char *dirname = mallocstrcpy(NULL, buf);
	char *slash, *filename;
	size_t filenamelen;
	char **matches = NULL;
	DIR *dir;
	const struct dirent *nextdir;

	*num_matches = 0;
	dirname[buf_len] = '\0';

	/* If there's a / in the name, split out filename and directory parts. */
	slash = strrchr(dirname, '/');
	if (slash != NULL) {
		char *wasdirname = dirname;

		filename = mallocstrcpy(NULL, ++slash);
		/* Cut off the filename part after the slash. */
		*slash = '\0';
		dirname = real_dir_from_tilde(dirname);
		/* A non-absolute path is relative to the current browser directory. */
		if (dirname[0] != '/') {
			dirname = charealloc(dirname, strlen(present_path) +
												strlen(wasdirname) + 1);
			sprintf(dirname, "%s%s", present_path, wasdirname);
		}
		free(wasdirname);
	} else {
		filename = dirname;
		dirname = mallocstrcpy(NULL, present_path);
	}

	assert(dirname[strlen(dirname) - 1] == '/');

	dir = opendir(dirname);

	if (dir == NULL) {
		/* Don't print an error, just shut up and return. */
		beep();
		free(filename);
		free(dirname);
		return NULL;
	}

	filenamelen = strlen(filename);

	while ((nextdir = readdir(dir)) != NULL) {
		bool skip_match = FALSE;

		/* See if this matches. */
		if (strncmp(nextdir->d_name, filename, filenamelen) == 0 &&
				(*filename == '.' || (strcmp(nextdir->d_name, ".") != 0 &&
				strcmp(nextdir->d_name, "..") != 0))) {
			/* Cool, found a match.  Add it to the list.  This makes a
			 * lot more sense to me (Chris) this way... */

			char *tmp = charalloc(strlen(dirname) + strlen(nextdir->d_name) + 1);
			sprintf(tmp, "%s%s", dirname, nextdir->d_name);

#ifdef ENABLE_OPERATINGDIR
			/* ...unless the match exists outside the operating
			 * directory, in which case just go to the next match. */
			skip_match = outside_of_confinement(tmp, TRUE);
#endif

			/* ...or unless the match isn't a directory and allow_files
			 * isn't set, in which case just go to the next match. */
			skip_match = skip_match || (!allow_files && !is_dir(tmp));

			free(tmp);

			if (skip_match)
				continue;

			matches = (char **)nrealloc(matches, (*num_matches + 1) *
										sizeof(char *));
			matches[*num_matches] = mallocstrcpy(NULL, nextdir->d_name);
			++(*num_matches);
		}
	}

	closedir(dir);
	free(dirname);
	free(filename);

	return matches;
}

/* Do tab completion.  place refers to how much the statusbar cursor
 * position should be advanced.  refresh_func is the function we will
 * call to refresh the edit window. */
char *input_tab(char *buf, bool allow_files, size_t *place,
		bool *lastwastab, void (*refresh_func)(void), bool *listed)
{
	size_t num_matches = 0, buf_len;
	char **matches = NULL;

	assert(buf != NULL && place != NULL && *place <= strlen(buf) &&
				lastwastab != NULL && refresh_func != NULL && listed != NULL);

	*listed = FALSE;

	/* If the word starts with `~' and there is no slash in the word,
	 * then try completing this word as a username. */
	if (*place > 0 && *buf == '~') {
		const char *slash = strchr(buf, '/');

		if (slash == NULL || slash >= buf + *place)
			matches = username_tab_completion(buf, &num_matches, *place);
	}

	/* Match against files relative to the current working directory. */
	if (matches == NULL)
		matches = cwd_tab_completion(buf, allow_files, &num_matches, *place);

	buf_len = strlen(buf);

	if (num_matches == 0 || *place != buf_len)
		beep();
	else {
		size_t match, common_len = 0;
		char *mzero, *glued;
		const char *lastslash = revstrstr(buf, "/", buf + *place);
		size_t lastslash_len = (lastslash == NULL) ? 0 : lastslash - buf + 1;
		char char1[MAXCHARLEN], char2[MAXCHARLEN];
		int len1, len2;

		/* Get the number of characters that all matches have in common. */
		while (TRUE) {
			len1 = parse_mbchar(matches[0] + common_len, char1, NULL);

			for (match = 1; match < num_matches; match++) {
				len2 = parse_mbchar(matches[match] + common_len, char2, NULL);

				if (len1 != len2 || strncmp(char1, char2, len2) != 0)
					break;
			}

			if (match < num_matches || matches[0][common_len] == '\0')
				break;

			common_len += len1;
		}

		mzero = charalloc(lastslash_len + common_len + 1);

		strncpy(mzero, buf, lastslash_len);
		strncpy(mzero + lastslash_len, matches[0], common_len);

		common_len += lastslash_len;
		mzero[common_len] = '\0';

		/* Cover also the case of the user specifying a relative path. */
		glued = charalloc(strlen(present_path) + strlen(mzero) + 1);
		sprintf(glued, "%s%s", present_path, mzero);

		assert(common_len >= *place);

		if (num_matches == 1 && (is_dir(mzero) || is_dir(glued))) {
			mzero[common_len++] = '/';

			assert(common_len > *place);
		}

		if (num_matches > 1 && (common_len != *place || !*lastwastab))
			beep();

		/* If the matches have something in common, show that part. */
		if (common_len != *place) {
			buf = charealloc(buf, common_len + buf_len - *place + 1);
			charmove(buf + common_len, buf + *place, buf_len - *place + 1);
			strncpy(buf, mzero, common_len);
			*place = common_len;
		}

		if (!*lastwastab)
			*lastwastab = TRUE;
		else if (num_matches > 1) {
			size_t longest_name = 0, ncols;
			int editline = 0;

			/* Sort the list of available choices. */
			qsort(matches, num_matches, sizeof(char *), diralphasort);

			/* Find the length of the longest among the choices. */
			for (match = 0; match < num_matches; match++) {
				size_t namelen = breadth(matches[match]);

				if (namelen > longest_name)
					longest_name = namelen;
			}

			if (longest_name > COLS - 1)
				longest_name = COLS - 1;

			/* Each column will be (longest_name + 2) columns wide, i.e.
			 * two spaces between columns, except that there will be
			 * only one space after the last column. */
			ncols = (COLS + 1) / (longest_name + 2);

			/* Blank the edit window and hide the cursor. */
			blank_edit();
			curs_set(0);
			wmove(edit, 0, 0);

			/* Now print the list of matches out there. */
			for (match = 0; match < num_matches; match++) {
				char *disp;

				wmove(edit, editline, (longest_name + 2) * (match % ncols));

				if (match % ncols == 0 && editline == editwinrows - 1 &&
						num_matches - match > ncols) {
					waddstr(edit, _("(more)"));
					break;
				}

				disp = display_string(matches[match], 0, longest_name, FALSE, FALSE);
				waddstr(edit, disp);
				free(disp);

				if ((match + 1) % ncols == 0)
					editline++;
			}

			wnoutrefresh(edit);
			*listed = TRUE;
		}

		free(glued);
		free(mzero);
	}

	free_chararray(matches, num_matches);

	/* When we didn't list any matches now, refresh the edit window, just
	 * in case a previous tab showed a list, so we know where we are. */
	if (!*listed)
		refresh_func();

	return buf;
}
#endif /* ENABLE_TABCOMP */
