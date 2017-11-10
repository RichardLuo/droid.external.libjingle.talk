###################################################################
## author: Richard Luo                      
## date:   2013-03-06 20:59:16
##                                                                
###################################################################

#LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#include $(LOCAL_PATH)/../../../android_common.mk

#LOCAL_MODULE := libjingleplus

#LOCAL_CPP_EXTENSION := .cc

#LOCAL_SRC_FILES := \
	#libjingleplus.cc \
	#presencepushtask.cc \
	#rostertask.cc


#LOCAL_C_INCLUDES := \
	#$(path_libjingle) \
	#$(path_third_party) \
	#$(path_third_party)/webrtc \
	#$(path_external_gtest)/include

#LOCAL_CFLAGS += \
	#$(JINGLE_CFLAGS)

#LOCAL_PRELINK_MODULE := false

#LOCAL_MODULE_TAGS := eng

#LOCAL_SHARED_LIBRARIES := \
	#libjingle_media_android \
	#libjingle \
	#libjingle_p2p \
	#libutils \

#include $(BUILD_SHARED_LIBRARY)

