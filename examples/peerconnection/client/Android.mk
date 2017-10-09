##################################################################
# author: Richard Luo                      
# date:   2013-03-03 23:01:41
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../../../../android_common.mk

LOCAL_MODULE := peerconnection_client

LOCAL_C_INCLUDES := \
	/usr/include/gtk-2.0 \
	/usr/include/glib-2.0 \
	/usr/lib/i386-linux-gnu/glib-2.0/include \
	/usr/include/cairo \
	/usr/include/pango-1.0 \
	/usr/lib/i386-linux-gnu/gtk-2.0/include \
	/usr/include/gdk-pixbuf-2.0 \
	/usr/include/atk-1.0 \
	$(LOCAL_PATH) \
	$(path_libjingle) \
	$(path_third_party) \
	$(path_third_party)/webrtc \
	$(path_external_gtest)/include \
	$(path_third_party)/jsoncpp/source/include

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
		conductor.cc \
		defaults.cc \
		linux/main.cc \
		linux/main_wnd.cc \
		peer_connection_client.cc \

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

LOCAL_STATIC_LIBRARIES := \
	libgtest

LOCAL_SHARED_LIBRARIES := \
	libjingle_media_android \
	libjingle_peerconnection \
	libjingle \
	libjingle_p2p \
	libutils \
	libcutils \
	libbinder \
	liblog \

LOCAL_LDLIBS += -lpthread
LOCAL_LDLIBS += -ljsoncpp
LOCAL_LDLIBS += -lgtk-x11-2.0 
LOCAL_LDLIBS += -lgdk-x11-2.0 
LOCAL_LDLIBS += -latk-1.0 
LOCAL_LDLIBS += -lgio-2.0 
LOCAL_LDLIBS += -lpangoft2-1.0 
LOCAL_LDLIBS += -lpangocairo-1.0 
LOCAL_LDLIBS += -lgdk_pixbuf-2.0 
LOCAL_LDLIBS += -lcairo 
LOCAL_LDLIBS += -lpango-1.0 
LOCAL_LDLIBS += -lfreetype 
LOCAL_LDLIBS += -lfontconfig 
LOCAL_LDLIBS += -lgobject-2.0 
LOCAL_LDLIBS += -lglib-2.0 
LOCAL_LDLIBS += -pthread -lgthread-2.0 -lrt -lglib-2.0  

include $(BUILD_EXECUTABLE)
