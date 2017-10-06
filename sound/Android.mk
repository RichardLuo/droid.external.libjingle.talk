##################################################################
# author: Richard Luo                      
# date:   2013-03-03 22:20:31
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libjingle_sound
LOCAL_CXX11_DISABLED := yes
LOCAL_CPP_EXTENSION := .cc

LOCAL_CFLAGS := \
	'-DEXPAT_RELATIVE_PATH' \
	'-DFEATURE_ENABLE_VOICEMAIL' \
	'-DGTEST_RELATIVE_PATH' \
	'-DJSONCPP_RELATIVE_PATH' \
	'-DLOGGING=1' \
	'-DLINUX' \
	'-DSRTP_RELATIVE_PATH' \
	'-DFEATURE_ENABLE_SSL' \
	'-DFEATURE_ENABLE_PSTN' \
	'-DHAVE_SRTP' \
	'-DHAVE_WEBRTC_VIDEO' \
	'-DHAVE_WEBRTC_VOICE' \
	'-DUSE_WEBRTC_DEV_BRANCH' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DNO_TCMALLOC' \
	'-DDISABLE_NACL' \
	'-DCHROMIUM_BUILD' \
	'-DUSE_LIBJPEG_TURBO=1' \
	'-DUSE_PROPRIETARY_CODECS' \
	'-DENABLE_PEPPER_THREADING' \
	'-DENABLE_GPU=1' \
	'-DUSE_OPENSSL=1' \
	'-DENABLE_EGLIMAGE=1' \
	'-DUSE_SKIA=1' \
	'-DHASH_NAMESPACE=__gnu_cxx' \
	'-DPOSIX' \
	'-DDISABLE_DYNAMIC_CAST' \
	'-DHAVE_OPENSSL_SSL_H=1' \
	'-D_REENTRANT' \
	'-DXML_STATIC' \
	'-DANDROID' \
	'-D__GNU_SOURCE=1' \
	'-DUSE_STLPORT=1' \
	'-D_STLP_USE_PTR_SPECIALIZATIONS=1' \
	'-DCHROME_BUILD_ID=""' \
	'-DDYNAMIC_ANNOTATIONS_ENABLED=1' \
	'-DWTF_USE_DYNAMIC_ANNOTATIONS=1' \
	'-D_DEBUG'

path_third_party := $(LOCAL_PATH)/../../third_party

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../.. \
	$(path_third_party) \
	$(path_third_party)/webrtc \
	$(path_third_party)/expat/files/lib \
	$(path_third_party)/jsoncpp/overrides/include \
	$(path_third_party)/jsoncpp/source/include

LOCAL_SRC_FILES := \
	nullsoundsystem.cc \
	nullsoundsystemfactory.cc \
	platformsoundsystem.cc \
	platformsoundsystemfactory.cc \
	soundsysteminterface.cc \
	soundsystemproxy.cc \


LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng


LOCAL_SHARED_LIBRARIES += libjingle
LOCAL_SHARED_LIBRARIES += liblog


ifneq ($(TARGET_SIMULATOR),true)
LOCAL_C_INCLUDES += bionic		# very important!
LOCAL_C_INCLUDES += external/stlport/stlport 
LOCAL_C_INCLUDES += external/libjingle/third_party/alsa_headers
LOCAL_SHARED_LIBRARIES += libstlport libdl
LOCAL_CFLAGS += '-D_POSIX_SOURCE'
else
LOCAL_LDLIBS += -lrt -lpthread
endif


LOCAL_SRC_FILES += 	\
	alsasymboltable.cc \
	linuxsoundsystem.cc \
	alsasoundsystem.cc \

include $(BUILD_SHARED_LIBRARY)
