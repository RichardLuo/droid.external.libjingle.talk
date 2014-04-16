LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../android_common.mk
include $(LOCAL_PATH)/libjing_sources.mk

LOCAL_MODULE := libjingle

LOCAL_CFLAGS := \
	$(JINGLE_CFLAGS) \

LOCAL_C_INCLUDES := \
	$(JINGLE_C_INCLUDES)

LOCAL_SHARED_LIBRARIES += libexpat
LOCAL_SHARED_LIBRARIES += libjsoncpp
LOCAL_SHARED_LIBRARIES += libopenssl
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_LDLIBS += -ldl -lrt -lpthread

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/libjingle_p2p_android.mk
include $(LOCAL_PATH)/libjingle_media_android.mk
include $(LOCAL_PATH)/libjingle_unittest_main_android.mk
