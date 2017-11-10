##################################################################
# author: Richard Luo                      
# date:   2013-03-03 15:23:09
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/../android_common.mk

LOCAL_MODULE := libjingle_p2p

LOCAL_CPP_EXTENSION := .cc

LOCAL_C_INCLUDES +=  \
	$(JINGLE_C_INCLUDES) \
	external/gtest/include						\
	$(path_third_party)/libsrtp/srtp/include	\
	$(path_libsrtp)/srtp/crypto/include			\
	$(path_libsrtp)/config 

LOCAL_CFLAGS += \
	$(JINGLE_CFLAGS) \
	'-DHAVE_STDLIB_H' \
	'-DHAVE_STRING_H' \
	'-D_FILE_OFFSET_BITS=64' \
	'-DNO_TCMALLOC' \
	'-DDISABLE_NACL' \
	'-DCPU_CISC' \
	'-DHAVE_INT16_T' \
	'-DHAVE_INT32_T' \
	'-DHAVE_INT8_T' \
	'-DHAVE_UINT16_T' \
	'-DHAVE_UINT32_T' \
	'-DHAVE_UINT64_T' \
	'-DHAVE_UINT8_T' \
	'-DSIZEOF_UNSIGNED_LONG=4' \
	'-DSIZEOF_UNSIGNED_LONG_LONG=8' \
	'-DHAVE_STDINT_H' \
	'-DHAVE_INTTYPES_H' \
	'-DHAVE_NETINET_IN_H' \
	'-DINLINE=inline' \
	'-DCHROMIUM_BUILD' \
	'-DUSE_LIBJPEG_TURBO=1' \
	'-DUSE_PROPRIETARY_CODECS' \
	'-DENABLE_PEPPER_THREADING' \
	'-DENABLE_GPU=1' \
	'-DUSE_OPENSSL=1' \
	'-DENABLE_EGLIMAGE=1' \
	'-DUSE_SKIA=1' \
	'-DANDROID' \
	'-D__GNU_SOURCE=1' \
	'-DUSE_STLPORT=1' \
	'-D_STLP_USE_PTR_SPECIALIZATIONS=1' \
	'-DCHROME_BUILD_ID=""' \
	'-DDYNAMIC_ANNOTATIONS_ENABLED=1' \
	'-DWTF_USE_DYNAMIC_ANNOTATIONS=1'

LOCAL_SRC_FILES += \
        p2p/base/constants.cc					\
        p2p/base/dtlstransportchannel.cc		\
        p2p/base/p2ptransport.cc				\
        p2p/base/p2ptransportchannel.cc			\
        p2p/base/parsing.cc						\
        p2p/base/port.cc						\
        p2p/base/portallocator.cc				\
        p2p/base/portallocatorsessionproxy.cc	\
        p2p/base/portproxy.cc					\
        p2p/base/pseudotcp.cc					\
        p2p/base/relayport.cc					\
        p2p/base/relayserver.cc					\
        p2p/base/rawtransport.cc				\
        p2p/base/rawtransportchannel.cc			\
        p2p/base/session.cc						\
        p2p/base/sessiondescription.cc			\
        p2p/base/sessionmanager.cc				\
        p2p/base/sessionmessages.cc				\
        p2p/base/stun.cc						\
        p2p/base/stunport.cc					\
        p2p/base/stunrequest.cc					\
        p2p/base/stunserver.cc					\
        p2p/base/tcpport.cc						\
        p2p/base/transport.cc					\
        p2p/base/transportchannel.cc			\
        p2p/base/transportchannelproxy.cc		\
        p2p/base/transportdescriptionfactory.cc	\
        p2p/base/transportdescriptionfactory.h	\
        p2p/base/turnport.cc					\
        p2p/base/turnserver.cc					\
        p2p/client/basicportallocator.cc		\
        p2p/client/connectivitychecker.cc		\
        p2p/client/httpportallocator.cc			\
        p2p/client/socketmonitor.cc				\
        session/tunnel/pseudotcpchannel.cc		\
        session/tunnel/tunnelsessionclient.cc	\
        session/tunnel/securetunnelsessionclient.cc		\
        session/media/channel.cc						\
        session/media/audiomonitor.cc					\
        session/media/call.cc							\
        session/media/channelmanager.cc					\
        session/media/currentspeakermonitor.cc			\
        session/media/mediamessages.cc					\
        session/media/mediamonitor.cc					\
        session/media/mediarecorder.cc					\
        session/media/mediasession.cc					\
        session/media/mediasessionclient.cc				\
        session/media/rtcpmuxfilter.cc					\
        session/media/soundclip.cc						\


ifneq ($(TARGET_SIMULATOR),true)
LOCAL_C_INCLUDES += bionic		# very important!
LOCAL_C_INCLUDES += external/stlport/stlport 
LOCAL_C_INCLUDES += $(path_external_gtest)/include
LOCAL_SHARED_LIBRARIES += libstlport libdl
endif

LOCAL_SRC_FILES += session/media/srtpfilter.cc
LOCAL_SRC_FILES += session/media/ssrcmuxfilter.cc
LOCAL_SRC_FILES += session/media/typingmonitor.cc


LOCAL_SHARED_LIBRARIES += libjingle
LOCAL_SHARED_LIBRARIES += libjingle_media_android
LOCAL_SHARED_LIBRARIES += libwebrtc_common_video
LOCAL_SHARED_LIBRARIES += libwebrtc_system_wrappers
LOCAL_SHARED_LIBRARIES += libyuv_shared

LOCAL_SHARED_LIBRARIES += libsrtp
LOCAL_SHARED_LIBRARIES += libexpat
LOCAL_SHARED_LIBRARIES += libjsoncpp
LOCAL_SHARED_LIBRARIES += libssl
LOCAL_SHARED_LIBRARIES += liblog

LOCAL_LDLIBS += -lrt -lpthread

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

include $(BUILD_SHARED_LIBRARY)
