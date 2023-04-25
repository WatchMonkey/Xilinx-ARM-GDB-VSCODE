#
#	compiler makefile 
#

.PHONY: all

all: libLogOp.so

user_obj := 

user_obj += 
libLogOp.so: $(user_obj)
	$(CC) $(SHARELIB_LDFLAGS) $(INCLUDE_PATH) -o $@ $^




# Compiler Rules Reference Define Information

CROSS_COMPILE := arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar


# User Define Flags Information

pj_CFLAGS := 
pj_CPPFLAGS := 
pj_LDFLAGS := 
DEBUG_LEVEL := -g
SHARELIB_CFLAGS = -fpic
SHARELIB_LDFLAGS = -shared
INCLUDE_PATH = -I$(pwd)


# Compiler Rules

%.o: %.c
	$(CC) $(DEBUG_LEVEL) $(pj_CFLAGS) $(SHARELIB_CFLAGS) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<

%.o: %.cpp
	$(CPP) $(DEBUG_LEVEL) $(pj_CPPFLAGS) $(SHARELIB_CFLAGS) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<

