##################################################################
# author: Richard Luo                      
# date:   2013-03-03 23:01:41
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../../../android_common.mk

LOCAL_MODULE := peerconnection_server

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/common \
	$(path_libjingle) \

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
		main.cc \

LOCAL_CFLAGS := \
	$(JINGLE_CFLAGS)

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := eng

LOCAL_SHARED_LIBRARIES := \
	libpeerconnection_server \
	libjingle_media_android  \
	libjingle_peerconnection \
	libjingle \
	libjingle_p2p \
	libutils \
	libcutils \
	libbinder \
	liblog \

LOCAL_LDLIBS += -lpthread

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../../../android_common.mk

LOCAL_MODULE := echo_tcp_server

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/common \
	$(path_libjingle) \

LOCAL_SRC_FILES := \
	echo_tcp_server.cpp

LOCAL_CFLAGS := \
	$(JINGLE_CFLAGS)

LOCAL_MODULE_TAGS := eng

LOCAL_PRELINK_MODULE := false

LOCAL_SHARED_LIBRARIES := \
	libpeerconnection_server \
	libjingle_media_android  \
	libjingle_peerconnection \
	libjingle \
	libjingle_p2p \
	libutils \
	libcutils \
	libbinder \
	liblog \

LOCAL_LDLIBS += -lpthread

include $(BUILD_EXECUTABLE)
