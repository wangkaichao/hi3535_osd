#Makefile

ROOTDIR = ../..
HISIDIR = $(ROOTDIR)/hisi_040

CROSS ?= arm-hisiv100nptl-linux-

CC := $(CROSS)gcc
AR := $(CROSS)ar
LD := $(CROSS)ld
STRIP := $(CROSS)strip 
RM := rm -rf
CP := cp -rf

CSOURCE:=$(shell find ./ -name "*.c")
HSOURCE:=$(shell find ./ -name "*.h")
OBJECTS:=$(patsubst %.c, %.o, $(CSOURCE))


INC += -I./ -I../inc -I../src -I$(ROOTDIR)/inc -I$(HISIDIR)/include -I$(HISIDIR)/common -I$(HISIDIR)/extdrv
LINK_LIB += -L$(ROOTDIR)/lib -L$(HISIDIR)/lib

HIARCH := hi3535
HICHIP := 0x35350100
HIDBG := HI_RELEASE
HI_FPGA := HI_XXXX

CFLAGS += -Wall $(INC) -D$(HIARCH) -DHICHIP=$(HICHIP) -D$(HIDBG) -D$(HI_FPGA) 
CFLAGS += -march=armv7-a -mcpu=cortex-a9
	
LDLIBS := -lparam -lsample_common -lcomm -lnet\
	-lmpi -lhdmi -ltde -lVoiceEngine -lvqe -lresampler -ljpeg -lmem\
	-lpthread -lm
	
#	-lpthread -lm -lrt -ldl -lc -lstdc++
	
%.o : %.c
	$(CC) -c $< $(CFLAGS)

all : $(TARGETLIB) $(TARGETEXE)

ifneq ($(TARGETLIB),)
$(TARGETLIB) : $(OBJECTS)
	$(AR) -rv $@ $^
	$(CP) $@ $(ROOTDIR)/lib
clean:	
	$(RM) $(OBJECTS) $(TARGETLIB)
	$(RM) $(ROOTDIR)/lib/$(TARGETLIB)
endif	

ifneq ($(TARGETEXE),)
$(TARGETEXE) : $(OBJECTS) $(HSOURCE) 
	$(CC) -o $@ $^ $(CFLAGS) $(LINK_LIB) $(LDLIBS)
	$(STRIP) $@
	$(CP) $@ /opt/source/hi3535_osd/rootfs_uclibc_hi3535_040_16M/bin/
clean:
	$(RM) $(OBJECTS) $(TARGETEXE)
endif
