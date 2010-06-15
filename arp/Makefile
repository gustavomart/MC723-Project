
# ####################################################
# ARP main Makefile
# ####################################################

# Name of the platform
PLATFORM := fft_mips

# Variable that points to SystemC installation path
#export SYSTEMC :=  /home/$(USER)/mc723/systemc
export SYSTEMC :=  /home/staff/rodolfo/mc723/systemc

# Variable that points to TLM installation path
#export TLM_PATH := /home/$(USER)/mc723/tlm
export TLM_PATH := /home/staff/rodolfo/mc723/tlm

# Variable that points to ArchC installation path
#export ARCHC_PATH = /home/$(USER)/mc723/archc
export ARCHC_PATH := /home/staff/rodolfo/mc723/archc

export PATH:=/l/archc/compilers/bin:$(PATH)

export ARP:=$(PWD)

# ####################################################
# You don't need to alter after this point
# ####################################################

include platforms/$(PLATFORM)/defs.arp

export HOST_OS:= linux

export LIB_DIR:=-L $(SYSTEMC)/lib-$(HOST_OS) -L $(ARCHC_PATH)/lib \
  $(foreach ip, $(IP), -L $(ARP)/ip/$(ip)) \
  $(foreach is, $(IS), -L $(ARP)/is/$(is)) \
  $(foreach wrapper, $(WRAPPER), -L $(ARP)/wrappers/$(wrapper)) \
  $(foreach processor, $(PROCESSOR), -L $(ARP)/processors/$(processor))

export INC_DIR:=-I $(SYSTEMC)/include -I $(ARCHC_PATH)/include/archc -I $(TLM_PATH) \
  $(foreach ip, $(IP), -I $(ARP)/ip/$(ip)) \
  $(foreach is, $(IS), -I $(ARP)/is/$(is)) \
  $(foreach wrapper, $(WRAPPER), -I $(ARP)/wrappers/$(wrapper)) \
  $(foreach processor, $(PROCESSOR), -I $(ARP)/processors/$(processor))

export LIBS:= \
  $(foreach processor, $(PROCESSOR), -l$(processor))\
  $(foreach ip, $(IP), -l$(ip)) \
  $(foreach is, $(IS), -l$(is))\
  $(foreach wrapper, $(WRAPPER), -l$(wrapper)) \
  -lsystemc -larchc -lm

export LIBFILES:= \
  $(foreach processor, $(PROCESSOR), $(ARP)/processors/$(processor)/lib$(processor).a)\
  $(foreach ip, $(IP), $(ARP)/ip/$(ip)/lib$(ip).a)\
  $(foreach is, $(IS), $(ARP)/is/$(is)/lib$(is).a)\
  $(foreach wrapper, $(WRAPPER), $(ARP)/wrapper/$(wrapper)/lib$(wrapper).a)\

#export CFLAGS:=-g
export CFLAGS:=-O3

export CC:=g++

#------------------------------------------------------
.SILENT:

#------------------------------------------------------
all:
	cd $(ARP)
	for ip in $(IP); do echo Making IP $$ip ...; \
	    cd ip/$$ip; $(MAKE) lib; cd $(ARP); done
	for is in $(IS); do echo Making IS $$is ...; \
            cd is/$$is; $(MAKE) lib; cd $(ARP); done
	for wrapper in $(WRAPPER); do \
            echo Making Wrapper $$wrapper ...; \
            cd wrappers/$$wrapper; $(MAKE) lib; \
            cd $(ARP); done
	for processor in $(PROCESSOR); do \
            echo Making Processor $$processor ...;\
            cd processors/$$processor ; $(MAKE) lib ; \
             cd $(ARP); done
	for sw in $(SW); do \
            echo Making Software $$sw ...; cd sw/$$sw; \
            $(MAKE); cd $(ARP); done
	for sw in $(SW) ; do \
            cp sw/$$sw/*.x platforms/$(PLATFORM) ; done
	echo Making Platform $(PLATFORM)
	cd platforms/$(PLATFORM); $(MAKE)

#------------------------------------------------------
clean:
	cd $(ARP)
	for ip in $(IP) ; do cd ip/$$ip ; \
            $(MAKE) clean ; cd $(ARP); done
	for is in $(IS) ; do cd is/$$is ; \
            $(MAKE) clean ; cd $(ARP); done
	for wrapper in $(WRAPPER); do \
            cd wrappers/$$wrapper; $(MAKE) clean; \
            cd $(ARP); done
	for processor in $(PROCESSOR) ; do \
            cd processors/$$processor ; $(MAKE) clean; \
            cd $(ARP); done
	for sw in $(SW) ; do cd sw/$$sw ; \
            $(MAKE) clean ; cd $(ARP); done
	for sw in $(SW) ; do \
            rm -f platforms/$(PLATFORM)/*.x; cd $(ARP); done
	cd platforms/$(PLATFORM); $(MAKE) clean

#------------------------------------------------------
distclean:
	cd $(ARP)
	for ip in $(IP) ; do cd ip/$$ip ; \
            $(MAKE) distclean ; cd $(ARP); done
	for is in $(IS) ; do cd is/$$is ; \
            $(MAKE) distclean ; cd $(ARP); done
	for wrapper in $(WRAPPER); do \
            cd wrappers/$$wrapper; $(MAKE) distclean; \
            cd $(ARP); done
	for processor in $(PROCESSOR) ; do \
            cd processors/$$processor ; $(MAKE) distclean; \
            cd $(ARP); done
	for sw in $(SW) ; do cd sw/$$sw ; \
            $(MAKE) distclean ; cd $(ARP); done
	for sw in $(SW) ; do \
            rm -f platforms/$(PLATFORM)/*.x; cd $(ARP); done
	cd platforms/$(PLATFORM); $(MAKE) distclean

#------------------------------------------------------
run: all
	cd $(ARP)
	cd platforms/$(PLATFORM); $(MAKE) run
