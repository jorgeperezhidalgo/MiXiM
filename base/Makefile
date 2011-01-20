#
# OMNeT++/OMNEST Makefile for libmiximbase
#
# This file was generated with the command:
#  opp_makemake -f --deep --make-so -o miximbase -O out -L../out/$(CONFIGNAME)/tests/testUtils -L../out/$(CONFIGNAME)/tests/power/utils -L../out/$(CONFIGNAME)/modules
#

# Name of target to be created (-o option)
TARGET = libmiximbase$(SHARED_LIB_SUFFIX)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -IconnectionManager \
    -Imessages \
    -Imodules \
    -IphyLayer \
    -Itimers \
    -Iutils

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = -L../out/$(CONFIGNAME)/tests/testUtils -L../out/$(CONFIGNAME)/tests/power/utils -L../out/$(CONFIGNAME)/modules
LIBS += -Wl,-rpath,`abspath ../out/$(CONFIGNAME)/tests/testUtils` -Wl,-rpath,`abspath ../out/$(CONFIGNAME)/tests/power/utils` -Wl,-rpath,`abspath ../out/$(CONFIGNAME)/modules`

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = base
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = \
    $O/connectionManager/NicEntryDirect.o \
    $O/connectionManager/ChannelAccess.o \
    $O/connectionManager/ConnectionManager.o \
    $O/connectionManager/NicEntryDebug.o \
    $O/connectionManager/BaseConnectionManager.o \
    $O/modules/BaseModule.o \
    $O/modules/BaseApplLayer.o \
    $O/modules/BatteryAccess.o \
    $O/modules/BaseWorldUtility.o \
    $O/modules/Blackboard.o \
    $O/modules/BaseBattery.o \
    $O/modules/TestApplLayer.o \
    $O/modules/BaseUtility.o \
    $O/modules/BaseMobility.o \
    $O/modules/BaseArp.o \
    $O/modules/BaseNetwLayer.o \
    $O/modules/BaseLayer.o \
    $O/modules/BaseMacLayer.o \
    $O/phyLayer/BaseDecider.o \
    $O/phyLayer/PhyUtils.o \
    $O/phyLayer/ChannelState.o \
    $O/phyLayer/Signal.o \
    $O/phyLayer/BasePhyLayer.o \
    $O/phyLayer/Decider.o \
    $O/phyLayer/ChannelInfo.o \
    $O/phyLayer/MappingBase.o \
    $O/phyLayer/MappingUtils.o \
    $O/timers/RepeatTimerCore.o \
    $O/timers/Timer.o \
    $O/timers/TimerCore.o \
    $O/timers/RepeatTimer.o \
    $O/timers/GlobalTime.o \
    $O/timers/FrameTimer.o \
    $O/timers/FrameTimerGenerator.o \
    $O/utils/winsupport.o \
    $O/utils/Coord.o \
    $O/messages/AirFrame_m.o \
    $O/messages/ApplPkt_m.o \
    $O/messages/BorderMsg_m.o \
    $O/messages/MacPkt_m.o \
    $O/messages/ChannelSenseRequest_m.o \
    $O/messages/NetwPkt_m.o

