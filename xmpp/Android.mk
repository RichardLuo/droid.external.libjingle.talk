##################################################################
# author: Richard Luo                      
# date:   2013-03-05 17:15:48
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../android_common.mk

LOCAL_MODULE := libjingle_xmpphelp

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
	jingleinfotask.cc

LOCAL_C_INCLUDES += \
	$(JINGLE_C_INCLUDES) \
	external/gtest/include

LOCAL_CFLAGS := \
	$(JINGLE_CFLAGS)

LOCAL_SHARED_LIBRARIES += libjingle
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)
