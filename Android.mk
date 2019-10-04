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
	external/libncurses/include \
	external/openssh/openbsd-compat

LOCAL_CFLAGS += \
	-DHAVE_CONFIG_H \
	-DLOCALEDIR=\"/data/locale\" \
	-DSYSCONFDIR=\"/system/etc/nano\"

LOCAL_CFLAGS += -Wno-sign-compare -Wno-unused-parameter

LOCAL_SHARED_LIBRARIES += \
	libncurses \
	libssh

LOCAL_MODULE := nano
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

# ========================================================
# nano configs
# ========================================================
NANO_ETC := $(TARGET_OUT_ETC)/$(LOCAL_MODULE)

syntax_files := $(wildcard $(LOCAL_PATH)/syntax/*.nanorc)
NANO_SYNTAX := $(addprefix $(LOCAL_PATH)/syntax/,$(notdir $(syntax_files)))
$(NANO_SYNTAX): $(LOCAL_INSTALLED_MODULE)
	@echo "Install: $@ -> $(NANO_ETC)"
	@mkdir -p $(NANO_ETC)
	$(hide) cp $@ $(NANO_ETC)

nanorc_file := $(LOCAL_PATH)/etc/nanorc
NANO_NANORC := $(addprefix $(LOCAL_PATH)/etc/,$(notdir $(nanorc_file)))
$(NANO_NANORC): $(LOCAL_INSTALLED_MODULE)
	@echo "Install: $@ -> $(NANO_ETC)"
	@mkdir -p $(NANO_ETC)
	$(hide) cp $@ $(NANO_ETC)

ALL_DEFAULT_INSTALLED_MODULES += $(NANO_SYNTAX) $(NANO_NANORC)

ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(NANO_SYNTAX) \
	$(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(NANO_NANORC)

# ========================================================
include $(call all-makefiles-under,$(LOCAL_PATH))
