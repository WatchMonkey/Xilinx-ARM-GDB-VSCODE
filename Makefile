#
#	Author:Peji
#	Date:2023-04-19
#	Descriptor:top level  makefile
#	Detail:
#		this file was used by user and try to start operation that compiler target binary file
#


.PHONY: all
all: subinclude subdir target


# 
pj_subdirs = LogLib
#pj_subdirs += appDemoLib

.PHONY: subdir $(pj_subdirs)
subdir: $(pj_subdirs)
$(pj_subdirs):
	@echo ~~~~~~~~11~~~~~~~~~pj_subdirs
	$(MAKE) -C $@


.PHONY: subinclude
subinclude:
	@echo ##########11#####pj_subdirs
# ifneq "$(pj_subdirs)" ""
# 	@for temp in $(pj_subdirs);\
# 	do\
# 		INCLUDE_PATH += $$temp;\
# 	done	
# endif


#pj_target := appDemo
pj_target += appAttach


#user_obj = appDemo.o


.PHONY: target
target: clean $(foreach target,$(pj_target),$(target))
$(pj_target):$(pj_target).o
#$(foreach target,$(pj_target),$(target)):$@.o
	-$(RM) $@
	$(CC) $(pj_LDFLAGS) $(LD_LIB_PATH) $(foreach dir,$(pj_subdirs),-L$(shell pwd)\$(dir)) -o $@ $^


.PHONY:clean
clean:
	-$(RM) *.d *.o




# Compiler Rules Reference Define Information

CROSS_COMPILE := arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar


# User Define Flags Information

pj_CFLAGS := 
pj_CPPFLAGS := 
pj_LDFLAGS := -lLogOp
DEBUG_LEVEL := -g
SHARELIB_CFLAGS = -fpic
SHARELIB_LDFLAGS = -shared
INCLUDE_PATH = -I$(shell pwd)
LD_LIB_PATH = -L$(shell pwd)

# Compiler Rules

%.o: %.c
	$(CC) $(DEBUG_LEVEL) $(pj_CFLAGS) $(SHARELIB_CFLAGS) $(INCLUDE_PATH) $(foreach dir,$(pj_subdirs),-I$(shell pwd)\$(dir)) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<

%.o: %.cpp
	$(CPP) $(DEBUG_LEVEL) $(pj_CPPFLAGS) $(SHARELIB_CFLAGS) $(INCLUDE_PATH) $(foreach dir,$(pj_subdirs),-I$(shell pwd)\$(dir)) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<