# Message files
MSGFILES = \
    messages/AirFrame.msg \
    messages/ApplPkt.msg \
    messages/BorderMsg.msg \
    messages/MacPkt.msg \
    messages/ChannelSenseRequest.msg \
    messages/NetwPkt.msg

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
	-rm -f miximbase miximbase.exe libmiximbase.so libmiximbase.a libmiximbase.dll libmiximbase.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f connectionManager/*_m.cc connectionManager/*_m.h
	-rm -f messages/*_m.cc messages/*_m.h
	-rm -f modules/*_m.cc modules/*_m.h
	-rm -f phyLayer/*_m.cc phyLayer/*_m.h
	-rm -f timers/*_m.cc timers/*_m.h
	-rm -f utils/*_m.cc utils/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc connectionManager/*.cc messages/*.cc modules/*.cc phyLayer/*.cc timers/*.cc utils/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/connectionManager/ChannelAccess.o: connectionManager/ChannelAccess.cc \
	connectionManager/ChannelAccess.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	phyLayer/MappingBase.h \
	modules/BaseUtility.h \
	utils/FindModule.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	modules/BatteryAccess.h \
	connectionManager/BaseConnectionManager.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	connectionManager/NicEntry.h \
	utils/HostState.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	messages/AirFrame_m.h \
	utils/FWMath.h
$O/connectionManager/NicEntryDebug.o: connectionManager/NicEntryDebug.cc \
	connectionManager/ChannelAccess.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	phyLayer/MappingBase.h \
	modules/BaseUtility.h \
	utils/FindModule.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	modules/BatteryAccess.h \
	connectionManager/BaseConnectionManager.h \
	phyLayer/Interpolation.h \
	connectionManager/NicEntryDebug.h \
	utils/Move.h \
	connectionManager/NicEntry.h \
	utils/HostState.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	messages/AirFrame_m.h \
	utils/FWMath.h
$O/connectionManager/NicEntryDirect.o: connectionManager/NicEntryDirect.cc \
	connectionManager/ChannelAccess.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	phyLayer/MappingBase.h \
	modules/BaseUtility.h \
	connectionManager/NicEntryDirect.h \
	utils/FindModule.h \
	utils/Coord.h \
	phyLayer/Mapping.h \
	modules/BaseWorldUtility.h \
	modules/BatteryAccess.h \
	connectionManager/BaseConnectionManager.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	connectionManager/NicEntry.h \
	utils/HostState.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	messages/AirFrame_m.h \
	utils/FWMath.h
$O/connectionManager/ConnectionManager.o: connectionManager/ConnectionManager.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	connectionManager/NicEntry.h \
	connectionManager/ConnectionManager.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	connectionManager/BaseConnectionManager.h \
	utils/FWMath.h
$O/connectionManager/BaseConnectionManager.o: connectionManager/BaseConnectionManager.cc \
	utils/ImNotifiable.h \
	connectionManager/NicEntryDebug.h \
	modules/Blackboard.h \
	connectionManager/NicEntry.h \
	connectionManager/NicEntryDirect.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	connectionManager/BaseConnectionManager.h \
	utils/FWMath.h
$O/messages/AirFrame_m.o: messages/AirFrame_m.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	phyLayer/MappingBase.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	utils/HostState.h \
	phyLayer/Signal_.h \
	phyLayer/MappingUtils.h \
	messages/AirFrame_m.h \
	utils/FWMath.h
$O/messages/MacPkt_m.o: messages/MacPkt_m.cc \
	messages/MacPkt_m.h
$O/messages/NetwPkt_m.o: messages/NetwPkt_m.cc \
	messages/NetwPkt_m.h
$O/messages/ChannelSenseRequest_m.o: messages/ChannelSenseRequest_m.cc \
	phyLayer/ChannelState.h \
	messages/ChannelSenseRequest_m.h
$O/messages/BorderMsg_m.o: messages/BorderMsg_m.cc \
	utils/Coord.h \
	messages/BorderMsg_m.h \
	utils/FWMath.h
$O/messages/ApplPkt_m.o: messages/ApplPkt_m.cc \
	messages/ApplPkt_m.h
$O/modules/BaseUtility.o: modules/BaseUtility.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	modules/BatteryAccess.h \
	modules/BaseMobility.h \
	utils/Move.h \
	utils/HostState.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	utils/FWMath.h
$O/modules/BaseLayer.o: modules/BaseLayer.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	utils/Coord.h \
	modules/BatteryAccess.h \
	modules/BaseLayer.h \
	utils/PassedMessage.h \
	utils/FWMath.h
$O/modules/BaseWorldUtility.o: modules/BaseWorldUtility.cc \
	utils/ImNotifiable.h \
	connectionManager/NicEntry.h \
	modules/Blackboard.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	connectionManager/BaseConnectionManager.h \
	utils/FWMath.h
$O/modules/BaseNetwLayer.o: modules/BaseNetwLayer.cc \
	modules/AddressingInterface.h \
	phyLayer/AnalogueModel.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	modules/BaseMacLayer.h \
	modules/BaseLayer.h \
	modules/BatteryAccess.h \
	connectionManager/BaseConnectionManager.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	phyLayer/ChannelState.h \
	modules/BaseBattery.h \
	phyLayer/PhyUtils.h \
	utils/SimpleAddress.h \
	utils/FWMath.h \
	modules/ArpInterface.h \
	utils/ImNotifiable.h \
	phyLayer/MappingBase.h \
	utils/FindModule.h \
	phyLayer/MacToPhyInterface.h \
	modules/BaseNetwLayer.h \
	utils/Coord.h \
	phyLayer/Mapping.h \
	connectionManager/NicEntry.h \
	utils/HostState.h \
	modules/BaseModule.h \
	phyLayer/Signal_.h \
	phyLayer/MappingUtils.h \
	messages/NetwPkt_m.h \
	utils/NetwToMacControlInfo.h \
	utils/PassedMessage.h \
	utils/NetwControlInfo.h \
	messages/MacPkt_m.h
$O/modules/BaseModule.o: modules/BaseModule.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseModule.h \
	utils/FWMath.h
$O/modules/BaseMacLayer.o: modules/BaseMacLayer.cc \
	modules/AddressingInterface.h \
	phyLayer/AnalogueModel.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	modules/BaseMacLayer.h \
	modules/BatteryAccess.h \
	modules/BaseLayer.h \
	connectionManager/BaseConnectionManager.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	phyLayer/ChannelState.h \
	modules/BaseBattery.h \
	phyLayer/PhyUtils.h \
	utils/SimpleAddress.h \
	utils/FWMath.h \
	connectionManager/ChannelAccess.h \
	utils/ImNotifiable.h \
	phyLayer/MappingBase.h \
	utils/FindModule.h \
	phyLayer/MacToPhyInterface.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	connectionManager/NicEntry.h \
	utils/HostState.h \
	modules/BaseModule.h \
	utils/MacToNetwControlInfo.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	utils/NetwToMacControlInfo.h \
	utils/PassedMessage.h \
	messages/AirFrame_m.h \
	messages/MacPkt_m.h
$O/modules/BaseMobility.o: modules/BaseMobility.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	modules/BatteryAccess.h \
	modules/BaseMobility.h \
	utils/Move.h \
	utils/HostState.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	messages/BorderMsg_m.h \
	utils/FWMath.h
$O/modules/BaseApplLayer.o: modules/BaseApplLayer.cc \
	utils/ImNotifiable.h \
	messages/ApplPkt_m.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseLayer.h \
	modules/BatteryAccess.h \
	utils/HostState.h \
	modules/BaseApplLayer.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	utils/PassedMessage.h \
	utils/FWMath.h
$O/modules/BaseBattery.o: modules/BaseBattery.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	utils/Coord.h \
	utils/FWMath.h
$O/modules/Blackboard.o: modules/Blackboard.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h
$O/modules/BaseArp.o: modules/BaseArp.cc \
	modules/AddressingInterface.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseArp.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseModule.h \
	utils/FWMath.h \
	modules/ArpInterface.h
$O/modules/TestApplLayer.o: modules/TestApplLayer.cc \
	utils/ImNotifiable.h \
	messages/ApplPkt_m.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	modules/TestApplLayer.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseLayer.h \
	modules/BatteryAccess.h \
	utils/HostState.h \
	modules/BaseApplLayer.h \
	modules/BaseBattery.h \
	modules/BaseModule.h \
	utils/PassedMessage.h \
	utils/SimpleAddress.h \
	utils/FWMath.h \
	utils/NetwControlInfo.h
$O/modules/BatteryAccess.o: modules/BatteryAccess.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	modules/BaseBattery.h \
	utils/Coord.h \
	modules/BaseModule.h \
	modules/BatteryAccess.h \
	utils/FWMath.h
$O/phyLayer/MappingUtils.o: phyLayer/MappingUtils.cc \
	phyLayer/MappingBase.h \
	phyLayer/MappingUtils.h \
	utils/FWMath.h \
	phyLayer/Interpolation.h
$O/phyLayer/Decider.o: phyLayer/Decider.cc \
	phyLayer/DeciderToPhyInterface.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	phyLayer/MappingBase.h \
	modules/BaseUtility.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	phyLayer/Decider.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	phyLayer/ChannelState.h \
	messages/ChannelSenseRequest_m.h \
	utils/HostState.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	messages/AirFrame_m.h \
	utils/FWMath.h
$O/phyLayer/BasePhyLayer.o: phyLayer/BasePhyLayer.cc \
	phyLayer/DeciderToPhyInterface.h \
	phyLayer/AnalogueModel.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	phyLayer/Decider.h \
	phyLayer/PhyToMacControlInfo.h \
	modules/BatteryAccess.h \
	connectionManager/BaseConnectionManager.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	phyLayer/ChannelState.h \
	messages/ChannelSenseRequest_m.h \
	modules/BaseBattery.h \
	phyLayer/PhyUtils.h \
	phyLayer/ChannelInfo.h \
	utils/FWMath.h \
	connectionManager/ChannelAccess.h \
	utils/ImNotifiable.h \
	phyLayer/MappingBase.h \
	utils/FindModule.h \
	phyLayer/MacToPhyInterface.h \
	utils/Coord.h \
	phyLayer/Mapping.h \
	modules/BaseWorldUtility.h \
	connectionManager/NicEntry.h \
	phyLayer/BasePhyLayer.h \
	utils/HostState.h \
	modules/BaseModule.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	phyLayer/MacToPhyControlInfo.h \
	messages/AirFrame_m.h \
	messages/MacPkt_m.h
$O/phyLayer/MappingBase.o: phyLayer/MappingBase.cc \
	phyLayer/MappingBase.h \
	utils/FWMath.h \
	phyLayer/Interpolation.h
$O/phyLayer/BaseDecider.o: phyLayer/BaseDecider.cc \
	phyLayer/DeciderToPhyInterface.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	phyLayer/MappingBase.h \
	modules/BaseUtility.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	modules/BaseWorldUtility.h \
	phyLayer/Decider.h \
	phyLayer/Interpolation.h \
	phyLayer/BaseDecider.h \
	utils/Move.h \
	phyLayer/ChannelState.h \
	messages/ChannelSenseRequest_m.h \
	utils/HostState.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	messages/AirFrame_m.h \
	utils/FWMath.h
$O/phyLayer/PhyUtils.o: phyLayer/PhyUtils.cc \
	phyLayer/AnalogueModel.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	phyLayer/MappingBase.h \
	modules/BaseUtility.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	utils/HostState.h \
	phyLayer/PhyUtils.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	utils/FWMath.h
$O/phyLayer/ChannelState.o: phyLayer/ChannelState.cc \
	phyLayer/ChannelState.h
$O/phyLayer/ChannelInfo.o: phyLayer/ChannelInfo.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	phyLayer/MappingBase.h \
	modules/BaseUtility.h \
	utils/Coord.h \
	phyLayer/Mapping.h \
	phyLayer/Interpolation.h \
	utils/Move.h \
	utils/HostState.h \
	phyLayer/Signal_.h \
	phyLayer/MappingUtils.h \
	messages/AirFrame_m.h \
	utils/FWMath.h \
	phyLayer/ChannelInfo.h
$O/phyLayer/Signal.o: phyLayer/Signal.cc \
	utils/Move.h \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	phyLayer/MappingBase.h \
	utils/HostState.h \
	phyLayer/Mapping.h \
	utils/Coord.h \
	phyLayer/MappingUtils.h \
	phyLayer/Signal_.h \
	utils/FWMath.h \
	phyLayer/Interpolation.h
$O/timers/RepeatTimer.o: timers/RepeatTimer.cc \
	utils/ImNotifiable.h \
	timers/RepeatTimerCore.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	modules/BaseModule.h \
	utils/Coord.h \
	timers/RepeatTimer.h \
	utils/FWMath.h
$O/timers/TimerCore.o: timers/TimerCore.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	timers/Timer.h \
	timers/TimerCore.h \
	modules/BaseModule.h \
	utils/Coord.h \
	utils/FWMath.h
$O/timers/FrameTimer.o: timers/FrameTimer.cc \
	utils/ImNotifiable.h \
	timers/GlobalTime.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	timers/FrameTimer.h \
	utils/HostState.h \
	utils/FindModule.h \
	modules/BaseModule.h \
	utils/Coord.h \
	timers/FrameTimerGenerator.h \
	utils/FWMath.h
$O/timers/Timer.o: timers/Timer.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	timers/Timer.h \
	timers/TimerCore.h \
	modules/BaseModule.h \
	utils/Coord.h \
	utils/FWMath.h
$O/timers/GlobalTime.o: timers/GlobalTime.cc \
	timers/GlobalTime.h
$O/timers/RepeatTimerCore.o: timers/RepeatTimerCore.cc \
	utils/ImNotifiable.h \
	modules/Blackboard.h \
	timers/RepeatTimerCore.h \
	modules/BaseUtility.h \
	utils/HostState.h \
	utils/FindModule.h \
	utils/Coord.h \
	modules/BaseModule.h \
	timers/RepeatTimer.h \
	utils/FWMath.h
$O/timers/FrameTimerGenerator.o: timers/FrameTimerGenerator.cc \
	utils/ImNotifiable.h \
	timers/GlobalTime.h \
	modules/Blackboard.h \
	modules/BaseUtility.h \
	timers/FrameTimer.h \
	utils/HostState.h \
	utils/FindModule.h \
	modules/BaseModule.h \
	utils/Coord.h \
	timers/FrameTimerGenerator.h \
	utils/FWMath.h
$O/utils/winsupport.o: utils/winsupport.cc \
	utils/winsupport.h
$O/utils/Coord.o: utils/Coord.cc \
	utils/Coord.h \
	utils/FWMath.h

