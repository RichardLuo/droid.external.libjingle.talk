##################################################################
# author: Richard Luo                      
# date:   2013-03-06 20:56:29
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../../../android_common.mk

LOCAL_MODULE := libjingleplus_main

LOCAL_CPP_EXTENSION := cc

LOCAL_SRC_FILES := libjingleplus_main.cc

LOCAL_C_INCLUDES += $(JINGLE_C_INCLUDES)

LOCAL_CFLAGS += $(JINGLE_CFLAGS)

LOCAL_SHARED_LIBRARIES := libutils 
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := eng

LOCAL_SHARED_LIBRARIES := \
	libjingle_media_android \
	libjingle \
	libjingle_p2p \
	libutils \


include $(BUILD_EXECUTABLE)

