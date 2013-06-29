##################################################################
# author: Richard Luo                      
# date:   2013-03-03 16:04:58
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libjingle_media_android

LOCAL_SRC_FILES := \

LOCAL_C_INCLUDES += $(LOCAL_PATH)/..
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../third_party/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../third_party/webrtc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../third_party/libyuv/include

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES += media/base/capturemanager.cc
LOCAL_SRC_FILES += media/base/capturerenderadapter.cc
LOCAL_SRC_FILES += media/base/codec.cc
LOCAL_SRC_FILES += media/base/constants.cc
LOCAL_SRC_FILES += media/base/cpuid.cc
LOCAL_SRC_FILES += media/base/filemediaengine.cc
LOCAL_SRC_FILES += media/base/hybridvideoengine.cc
LOCAL_SRC_FILES += media/base/mediaengine.cc
LOCAL_SRC_FILES += media/base/rtpdataengine.cc
LOCAL_SRC_FILES += media/base/rtpdump.cc
LOCAL_SRC_FILES += media/base/rtputils.cc
LOCAL_SRC_FILES += media/base/streamparams.cc
LOCAL_SRC_FILES += media/base/videoadapter.cc
LOCAL_SRC_FILES += media/base/videocapturer.cc
LOCAL_SRC_FILES += media/base/videocommon.cc
LOCAL_SRC_FILES += media/base/videoframe.cc
LOCAL_SRC_FILES += media/devices/devicemanager.cc
LOCAL_SRC_FILES += media/devices/filevideocapturer.cc
LOCAL_SRC_FILES += media/webrtc/webrtcpassthroughrender.cc

# LOCAL_SRC_FILES += media/webrtc/webrtcvideocapturer.cc
LOCAL_SRC_FILES += media/webrtc/webrtcvideoengine.cc

LOCAL_SRC_FILES += media/webrtc/webrtcvideoframe.cc
LOCAL_SRC_FILES += media/webrtc/webrtcvideoframe.h

LOCAL_SRC_FILES += media/webrtc/webrtcvoiceengine.cc
LOCAL_SRC_FILES += media/webrtc/webrtcvoiceengine.h

# LOCAL_SRC_FILES += media/devices/gtkvideorenderer.cc
LOCAL_SRC_FILES += media/devices/libudevsymboltable.cc
LOCAL_SRC_FILES += media/devices/linuxdeviceinfo.cc
LOCAL_SRC_FILES += media/devices/linuxdevicemanager.cc
LOCAL_SRC_FILES += media/devices/v4llookup.cc

LOCAL_CFLAGS += \
	'-DEXPAT_RELATIVE_PATH' \
	'-DFEATURE_ENABLE_VOICEMAIL' \
	'-DGTEST_RELATIVE_PATH' \
	'-DJSONCPP_RELATIVE_PATH' \
	'-DLOGGING=1' \
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
	'-DHAVE_SYS_UIO_H' \
	'-DDYNAMIC_ANNOTATIONS_ENABLED=1' \
	'-DWTF_USE_DYNAMIC_ANNOTATIONS=1' \
	'-D_DEBUG' '-DLINUX' \


LOCAL_SHARED_LIBRARIES += libjingle
LOCAL_SHARED_LIBRARIES += libwebrtc_common_video
LOCAL_SHARED_LIBRARIES += libwebrtc_video_render
LOCAL_SHARED_LIBRARIES += libwebrtc_system_wrappers
LOCAL_SHARED_LIBRARIES += libwebrtc_utility
LOCAL_SHARED_LIBRARIES += libwebrtc_vie_core
LOCAL_SHARED_LIBRARIES += libyuv_shared
LOCAL_SHARED_LIBRARIES += libwebrtc_video_capture
LOCAL_SHARED_LIBRARIES += libwebrtc_voe_core
LOCAL_SHARED_LIBRARIES += libjingle_sound


LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

LOCAL_LDLIBS += -lrt -lpthread


include $(BUILD_SHARED_LIBRARY)

