###################################################################
## author: Richard Luo                      
## date:   2013-03-03 23:01:41
##                                                                
###################################################################

#LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#include $(LOCAL_PATH)/../../../../android_common.mk

#LOCAL_MODULE := peerconnection_server

#LOCAL_C_INCLUDES := \
	#$(LOCAL_PATH)/common \
	#$(path_libjingle) \

#LOCAL_CPP_EXTENSION := .cc

#LOCAL_SRC_FILES := \
		#main.cc \

#LOCAL_CFLAGS := \
	#$(JINGLE_CFLAGS)

#LOCAL_PRELINK_MODULE := false

#LOCAL_MODULE_TAGS := eng

#LOCAL_SHARED_LIBRARIES := \
	#libpeerconnection_server \
	#libjingle_media_android  \
	#libjingle_peerconnection \
	#libjingle \
	#libjingle_p2p \
	#libutils \
	#libcutils \
	#libbinder \
	#liblog \

#LOCAL_LDLIBS += -lpthread

#include $(BUILD_EXECUTABLE)


#include $(CLEAR_VARS)
#include $(LOCAL_PATH)/../../../../android_common.mk

#LOCAL_MODULE := echo_tcp_server

#LOCAL_C_INCLUDES := \
	#$(LOCAL_PATH)/common \
	#$(path_libjingle) \

#LOCAL_SRC_FILES := \
	#echo_tcp_server.cpp

#LOCAL_CFLAGS := \
	#$(JINGLE_CFLAGS) \
	#'-D_FILE_OFFSET_BITS=64' \
	#'-DCHROMIUM_BUILD' \
	#'-DUSE_DEFAULT_RENDER_THEME=1' \
	#'-DUSE_LIBJPEG_TURBO=1' \
	#'-DUSE_NSS=1' \
	#'-DENABLE_ONE_CLICK_SIGNIN' \
	#'-DGTK_DISABLE_SINGLE_INCLUDES=1' \
	#'-DENABLE_REMOTING=1' \
	#'-DENABLE_WEBRTC=1' \
	#'-DENABLE_PEPPER_THREADING' \
	#'-DENABLE_CONFIGURATION_POLICY' \
	#'-DENABLE_INPUT_SPEECH' \
	#'-DENABLE_NOTIFICATIONS' \
	#'-DENABLE_GPU=1' \
	#'-DENABLE_EGLIMAGE=1' \
	#'-DUSE_SKIA=1' \
	#'-DENABLE_TASK_MANAGER=1' \
	#'-DENABLE_WEB_INTENTS=1' \
	#'-DENABLE_EXTENSIONS=1' \
	#'-DENABLE_PLUGIN_INSTALLATION=1' \
	#'-DENABLE_PLUGINS=1' \
	#'-DENABLE_SESSION_SERVICE=1' \
	#'-DENABLE_THEMES=1' \
	#'-DENABLE_BACKGROUND=1' \
	#'-DENABLE_AUTOMATION=1' \
	#'-DENABLE_GOOGLE_NOW=1' \
	#'-DENABLE_LANGUAGE_DETECTION=1' \
	#'-DENABLE_PRINTING=1' \
	#'-DENABLE_CAPTIVE_PORTAL_DETECTION=1' \
	#'-DLINUX' \
	#'-DHASH_NAMESPACE=__gnu_cxx' \
	#'-DPOSIX' \
	#'-DDISABLE_DYNAMIC_CAST' \
	#'-DHAVE_OPENSSL_SSL_H=1' \
	#'-D_REENTRANT' \
	#'-DDYNAMIC_ANNOTATIONS_ENABLED=1' \
	#'-DWTF_USE_DYNAMIC_ANNOTATIONS=1' \

#LOCAL_MODULE_TAGS := eng

#LOCAL_PRELINK_MODULE := false

#LOCAL_SHARED_LIBRARIES := \
	#libpeerconnection_server \
	#libjingle_media_android  \
	#libjingle_peerconnection \
	#libjingle \
	#libjingle_p2p \
	#libutils \
	#libcutils \

#LOCAL_LDLIBS += -lpthread

#include $(BUILD_EXECUTABLE)
