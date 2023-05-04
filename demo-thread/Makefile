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
top_file = false

#工程目录编译生成目标的类型 binary static share
type := binary

#被当前工程所依赖的工程 的 工程目录下的目录名称
dep_prj_dir = libLog libThread

#当前编译工程的依赖库文件
dep_lib = LogOp ThreadOp stdc++

#当前编译工程的目标文件
target := demoThread




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