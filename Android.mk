LOCAL_PATH:= $(call my-dir)

# ========================================================
# nano
# ========================================================
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	src/browser.c \
	src/chars.c \
	src/color.c \
	src/cut.c \
	src/files.c \
	src/global.c \
	src/help.c \
	src/history.c \
	src/move.c \
	src/nano.c \
	src/prompt.c \
	src/rcfile.c \
	src/search.c \
	src/text.c \
	src/utils.c \
	src/winio.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	external/libncurses/include

LOCAL_CFLAGS += \
	-DHAVE_CONFIG_H \
	-DLOCALEDIR=\"/data/locale\" \
	-DSYSCONFDIR=\"/system_ext/etc/nano\"

LOCAL_CFLAGS += -Wno-sign-compare -Wno-unused-parameter

LOCAL_SHARED_LIBRARIES += \
	libncurses

LOCAL_MODULE := nano
LOCAL_MODULE_TAGS := optional
LOCAL_SYSTEM_EXT_MODULE := true
include $(BUILD_EXECUTABLE)

# ========================================================
# nano configs
# ========================================================
NANO_ETC := $(TARGET_OUT_SYSTEM_EXT_ETC)/$(LOCAL_MODULE)

syntax_files := $(wildcard $(LOCAL_PATH)/syntax/*.nanorc)
NANO_SYNTAX := $(addprefix $(NANO_ETC)/,$(notdir $(syntax_files)))
$(NANO_SYNTAX): $(NANO_ETC)/%: $(LOCAL_PATH)/syntax/% | $(LOCAL_BUILT_MODULE)
	@echo "Install: $@ -> $(NANO_ETC)"
	@mkdir -p $(dir $@)
	cp $< $@

nanorc_file := $(LOCAL_PATH)/etc/nanorc
NANO_NANORC := $(addprefix $(NANO_ETC)/,$(notdir $(nanorc_file)))
$(NANO_NANORC): $(nanorc_file)
	@echo "Install: $@ -> $(NANO_ETC)"
	@mkdir -p $(dir $@)
	$(hide) cp $< $@

ALL_DEFAULT_INSTALLED_MODULES += $(NANO_SYNTAX) $(NANO_NANORC)

ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(NANO_SYNTAX) \
	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(NANO_NANORC)

# ========================================================
include $(call all-makefiles-under,$(LOCAL_PATH))
