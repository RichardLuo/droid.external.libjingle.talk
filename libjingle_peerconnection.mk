##################################################################
# author: Richard Luo                      
# date:   2013-03-03 22:30:06
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libjingle_peerconnection


path_third_party := $(LOCAL_PATH)/../third_party
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(path_third_party) \
	$(path_third_party)/webrtc \
	$(path_third_party)/expat/files/lib \
	$(path_third_party)/jsoncpp/overrides/include \
	$(path_third_party)/jsoncpp/source/include \
	$(path_third_party)/gtest/include

LOCAL_SRC_FILES := \
	app/webrtc/audiotrack.cc \
	app/webrtc/datachannel.cc \
	app/webrtc/dtmfsender.cc \
	app/webrtc/jsepicecandidate.cc \
	app/webrtc/jsepsessiondescription.cc \
	app/webrtc/localaudiosource.cc \
	app/webrtc/localvideosource.cc \
	app/webrtc/mediastream.cc \
	app/webrtc/mediastreamhandler.cc \
	app/webrtc/mediastreamproxy.cc \
	app/webrtc/mediastreamsignaling.cc \
	app/webrtc/mediastreamtrackproxy.cc \
	app/webrtc/peerconnection.cc \
	app/webrtc/peerconnectionfactory.cc \
	app/webrtc/peerconnectionproxy.cc \
	app/webrtc/portallocatorfactory.cc \
	app/webrtc/statscollector.cc \
	app/webrtc/videosourceproxy.cc \
	app/webrtc/videotrack.cc \
	app/webrtc/videotrackrenderers.cc \
	app/webrtc/webrtcsdp.cc \
	app/webrtc/webrtcsession.cc


LOCAL_SHARED_LIBRARIES := libutils 
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := eng


include $(BUILD_SHARED_LIBRARY)
