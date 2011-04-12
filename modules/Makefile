#
# OMNeT++/OMNEST Makefile for libmiximmodules
#
# This file was generated with the command:
#  opp_makemake -f --deep --make-so -o miximmodules -O out -I../base/modules -I../base/messages -I../base/phyLayer -I../base/connectionManager -I../base/utils -L../out/$(CONFIGNAME)/tests/testUtils -L../out/$(CONFIGNAME)/base -L../out/$(CONFIGNAME)/tests/power/utils -lmiximbase
#

# Name of target to be created (-o option)
TARGET = libmiximmodules$(SHARED_LIB_SUFFIX)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I../base/modules \
    -I../base/messages \
    -I../base/phyLayer \
    -I../base/connectionManager \
    -I../base/utils \
    -I. \
    -IanalogueModel \
    -Iapplication \
    -IconnectionManager \
    -Imac \
    -Imessages \
    -Imobility \
    -Inetw \
    -Inic \
    -Inode \
    -Iphy \
    -Ipower \
    -Ipower/battery \
    -Itransport \
    -Iutility

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L../out/$(CONFIGNAME)/tests/testUtils -L../out/$(CONFIGNAME)/base -L../out/$(CONFIGNAME)/tests/power/utils  -lmiximbase
LIBS += -Wl,-rpath,`abspath ../out/$(CONFIGNAME)/tests/testUtils` -Wl,-rpath,`abspath ../out/$(CONFIGNAME)/base` -Wl,-rpath,`abspath ../out/$(CONFIGNAME)/tests/power/utils`

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = modules
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/analogueModel/JakesFading.o \
    $O/analogueModel/LogNormalShadowing.o \
    $O/analogueModel/UWBIRIEEE802154APathlossModel.o \
    $O/analogueModel/SimplePathlossModel.o \
    $O/analogueModel/UWBIRStochasticPathlossModel.o \
    $O/application/SensorApplLayer.o \
    $O/application/TrafficGen.o \
    $O/application/BurstApplLayer.o \
    $O/application/TestApplication.o \
    $O/application/BurstApplLayerBattery.o \
    $O/connectionManager/UnitDisk.o \
    $O/mac/Mac80211.o \
    $O/mac/csma.o \
    $O/mac/CSMA802154.o \
    $O/mac/CSMAMacLayer.o \
    $O/mac/UWBIRMac.o \
    $O/mac/AlohaMacLayer.o \
    $O/messages/UWBIRMacPkt.o \
    $O/messages/AggrPkt.o \
    $O/mobility/MassMobility.o \
    $O/mobility/TractorMobility.o \
    $O/mobility/TurtleMobility.o \
    $O/mobility/BonnMotionMobility.o \
    $O/mobility/ANSimMobility.o \
    $O/mobility/RectangleMobility.o \
    $O/mobility/BonnMotionFileCache.o \
    $O/mobility/ConstSpeedMobility.o \
    $O/mobility/LinearMobility.o \
    $O/mobility/CircleMobility.o \
    $O/mobility/LineSegmentsMobilityBase.o \
    $O/netw/DummyRoute.o \
    $O/netw/ArpHost.o \
    $O/phy/DeciderUWBIRED.o \
    $O/phy/PhyLayer.o \
    $O/phy/PhyLayerBattery.o \
    $O/phy/Decider802154Narrow.o \
    $O/phy/PhyLayerUWBIR.o \
    $O/phy/SNRThresholdDecider.o \
    $O/phy/Decider80211.o \
    $O/phy/Decider80211Battery.o \
    $O/phy/DeciderUWBIREDSync.o \
    $O/phy/DeciderUWBIREDSyncOnAddress.o \
    $O/power/battery/BatteryStats.o \
    $O/power/battery/SimpleBattery.o \
    $O/transport/Aggregation.o \
    $O/utility/IEEE802154A.o \
    $O/utility/phyPER.o \
    $O/utility/SimTracer.o \
    $O/messages/AggrPkt_m.o \
    $O/messages/DummyRoutePkt_m.o \
    $O/messages/Mac80211Pkt_m.o \
    $O/messages/AirFrameUWBIR_m.o \
    $O/messages/UWBIRMacPkt_m.o

