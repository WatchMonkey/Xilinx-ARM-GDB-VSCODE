#
#	compiler makefile 
#

.PHONY: all
#all: clean libLogOp.so
all: libLogOp.so


user_obj += LogLib.o
libLogOp.so: $(user_obj)
	$(CC) $(SHARELIB_LDFLAGS) $(LD_LIB_PATH) -o $@ $^



.PHONY: clean
clean:
	-$(RM)  *.d *.o *.so


# Compiler Rules Reference Define Information

CROSS_COMPILE := arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar
#RM := rm

# User Define Flags Information

pj_CFLAGS := -fpermissive
pj_CPPFLAGS := -fpermissive
pj_LDFLAGS := 
DEBUG_LEVEL := -g
SHARELIB_CFLAGS = -fpic
SHARELIB_LDFLAGS = -shared
INCLUDE_PATH = -I$(shell pwd)
#INCLUDE_PATH += -I/H/ToolChain-Xilinx/vivado2018.3/gnu/aarch32/nt/gcc-arm-linux-gnueabi/include
LD_LIB_PATH =



# Compiler Rules

%.o: %.c
	$(CC) $(DEBUG_LEVEL) $(pj_CFLAGS) $(SHARELIB_CFLAGS) $(INCLUDE_PATH) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<

%.o: %.cpp
	$(CPP) $(DEBUG_LEVEL) $(pj_CPPFLAGS) $(SHARELIB_CFLAGS) $(INCLUDE_PATH) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<

