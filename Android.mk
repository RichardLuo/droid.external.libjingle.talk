LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../android_common.mk
include $(LOCAL_PATH)/libjing_sources.mk

LOCAL_MODULE := libjingle

LOCAL_CFLAGS := \
	$(JINGLE_CFLAGS) \

LOCAL_C_INCLUDES := \
	external/cppformat \
	external/expat/lib \
	$(JINGLE_C_INCLUDES) \
	$(path_openssl)/include


LOCAL_SHARED_LIBRARIES += libexpat
LOCAL_SHARED_LIBRARIES += libjsoncpp
LOCAL_SHARED_LIBRARIES += libcrypto
LOCAL_SHARED_LIBRARIES += libssl
LOCAL_SHARED_LIBRARIES += libeasyutils
LOCAL_SHARED_LIBRARIES += libcppformat
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog

ifneq ($(TARGET_SIMULATOR),true)
LOCAL_C_INCLUDES += external/stlport/stlport
LOCAL_C_INCLUDES += bionic		# very important!
LOCAL_SHARED_LIBRARIES += libstlport libdl
endif

LOCAL_LDLIBS += -ldl -lrt -lpthread

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)

#include $(LOCAL_PATH)/libjingle_p2p_android.mk
#include $(LOCAL_PATH)/libjingle_media_android.mk

ifeq ($(TARGET_SIMULATOR),true)
include $(LOCAL_PATH)/libjingle_unittest_main_android.mk
endif


