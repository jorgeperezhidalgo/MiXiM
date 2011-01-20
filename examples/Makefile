#
# OMNeT++/OMNEST Makefile for MiXiM
#
# This file was generated with the command:
#  opp_makemake -f --nolink -O out -d ieee802154a -d analogueModels -d ieee802154Narrow -d CSMAMac -d Mac80211 -d baseNetwork -d multiConnectionManager -L../out/$(CONFIGNAME)/tests/testUtils -L../out/$(CONFIGNAME)/base -L../out/$(CONFIGNAME)/tests/power/utils -L../out/$(CONFIGNAME)/modules
#

# C++ include paths (with -I)
INCLUDE_PATH = -I.

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = examples
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS =

# Message files
MSGFILES =

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

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target

all: $(OBJS) submakedirs Makefile
	@# Do nothing

submakedirs:  ieee802154a_dir analogueModels_dir ieee802154Narrow_dir CSMAMac_dir Mac80211_dir baseNetwork_dir multiConnectionManager_dir

.PHONY: ieee802154a analogueModels ieee802154Narrow CSMAMac Mac80211 baseNetwork multiConnectionManager
ieee802154a: ieee802154a_dir
analogueModels: analogueModels_dir
ieee802154Narrow: ieee802154Narrow_dir
CSMAMac: CSMAMac_dir
Mac80211: Mac80211_dir
baseNetwork: baseNetwork_dir
multiConnectionManager: multiConnectionManager_dir

ieee802154a_dir:
	cd ieee802154a && $(MAKE)

analogueModels_dir:
	cd analogueModels && $(MAKE)

ieee802154Narrow_dir:
	cd ieee802154Narrow && $(MAKE)

CSMAMac_dir:
	cd CSMAMac && $(MAKE)

Mac80211_dir:
	cd Mac80211 && $(MAKE)

baseNetwork_dir:
	cd baseNetwork && $(MAKE)

multiConnectionManager_dir:
	cd multiConnectionManager && $(MAKE)

.SUFFIXES: .cc

$O/%.o: %.cc
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)
	cd ieee802154a && $(MAKE) msgheaders
	cd analogueModels && $(MAKE) msgheaders
	cd ieee802154Narrow && $(MAKE) msgheaders
	cd CSMAMac && $(MAKE) msgheaders
	cd Mac80211 && $(MAKE) msgheaders
	cd baseNetwork && $(MAKE) msgheaders
	cd multiConnectionManager && $(MAKE) msgheaders

clean:
	-rm -rf $O
	-rm -f MiXiM MiXiM.exe libMiXiM.so libMiXiM.a libMiXiM.dll libMiXiM.dylib
	-rm -f ./*_m.cc ./*_m.h

	-cd ieee802154a && $(MAKE) clean
	-cd analogueModels && $(MAKE) clean
	-cd ieee802154Narrow && $(MAKE) clean
	-cd CSMAMac && $(MAKE) clean
	-cd Mac80211 && $(MAKE) clean
	-cd baseNetwork && $(MAKE) clean
	-cd multiConnectionManager && $(MAKE) clean

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc
	-cd ieee802154a && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd analogueModels && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd ieee802154Narrow && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd CSMAMac && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd Mac80211 && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd baseNetwork && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd multiConnectionManager && if [ -f Makefile ]; then $(MAKE) depend; fi

# DO NOT DELETE THIS LINE -- make depend depends on it.

