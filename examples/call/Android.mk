##################################################################
# author: Richard Luo                      
# date:   2013-03-03 23:01:41
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../../android_common.mk

LOCAL_MODULE := examples_call

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(path_libjingle) \
	$(path_third_party) \
	$(path_third_party)/webrtc \
	$(path_third_party)/gtest/include

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
	call_main.cc \
	callclient.cc \
	console.cc \
	friendinvitesendtask.cc \
	mediaenginefactory.cc \
	mucinviterecvtask.cc \
	mucinvitesendtask.cc \
	presencepushtask.cc

LOCAL_C_INCLUDES := \
	$(JINGLE_C_INCLUDES) 

LOCAL_CFLAGS := \
	$(JINGLE_CFLAGS) \
	'-D_FILE_OFFSET_BITS=64' \
	'-DCHROMIUM_BUILD' \
	'-DUSE_DEFAULT_RENDER_THEME=1' \
	'-DUSE_LIBJPEG_TURBO=1' \
	'-DUSE_NSS=1' \
	'-DENABLE_ONE_CLICK_SIGNIN' \
	'-DGTK_DISABLE_SINGLE_INCLUDES=1' \
	'-DENABLE_REMOTING=1' \
	'-DENABLE_WEBRTC=1' \
	'-DENABLE_PEPPER_THREADING' \
	'-DENABLE_CONFIGURATION_POLICY' \
	'-DENABLE_INPUT_SPEECH' \
	'-DENABLE_NOTIFICATIONS' \
	'-DENABLE_GPU=1' \
	'-DENABLE_EGLIMAGE=1' \
	'-DUSE_SKIA=1' \
	'-DENABLE_TASK_MANAGER=1' \
	'-DENABLE_WEB_INTENTS=1' \
	'-DENABLE_EXTENSIONS=1' \
	'-DENABLE_PLUGIN_INSTALLATION=1' \
	'-DENABLE_PLUGINS=1' \
	'-DENABLE_SESSION_SERVICE=1' \
	'-DENABLE_THEMES=1' \
	'-DENABLE_BACKGROUND=1' \
	'-DENABLE_AUTOMATION=1' \
	'-DENABLE_GOOGLE_NOW=1' \
	'-DENABLE_LANGUAGE_DETECTION=1' \
	'-DENABLE_PRINTING=1' \
	'-DENABLE_CAPTIVE_PORTAL_DETECTION=1' \
	'-DLINUX' \
	'-DHASH_NAMESPACE=__gnu_cxx' \
	'-DPOSIX' \
	'-DDISABLE_DYNAMIC_CAST' \
	'-DHAVE_OPENSSL_SSL_H=1' \
	'-D_REENTRANT' \
	'-DDYNAMIC_ANNOTATIONS_ENABLED=1' \
	'-DWTF_USE_DYNAMIC_ANNOTATIONS=1' \

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

LOCAL_SHARED_LIBRARIES := \
	libjingle_media_android \
	libjingle \
	libjingle_p2p \
	libutils \

LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += liblog


ifneq ($(TARGET_SIMULATOR),true)
LOCAL_C_INCLUDES += bionic		# very important!
LOCAL_C_INCLUDES += external/stlport/stlport 
LOCAL_SHARED_LIBRARIES += libstlport libdl
else
LOCAL_LDLIBS += -lpthread
endif


include $(BUILD_EXECUTABLE)