# Message files
MSGFILES = \
    messages/AggrPkt.msg \
    messages/DummyRoutePkt.msg \
    messages/Mac80211Pkt.msg \
    messages/AirFrameUWBIR.msg \
    messages/UWBIRMacPkt.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET)

$(TARGET) : $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(SHLIB_LD) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(LIBS) $(OMNETPP_LIBS) $(LDFLAGS)
	$(SHLIB_POSTPROCESS) $O/$(TARGET)

.PHONY:

.SUFFIXES: .cc

$O/%.o: %.cc
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f miximmodules miximmodules.exe libmiximmodules.so libmiximmodules.a libmiximmodules.dll libmiximmodules.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f analogueModel/*_m.cc analogueModel/*_m.h
	-rm -f application/*_m.cc application/*_m.h
	-rm -f connectionManager/*_m.cc connectionManager/*_m.h
	-rm -f mac/*_m.cc mac/*_m.h
	-rm -f messages/*_m.cc messages/*_m.h
	-rm -f mobility/*_m.cc mobility/*_m.h
	-rm -f netw/*_m.cc netw/*_m.h
	-rm -f nic/*_m.cc nic/*_m.h
	-rm -f node/*_m.cc node/*_m.h
	-rm -f phy/*_m.cc phy/*_m.h
	-rm -f power/*_m.cc power/*_m.h
	-rm -f power/battery/*_m.cc power/battery/*_m.h
	-rm -f transport/*_m.cc transport/*_m.h
	-rm -f utility/*_m.cc utility/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc analogueModel/*.cc application/*.cc connectionManager/*.cc mac/*.cc messages/*.cc mobility/*.cc netw/*.cc nic/*.cc node/*.cc phy/*.cc power/*.cc power/battery/*.cc transport/*.cc utility/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/analogueModel/UWBIRIEEE802154APathlossModel.o: analogueModel/UWBIRIEEE802154APathlossModel.cc \
	../base/phyLayer/AnalogueModel.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/Interpolation.h \
	utility/SimpleTimeConstMapping.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	../base/utils/Move.h \
	utility/IEEE802154A.h \
	../base/utils/HostState.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/utils/FWMath.h
$O/analogueModel/JakesFading.o: analogueModel/JakesFading.cc \
	../base/phyLayer/AnalogueModel.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	analogueModel/JakesFading.h \
	../base/utils/HostState.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/utils/FWMath.h
$O/analogueModel/LogNormalShadowing.o: analogueModel/LogNormalShadowing.cc \
	../base/phyLayer/AnalogueModel.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	../base/utils/HostState.h \
	analogueModel/LogNormalShadowing.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/utils/FWMath.h
$O/analogueModel/UWBIRStochasticPathlossModel.o: analogueModel/UWBIRStochasticPathlossModel.cc \
	../base/phyLayer/AnalogueModel.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/MappingBase.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	analogueModel/UWBIRStochasticPathlossModel.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	../base/utils/HostState.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/utils/FWMath.h
$O/analogueModel/SimplePathlossModel.o: analogueModel/SimplePathlossModel.cc \
	../base/phyLayer/AnalogueModel.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	../base/utils/HostState.h \
	analogueModel/SimplePathlossModel.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/utils/FWMath.h
$O/application/BurstApplLayer.o: application/BurstApplLayer.cc \
	../base/utils/ImNotifiable.h \
	../base/messages/ApplPkt_m.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/modules/TestApplLayer.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	application/BurstApplLayer.h \
	../base/utils/HostState.h \
	../base/modules/BaseApplLayer.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/utils/FWMath.h
$O/application/SensorApplLayer.o: application/SensorApplLayer.cc \
	../base/utils/ImNotifiable.h \
	../base/messages/ApplPkt_m.h \
	../base/modules/Blackboard.h \
	application/SensorApplLayer.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	utility/Packet.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseApplLayer.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/utils/FWMath.h \
	../base/utils/NetwControlInfo.h
$O/application/BurstApplLayerBattery.o: application/BurstApplLayerBattery.cc \
	application/BurstApplLayerBattery.h \
	../base/utils/ImNotifiable.h \
	../base/messages/ApplPkt_m.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/modules/TestApplLayer.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	application/BurstApplLayer.h \
	../base/utils/HostState.h \
	../base/modules/BaseApplLayer.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/utils/FWMath.h
$O/application/TrafficGen.o: application/TrafficGen.cc \
	../base/utils/ImNotifiable.h \
	../base/messages/ApplPkt_m.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/modules/BaseNetwLayer.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	utility/Packet.h \
	../base/utils/HostState.h \
	../base/modules/BaseApplLayer.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/messages/NetwPkt_m.h \
	../base/utils/SimpleAddress.h \
	../base/utils/FWMath.h \
	application/TrafficGen.h \
	../base/utils/NetwControlInfo.h \
	../base/modules/ArpInterface.h
$O/application/TestApplication.o: application/TestApplication.cc \
	../base/modules/Blackboard.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/ChannelInfo.h \
	../base/messages/ApplPkt_m.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/utils/NetwControlInfo.h \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/BaseUtility.h \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/phyLayer/Decider.h \
	phy/RadioUWBIR.h \
	../base/connectionManager/BaseConnectionManager.h \
	utility/SimpleTimeConstMapping.h \
	../base/phyLayer/Interpolation.h \
	utility/IEEE802154A.h \
	mac/AlohaMacLayer.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/FWMath.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	phy/DeciderResultUWBIR.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	utility/Packet.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mac/UWBIRMac.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/messages/AirFrame_m.h \
	application/TestApplication.h \
	../base/messages/MacPkt_m.h
$O/connectionManager/UnitDisk.o: connectionManager/UnitDisk.cc \
	../base/connectionManager/NicEntry.h \
	../base/connectionManager/ConnectionManager.h \
	../base/utils/Coord.h \
	connectionManager/UnitDisk.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/utils/FWMath.h
$O/mac/csma.o: mac/csma.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/modules/AddressingInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BaseLayer.h \
	../base/modules/BatteryAccess.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/phyLayer/Decider.h \
	../base/messages/SyncPkt_m.h \
	../base/connectionManager/BaseConnectionManager.h \
	mac/csma.h \
	../base/phyLayer/Interpolation.h \
	../base/phyLayer/BaseDecider.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/SimpleAddress.h \
	../base/utils/FWMath.h \
	../base/modules/ArpInterface.h \
	../base/utils/ImNotifiable.h \
	../base/modules/BaseArp.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	utility/DroppedPacket.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseModule.h \
	../base/utils/MacToNetwControlInfo.h \
	phy/DeciderResult802154Narrow.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/utils/PassedMessage.h \
	../base/utils/NetwToMacControlInfo.h \
	../base/messages/AirFrame_m.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/messages/MacPkt_m.h
$O/mac/AlohaMacLayer.o: mac/AlohaMacLayer.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/modules/BaseMacLayer.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/phyLayer/Decider.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	../base/connectionManager/BaseConnectionManager.h \
	phy/RadioUWBIR.h \
	../base/phyLayer/Interpolation.h \
	utility/SimpleTimeConstMapping.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	utility/IEEE802154A.h \
	mac/AlohaMacLayer.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/FWMath.h \
	../base/phyLayer/ChannelInfo.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	../base/phyLayer/MappingBase.h \
	phy/DeciderResultUWBIR.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	utility/Packet.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	mac/UWBIRMac.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/utils/PassedMessage.h \
	../base/utils/NetwToMacControlInfo.h \
	../base/messages/AirFrame_m.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/messages/MacPkt_m.h
$O/mac/Mac80211.o: mac/Mac80211.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/modules/BaseMacLayer.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/phyLayer/Decider.h \
	messages/Mac80211Pkt_m.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	../base/phyLayer/BaseDecider.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/SimpleAddress.h \
	../base/utils/FWMath.h \
	../base/utils/ImNotifiable.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	mac/Mac80211.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	phy/Decider80211.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	phy/DeciderResult80211.h \
	../base/modules/BaseModule.h \
	../base/utils/MacToNetwControlInfo.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	utility/Consts80211.h \
	../base/utils/NetwToMacControlInfo.h \
	../base/utils/PassedMessage.h \
	../base/messages/AirFrame_m.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/messages/MacPkt_m.h
$O/mac/CSMA802154.o: mac/CSMA802154.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BaseLayer.h \
	../base/modules/BatteryAccess.h \
	../base/phyLayer/Decider.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/messages/SyncPkt_m.h \
	../base/connectionManager/BaseConnectionManager.h \
	mac/csma.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/FWMath.h \
	../base/utils/ImNotifiable.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	mac/CSMA802154.h \
	utility/DroppedPacket.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseModule.h \
	../base/utils/MacToNetwControlInfo.h \
	phy/DeciderResult802154Narrow.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/utils/PassedMessage.h \
	../base/messages/AirFrame_m.h \
	../base/messages/MacPkt_m.h
$O/mac/CSMAMacLayer.o: mac/CSMAMacLayer.cc \
	../base/modules/AddressingInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BaseLayer.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/SimpleAddress.h \
	../base/utils/FWMath.h \
	../base/modules/ArpInterface.h \
	../base/utils/ImNotifiable.h \
	../base/modules/BaseArp.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mac/CSMAMacLayer.h \
	../base/modules/BaseModule.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/utils/PassedMessage.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/messages/MacPkt_m.h
$O/mac/UWBIRMac.o: mac/UWBIRMac.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BaseLayer.h \
	../base/modules/BatteryAccess.h \
	../base/phyLayer/Decider.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/connectionManager/BaseConnectionManager.h \
	phy/RadioUWBIR.h \
	utility/SimpleTimeConstMapping.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	utility/IEEE802154A.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/phyLayer/ChannelInfo.h \
	../base/utils/FWMath.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	phy/DeciderResultUWBIR.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	phy/MacToUWBIRPhyControlInfo.h \
	utility/Packet.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	../base/connectionManager/NicEntry.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/utils/HostState.h \
	../base/modules/BaseModule.h \
	mac/UWBIRMac.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/utils/PassedMessage.h \
	../base/messages/AirFrame_m.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/messages/MacPkt_m.h
$O/messages/AggrPkt_m.o: messages/AggrPkt_m.cc \
	../base/messages/ApplPkt_m.h \
	messages/AggrPkt_m.h
$O/messages/DummyRoutePkt_m.o: messages/DummyRoutePkt_m.cc \
	messages/DummyRoutePkt_m.h \
	../base/messages/NetwPkt_m.h
$O/messages/UWBIRMacPkt_m.o: messages/UWBIRMacPkt_m.cc \
	messages/UWBIRMacPkt_m.h \
	../base/messages/MacPkt_m.h
$O/messages/Mac80211Pkt_m.o: messages/Mac80211Pkt_m.cc \
	messages/Mac80211Pkt_m.h \
	../base/messages/MacPkt_m.h
$O/messages/UWBIRMacPkt.o: messages/UWBIRMacPkt.cc \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/messages/MacPkt_m.h
$O/messages/AggrPkt.o: messages/AggrPkt.cc \
	../base/messages/ApplPkt_m.h \
	messages/AggrPkt.h \
	messages/AggrPkt_m.h
$O/messages/AirFrameUWBIR_m.o: messages/AirFrameUWBIR_m.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/phyLayer/Interpolation.h \
	messages/AirFrameUWBIR_m.h \
	../base/utils/Move.h \
	utility/IEEE802154A.h \
	../base/utils/HostState.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/messages/AirFrame_m.h \
	../base/utils/FWMath.h
$O/mobility/ANSimMobility.o: mobility/ANSimMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	mobility/ANSimMobility.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	mobility/LineSegmentsMobilityBase.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/RectangleMobility.o: mobility/RectangleMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mobility/RectangleMobility.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/TractorMobility.o: mobility/TractorMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	mobility/TractorMobility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/TurtleMobility.o: mobility/TurtleMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	mobility/TurtleMobility.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	mobility/LineSegmentsMobilityBase.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/MassMobility.o: mobility/MassMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mobility/MassMobility.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/BonnMotionFileCache.o: mobility/BonnMotionFileCache.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	mobility/BonnMotionFileCache.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/ConstSpeedMobility.o: mobility/ConstSpeedMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	mobility/ConstSpeedMobility.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/LineSegmentsMobilityBase.o: mobility/LineSegmentsMobilityBase.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	mobility/LineSegmentsMobilityBase.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/BonnMotionMobility.o: mobility/BonnMotionMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	mobility/BonnMotionFileCache.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	mobility/BonnMotionMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	mobility/LineSegmentsMobilityBase.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/LinearMobility.o: mobility/LinearMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	mobility/LinearMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/mobility/CircleMobility.o: mobility/CircleMobility.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	mobility/CircleMobility.h \
	../base/modules/BaseMobility.h \
	../base/utils/Move.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/FWMath.h
$O/netw/DummyRoute.o: netw/DummyRoute.cc \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	utility/SimTracer.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BaseLayer.h \
	messages/DummyRoutePkt_m.h \
	../base/modules/BatteryAccess.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/modules/BaseMobility.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/SimpleAddress.h \
	../base/utils/FWMath.h \
	../base/modules/ArpInterface.h \
	../base/utils/ImNotifiable.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/connectionManager/ConnectionManager.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/modules/BaseNetwLayer.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	utility/Packet.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	netw/DummyRoute.h \
	../base/modules/BaseModule.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/utils/PassedMessage.h \
	../base/messages/NetwPkt_m.h \
	../base/utils/NetwToMacControlInfo.h \
	../base/messages/MacPkt_m.h \
	../base/utils/NetwControlInfo.h
$O/netw/ArpHost.o: netw/ArpHost.cc \
	../base/modules/AddressingInterface.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/HostState.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	../base/modules/BaseModule.h \
	netw/ArpHost.h \
	../base/utils/FWMath.h \
	../base/modules/ArpInterface.h
$O/phy/PhyLayerBattery.o: phy/PhyLayerBattery.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	phy/PhyLayer.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/modules/BatteryAccess.h \
	../base/phyLayer/Decider.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	../base/phyLayer/BaseDecider.h \
	phy/PhyLayerBattery.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/FWMath.h \
	../base/phyLayer/ChannelInfo.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	phy/Decider80211Battery.h \
	phy/Decider80211.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	utility/Consts80211.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/messages/AirFrame_m.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/messages/MacPkt_m.h
$O/phy/DeciderUWBIRED.o: phy/DeciderUWBIRED.cc \
	phy/PhyLayer.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BaseLayer.h \
	../base/modules/BatteryAccess.h \
	analogueModel/UWBIRStochasticPathlossModel.h \
	../base/utils/Move.h \
	phy/PhyLayerBattery.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/ChannelInfo.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	utility/UWBIRPacket.h \
	../base/phyLayer/Mapping.h \
	phy/PhyLayerUWBIR.h \
	../base/modules/BaseWorldUtility.h \
	phy/Decider80211Battery.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	phy/DeciderUWBIREDSyncOnAddress.h \
	../base/utils/PassedMessage.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/BaseUtility.h \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/phyLayer/Decider.h \
	phy/RadioUWBIR.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	utility/SimpleTimeConstMapping.h \
	../base/phyLayer/BaseDecider.h \
	phy/DeciderUWBIRED.h \
	utility/IEEE802154A.h \
	mac/AlohaMacLayer.h \
	../base/phyLayer/PhyUtils.h \
	phy/DeciderUWBIREDSync.h \
	../base/utils/FWMath.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	phy/DeciderResultUWBIR.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	utility/Packet.h \
	messages/AirFrameUWBIR_m.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	phy/Decider80211.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mac/UWBIRMac.h \
	utility/Consts80211.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/messages/AirFrame_m.h \
	../base/messages/MacPkt_m.h
$O/phy/DeciderUWBIREDSync.o: phy/DeciderUWBIREDSync.cc \
	../base/modules/Blackboard.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/ChannelInfo.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	utility/UWBIRPacket.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/BaseUtility.h \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/phyLayer/Decider.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	phy/RadioUWBIR.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	utility/SimpleTimeConstMapping.h \
	phy/DeciderUWBIRED.h \
	utility/IEEE802154A.h \
	mac/AlohaMacLayer.h \
	../base/phyLayer/PhyUtils.h \
	phy/DeciderUWBIREDSync.h \
	../base/utils/FWMath.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	phy/DeciderResultUWBIR.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	utility/Packet.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mac/UWBIRMac.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/messages/AirFrame_m.h \
	../base/messages/MacPkt_m.h
$O/phy/SNRThresholdDecider.o: phy/SNRThresholdDecider.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/utils/ImNotifiable.h \
	phy/SNRThresholdDecider.h \
	../base/modules/Blackboard.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/Decider.h \
	../base/phyLayer/Interpolation.h \
	../base/phyLayer/BaseDecider.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/utils/HostState.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/messages/AirFrame_m.h \
	../base/utils/FWMath.h
$O/phy/Decider802154Narrow.o: phy/Decider802154Narrow.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	phy/Decider802154Narrow.h \
	utility/Consts802154.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/phyLayer/Decider.h \
	../base/phyLayer/Interpolation.h \
	../base/phyLayer/BaseDecider.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/utils/HostState.h \
	phy/DeciderResult802154Narrow.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	../base/messages/AirFrame_m.h \
	../base/utils/FWMath.h \
	../base/messages/MacPkt_m.h
$O/phy/PhyLayerUWBIR.o: phy/PhyLayerUWBIR.cc \
	phy/PhyLayer.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BaseLayer.h \
	../base/modules/BatteryAccess.h \
	analogueModel/UWBIRStochasticPathlossModel.h \
	phy/PhyLayerBattery.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/ChannelInfo.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	utility/UWBIRPacket.h \
	../base/phyLayer/Mapping.h \
	phy/PhyLayerUWBIR.h \
	../base/modules/BaseWorldUtility.h \
	phy/Decider80211Battery.h \
	phy/MacToUWBIRPhyControlInfo.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	phy/DeciderUWBIREDSyncOnAddress.h \
	../base/utils/PassedMessage.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/BaseUtility.h \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	../base/phyLayer/Decider.h \
	phy/RadioUWBIR.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	utility/SimpleTimeConstMapping.h \
	../base/phyLayer/BaseDecider.h \
	phy/DeciderUWBIRED.h \
	utility/IEEE802154A.h \
	mac/AlohaMacLayer.h \
	../base/phyLayer/PhyUtils.h \
	phy/DeciderUWBIREDSync.h \
	../base/utils/FWMath.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	phy/DeciderResultUWBIR.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	utility/Packet.h \
	messages/AirFrameUWBIR_m.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	phy/Decider80211.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mac/UWBIRMac.h \
	utility/Consts80211.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/messages/AirFrame_m.h \
	../base/messages/MacPkt_m.h
$O/phy/PhyLayer.o: phy/PhyLayer.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	phy/PhyLayer.h \
	../base/phyLayer/AnalogueModel.h \
	phy/SNRThresholdDecider.h \
	../base/modules/Blackboard.h \
	phy/Decider802154Narrow.h \
	../base/modules/BaseUtility.h \
	../base/modules/BatteryAccess.h \
	../base/phyLayer/Decider.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	../base/phyLayer/BaseDecider.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	analogueModel/JakesFading.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	analogueModel/LogNormalShadowing.h \
	analogueModel/SimplePathlossModel.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/FWMath.h \
	../base/phyLayer/ChannelInfo.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	phy/Decider80211.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	utility/Consts80211.h \
	../base/messages/AirFrame_m.h \
	../base/messages/MacPkt_m.h
$O/phy/DeciderUWBIREDSyncOnAddress.o: phy/DeciderUWBIREDSyncOnAddress.cc \
	../base/modules/Blackboard.h \
	../base/modules/BaseMacLayer.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/modules/BaseBattery.h \
	../base/phyLayer/ChannelInfo.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/FindModule.h \
	../base/phyLayer/Mapping.h \
	utility/UWBIRPacket.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/BasePhyLayer.h \
	../base/modules/BaseModule.h \
	phy/DeciderUWBIREDSyncOnAddress.h \
	../base/utils/PassedMessage.h \
	../base/phyLayer/MacToPhyControlInfo.h \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/phyLayer/AnalogueModel.h \
	../base/modules/BaseUtility.h \
	messages/UWBIRMacPkt_m.h \
	messages/UWBIRMacPkt.h \
	../base/phyLayer/Decider.h \
	../base/phyLayer/PhyToMacControlInfo.h \
	phy/RadioUWBIR.h \
	../base/connectionManager/BaseConnectionManager.h \
	../base/phyLayer/Interpolation.h \
	utility/SimpleTimeConstMapping.h \
	phy/DeciderUWBIRED.h \
	utility/IEEE802154A.h \
	mac/AlohaMacLayer.h \
	../base/phyLayer/PhyUtils.h \
	../base/utils/FWMath.h \
	../base/connectionManager/ChannelAccess.h \
	../base/utils/ImNotifiable.h \
	phy/DeciderResultUWBIR.h \
	../base/phyLayer/MacToPhyInterface.h \
	../base/utils/Coord.h \
	utility/Packet.h \
	analogueModel/UWBIRIEEE802154APathlossModel.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	mac/UWBIRMac.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/messages/AirFrame_m.h \
	../base/messages/MacPkt_m.h
$O/phy/Decider80211.o: phy/Decider80211.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/MappingBase.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	messages/Mac80211Pkt_m.h \
	../base/phyLayer/Decider.h \
	../base/phyLayer/Interpolation.h \
	phy/Decider80211.h \
	../base/phyLayer/BaseDecider.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/utils/HostState.h \
	phy/DeciderResult80211.h \
	../base/phyLayer/MappingUtils.h \
	../base/phyLayer/Signal_.h \
	utility/Consts80211.h \
	../base/messages/AirFrame_m.h \
	../base/utils/FWMath.h \
	../base/messages/MacPkt_m.h
$O/phy/Decider80211Battery.o: phy/Decider80211Battery.cc \
	../base/phyLayer/DeciderToPhyInterface.h \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/utils/Coord.h \
	../base/phyLayer/Mapping.h \
	../base/modules/BaseWorldUtility.h \
	../base/phyLayer/Decider.h \
	phy/Decider80211Battery.h \
	../base/phyLayer/Interpolation.h \
	phy/Decider80211.h \
	../base/phyLayer/BaseDecider.h \
	../base/utils/Move.h \
	../base/phyLayer/ChannelState.h \
	../base/messages/ChannelSenseRequest_m.h \
	../base/utils/HostState.h \
	../base/phyLayer/Signal_.h \
	utility/Consts80211.h \
	../base/phyLayer/MappingUtils.h \
	../base/messages/AirFrame_m.h \
	../base/utils/FWMath.h
$O/power/battery/BatteryStats.o: power/battery/BatteryStats.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	power/battery/BatteryStats.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	power/battery/DeviceEntry.h \
	power/battery/BatteryState.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	power/battery/SimpleBattery.h \
	../base/utils/FWMath.h
$O/power/battery/SimpleBattery.o: power/battery/SimpleBattery.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	power/battery/BatteryStats.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	power/battery/DeviceEntry.h \
	power/battery/BatteryState.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	power/battery/SimpleBattery.h \
	../base/utils/FWMath.h
$O/transport/Aggregation.o: transport/Aggregation.cc \
	../base/utils/ImNotifiable.h \
	../base/messages/ApplPkt_m.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	../base/utils/Coord.h \
	messages/AggrPkt.h \
	../base/modules/BaseLayer.h \
	../base/modules/BatteryAccess.h \
	transport/Aggregation.h \
	messages/AggrPkt_m.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/utils/FWMath.h
$O/utility/phyPER.o: utility/phyPER.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/HostState.h \
	../base/utils/FindModule.h \
	utility/phyPER.h \
	../base/utils/Coord.h \
	../base/modules/BaseModule.h \
	utility/UWBIRPacket.h \
	../base/utils/FWMath.h \
	utility/Packet.h
$O/utility/IEEE802154A.o: utility/IEEE802154A.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/phyLayer/MappingBase.h \
	../base/modules/BaseUtility.h \
	../base/phyLayer/Mapping.h \
	../base/utils/Coord.h \
	../base/phyLayer/Interpolation.h \
	../base/utils/Move.h \
	utility/IEEE802154A.h \
	../base/utils/HostState.h \
	../base/phyLayer/Signal_.h \
	../base/phyLayer/MappingUtils.h \
	../base/utils/FWMath.h
$O/utility/SimTracer.o: utility/SimTracer.cc \
	../base/utils/ImNotifiable.h \
	../base/modules/Blackboard.h \
	../base/modules/BaseUtility.h \
	../base/utils/FindModule.h \
	utility/SimTracer.h \
	../base/connectionManager/ConnectionManager.h \
	../base/utils/Coord.h \
	../base/modules/BaseWorldUtility.h \
	../base/modules/BatteryAccess.h \
	../base/modules/BaseLayer.h \
	../base/connectionManager/BaseConnectionManager.h \
	utility/Packet.h \
	../base/connectionManager/NicEntry.h \
	../base/utils/HostState.h \
	../base/modules/BaseBattery.h \
	../base/modules/BaseModule.h \
	../base/utils/PassedMessage.h \
	../base/utils/FWMath.h

