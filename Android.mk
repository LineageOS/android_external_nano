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

include $(call all-makefiles-under,$(LOCAL_PATH))
