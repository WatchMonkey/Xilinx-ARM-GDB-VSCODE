#
#	Author:Peji
#	Date:2023-04-19
#	Descriptor:top level  makefile
#	Detail:
#		this file was used by user and try to start operation that compiler target binary file
#


# define phony target
.PHONY: all
all: subinclude submake subtarget
#ifneq "$(target)" ""
#all: subinclude submake subtarget
#else
#all: subinclude submake
#endif


# submake
.PHONY: submake $(dep_prj_dir)
submake: $(dep_prj_dir)
$(dep_prj_dir):
#	@echo ##########Current SubDir $(TOP_DIR)/$@
	$(MAKE) -C $(TOP_DIR)/$@


# subinclude
.PHONY: subinclude
subinclude:
#	@echo ##########11#####dep_prj_dir
# ifneq "$(dep_prj_dir)" ""
# 	@for temp in $(dep_prj_dir);\
# 	do\
# 		usr_INCLUDE_PATH += $$temp;\
# 	done	
# endif


# subtarget
usr_objects = $(patsubst %.c,%.o,$(wildcard *.c))
usr_objects += $(patsubst %.cpp,%.o,$(wildcard *.cpp))
.PHONY: subtarget
subtarget: $(target)
$(target):$(usr_objects)
	$(CC) $(usr_LDFLAGS) $(usr_LD_LIB_PATH) -o $@ $^


# subclean
#.PHONY: subclean
#subclean: clean $(dep_prj_dir)
#$(dep_prj_dir):
#	$(MAKE) -C $(TOP_DIR)/$(var) clean

# subclean
# .PHONY: subclean
# subclean: clean
# 	$(foreach var,$(dep_prj_dir),$(call OBJ_CLEAN,$(var)))
#	$(foreach var,$(dep_prj_dir),$(MAKE) -C $(TOP_DIR)/$(var) clean)

#subclean
.PHONY: subclean
subclean: clean
ifneq "$(dep_prj_dir)" ""
	@for temp in $(dep_prj_dir) ; \
	do \
		$(MAKE) -C $(TOP_DIR)/$$temp clean; \
	done
endif


# clean
.PHONY: clean
clean:
ifneq "$(target)" ""
	-$(RM) *.d *.o $(target)
endif


# rebuild all project
.PHONY: rebuild
rebuild: subclean all









# Compiler Rules Reference Define Information

CROSS_COMPILE := arm-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)g++
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar


# User Define Flags Information,only top makefile need this code
ifeq ($(top_file),true)

ifdef DEBUG
	DEBUG_LEVEL := -g
else
	DEBUG_LEVEL :=
endif

export DEBUG_LEVEL

endif


# User compiler parameter

ifeq ($(type),share)
usr_SHARELIB_CFLAGS = -fpic
usr_SHARELIB_LDFLAGS = -shared
else
usr_SHARELIB_CFLAGS =
usr_SHARELIB_LDFLAGS = 
endif


usr_CFLAGS = $(temp_FLAGS) $(usr_SHARELIB_CFLAGS) $(temp_CFLAGS)
usr_CPPFLAGS = $(temp_FLAGS) $(usr_SHARELIB_CFLAGS) $(temp_CPPFLAGS)

#usr_INCLUDE_PATH = -I$(shell pwd) $(foreach dir,$(dep_prj_dir),-I$(TOP_DIR)/$(dir))
#usr_LDFLAGS = $(usr_SHARELIB_LDFLAGS) $(foreach var,$(dep_lib),-l$(var))
#usr_LD_LIB_PATH = -L$(shell pwd) $(foreach dir,$(dep_prj_dir),-L$(TOP_DIR)/$(dir))
usr_INCLUDE_PATH = -I. $(foreach dir,$(dep_prj_dir),-I$(TOP_DIR)/$(dir))
usr_LDFLAGS = $(usr_SHARELIB_LDFLAGS) $(foreach var,$(dep_lib),-l$(var))
usr_LD_LIB_PATH = -L. $(foreach dir,$(dep_prj_dir),-L$(TOP_DIR)/$(dir))

# Compiler Rules

%.o: %.c
	$(CC) $(DEBUG_LEVEL) $(usr_CFLAGS) $(usr_INCLUDE_PATH) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<

%.o: %.cpp
	$(CPP) $(DEBUG_LEVEL) $(usr_CPPFLAGS) $(usr_INCLUDE_PATH) -c -MD -MF $(patsubst %.o,%.d,$@) -o $@ $<

# %.d: %.c
# 	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
# 	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
# 	rm -f $@.$$$$


# multi command vaiable define,命令包

define BUILD_BINARY

endef

define BUILD_SHARELIB
	
endef

define BUILD_STATICLIB
	
endef

define OBJ_CLEAN
$(MAKE) -C $(TOP_DIR)/$1 subclean
endef
