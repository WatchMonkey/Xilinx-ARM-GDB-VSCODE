#
#	Author:Peji
#	Date:2023-04-19
#	Descriptor:top level  makefile
#	Detail:
#		this file was used by user and try to start operation that compiler target binary file
#



# 
# user variable define
#
#工程目录的顶层makefile文件
top_file := false

#工程目录编译生成目标的类型 binary static share
type := share

#工程目录下的子工程目录名称
dep_prj_dir := libLog

#当前编译工程的依赖库文件
dep_lib := LogOp pthread

#当前编译工程的目标文件,只能够有一个目标
target := ThreadOp

#当前编译工程的需要使用的编译工具的编译选项
#temp_FLAGS = -fpermissive



# User Define Flags Information,only top makefile need this code
ifeq ($(top_file),true)

# check current shell type,if cygwin so need change dir path format
ifneq "$(findstring CYGWIN_NT,$(shell uname))" ""
TOP_DIR = $(shell cygpath -m $(PWD))
else

# check current shell type,if mingw so need change dir path format
ifneq "$(findstring MINGW64_NT,$(shell uname))" ""
TOP_DIR = $(shell cygpath -m $(PWD))
else
TOP_DIR = $(shell pwd)
endif

endif

export TOP_DIR
$(info TOP_DIR=$(TOP_DIR))

endif

-include $(TOP_DIR)/base.mk