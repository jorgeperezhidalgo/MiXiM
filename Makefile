#
# OMNeT++/OMNEST Makefile for MiXiM
#
# This file was generated with the command:
#  opp_makemake -f --nolink -O out -d tests -d examples -d base -d modules -Xinet -L./out/$(CONFIGNAME)/tests/testUtils -L./out/$(CONFIGNAME)/base -L./out/$(CONFIGNAME)/tests/power/utils -L./out/$(CONFIGNAME)/modules
#

# C++ include paths (with -I)
INCLUDE_PATH = -I.

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
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
# inserted from file 'makefrag':
examples_dir: modules_dir
tests_dir: modules_dir
modules_dir: base_dir

# <<<
#------------------------------------------------------------------------------

# Main target

all: $(OBJS) submakedirs Makefile
	@# Do nothing

submakedirs:  tests_dir examples_dir base_dir modules_dir

.PHONY: tests examples base modules
tests: tests_dir
examples: examples_dir
base: base_dir
modules: modules_dir

tests_dir:
	cd tests && $(MAKE)

examples_dir:
	cd examples && $(MAKE)

base_dir:
	cd base && $(MAKE)

modules_dir:
	cd modules && $(MAKE)

.SUFFIXES: .cc

$O/%.o: %.cc
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)
	cd tests && $(MAKE) msgheaders
	cd examples && $(MAKE) msgheaders
	cd base && $(MAKE) msgheaders
	cd modules && $(MAKE) msgheaders

clean:
	-rm -rf $O
	-rm -f MiXiM MiXiM.exe libMiXiM.so libMiXiM.a libMiXiM.dll libMiXiM.dylib
	-rm -f ./*_m.cc ./*_m.h

	-cd tests && $(MAKE) clean
	-cd examples && $(MAKE) clean
	-cd base && $(MAKE) clean
	-cd modules && $(MAKE) clean

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc
	-cd tests && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd examples && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd base && if [ -f Makefile ]; then $(MAKE) depend; fi
	-cd modules && if [ -f Makefile ]; then $(MAKE) depend; fi

# DO NOT DELETE THIS LINE -- make depend depends on it.

