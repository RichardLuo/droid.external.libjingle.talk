##################################################################
# author: Richard Luo                      
# date:   2013-03-10 17:21:20
#                                                                
##################################################################

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../android_common.mk

LOCAL_MODULE := android_libjingle_unittest_main

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
	base/unittest_main.cc \
	base/asynchttprequest_unittest.cc \
	base/atomicops_unittest.cc \
	base/autodetectproxy_unittest.cc \
	base/bandwidthsmoother_unittest.cc \
	base/base64_unittest.cc \
	base/basictypes_unittest.cc \
	base/buffer_unittest.cc \
	base/bytebuffer_unittest.cc \
	base/byteorder_unittest.cc \
	base/cpumonitor_unittest.cc \
	base/crc32_unittest.cc \
	base/event_unittest.cc \
	base/filelock_unittest.cc \
	base/fileutils_unittest.cc \
	base/helpers_unittest.cc \
	base/host_unittest.cc \
	base/httpbase_unittest.cc \
	base/httpcommon_unittest.cc \
	base/httpserver_unittest.cc \
	base/ipaddress_unittest.cc \
	base/logging_unittest.cc \
	base/md5digest_unittest.cc \
	base/messagedigest_unittest.cc \
	base/messagequeue_unittest.cc \
	base/multipart_unittest.cc \
	base/nat_unittest.cc \
	base/network_unittest.cc \
	base/nullsocketserver_unittest.cc \
	base/optionsfile_unittest.cc \
	base/pathutils_unittest.cc \
	base/physicalsocketserver_unittest.cc \
	base/proxy_unittest.cc \
	base/proxydetect_unittest.cc \
	base/ratelimiter_unittest.cc \
	base/ratetracker_unittest.cc \
	base/referencecountedsingletonfactory_unittest.cc \
	base/rollingaccumulator_unittest.cc \
	base/sha1digest_unittest.cc \
	base/sharedexclusivelock_unittest.cc \
	base/signalthread_unittest.cc \
	base/sigslot_unittest.cc \
	base/socket_unittest.cc \
	base/socketaddress_unittest.cc \
	base/stream_unittest.cc \
	base/stringencode_unittest.cc \
	base/stringutils_unittest.cc \
	base/task_unittest.cc \
	base/testclient_unittest.cc \
	base/thread_unittest.cc \
	base/timeutils_unittest.cc \
	base/urlencode_unittest.cc \
	base/versionparsing_unittest.cc \
	base/virtualsocket_unittest.cc \
	base/json_unittest.cc \
	xmllite/qname_unittest.cc \
	xmllite/xmlbuilder_unittest.cc \
	xmllite/xmlelement_unittest.cc \
	xmllite/xmlnsstack_unittest.cc \
	xmllite/xmlparser_unittest.cc \
	xmllite/xmlprinter_unittest.cc \
	xmpp/hangoutpubsubclient_unittest.cc \
	xmpp/jid_unittest.cc \
	xmpp/mucroomconfigtask_unittest.cc \
	xmpp/mucroomdiscoverytask_unittest.cc \
	xmpp/mucroomlookuptask_unittest.cc \
	xmpp/mucroomuniquehangoutidtask_unittest.cc \
	xmpp/pingtask_unittest.cc \
	xmpp/pubsubclient_unittest.cc \
	xmpp/pubsubtasks_unittest.cc \
	xmpp/util_unittest.cc \
	xmpp/xmppengine_unittest.cc \
	xmpp/xmpplogintask_unittest.cc \
	xmpp/xmpplocalengine_unittest.cc \
	xmpp/xmppstanzaparser_unittest.cc \
	base/sslidentity_unittest.cc \
	base/sslstreamadapter_unittest.cc


LOCAL_C_INCLUDES := \
	$(JINGLE_C_INCLUDES) 	\
	$(path_third_party)/gtest/include

LOCAL_CFLAGS += $(JINGLE_CFLAGS)

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := eng

LOCAL_STATIC_LIBRARIES := \
	libgtest

LOCAL_SHARED_LIBRARIES := \
	libjingle_media_android \
	libjingle \
	libjingle_p2p \
	libjsoncpp \
	libutils \


ifneq ($(TARGET_SIMULATOR),true)
LOCAL_C_INCLUDES += bionic		# very important!
LOCAL_C_INCLUDES += external/stlport/stlport 
LOCAL_SHARED_LIBRARIES += libstlport libdl
endif

LOCAL_LDLIBS += -lrt -lpthread


include $(BUILD_EXECUTABLE)
