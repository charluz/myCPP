#**********************************************************************
#
# Author : Charles Wu
#**********************************************************************

#-----------------------------------------------------------
## MinGW
#-----------------------------------------------------------
MINGW 			= D:\MINGW64\mingw64
MINGW_BIN 		= $(MINGW)\bin

MINGW_LIB		=  $(MINGW)\x86_64-w64-mingw32\lib
MINGW_LIB		+= $(MINGW)\lib
MINGW_LIB		+= $(MINGW)\lib\gcc\x86_64-w64-mingw32\lib
MINGW_LIB		+= $(MINGW)\lib\gcc\x86_64-w64-mingw32\8.1.0

MINGW_INC		= $(MINGW_LIB)\gcc\x86_64-w64-mingw32\8.1.0\include
MINGW_INC		+= $(MINGW_LIB)\gcc\x86_64-w64-mingw32\8.1.0\include-fixed
MINGW_INC		+= $(MINGW)\x86_64-w64-mingw32\include
#MINGW_INC		+= $(MINGW)\x86_64-w64-mingw32\include\c++
#MINGW_INC		+= $(MINGW)\x86_64-w64-mingw32\include\c++\backward
#MINGW_INC		+= $(MINGW)\x86_64-w64-mingw32\include\c++\x86_64-w64-mingw32

#-----------------------------------------------------------
## OpenCV V3.2
#-----------------------------------------------------------
CV32			= D:\OpenCV32\build_eclipse\install

CV32_INC		= $(CV32)\include
CV32_INC		+= $(CV32)\include\opencv2
CV32_INC		+= $(CV32)\include\core
CV32_INC		+= $(CV32)\include\core\hal

CV32_LIB		= $(CV32)\x64\mingw\lib

CV32_LDLIBS		= opencv_core320
CV32_LDLIBS		+= opencv_calib3d320
CV32_LDLIBS		+= opencv_videoio320
CV32_LDLIBS		+= opencv_video320
CV32_LDLIBS		+= opencv_features2d320
CV32_LDLIBS		+= opencv_flann320
CV32_LDLIBS		+= opencv_highgui320
CV32_LDLIBS		+= opencv_imgcodecs320
CV32_LDLIBS		+= opencv_imgproc320
CV32_LDLIBS		+= opencv_ml320
CV32_LDLIBS		+= opencv_objdetect320
CV32_LDLIBS		+= opencv_photo320
CV32_LDLIBS		+= opencv_shape320


#*************  TOOLCHAIN  Header   *************
TOOLCHAIN_INCLUDE	:= $(MINGW_INC)
TOOLCHAIN_INCLUDE	+= $(CV32_INC)

TOOLCHAIN_CFLAGS	:= $(addprefix -I,$(TOOLCHAIN_INCLUDE))

#*************  TOOLCHAIN Library   *************
TOOLCHAIN_LIB		= $(MINGW_LIB)
TOOLCHAIN_LIB		+= $(CV32_LIB)

TOOLCHAIN_LDLIBRARY	= $(CV32_LDLIBS)

TOOLCHAIN_LDFLAGS	= $(addprefix -L,$(TOOLCHAIN_LIB))
TOOLCHAIN_LDLIBS	= $(addprefix -l,$(TOOLCHAIN_LDLIBRARY))


 
#CROSS ?= none
export CROSS	:=

# C -compiler name, can be replaced by another compiler(replace gcc)
export CC		= $(CROSS)gcc
export GCPP		= $(CROSS)g++
export STRIP	= $(CROSS)strip
export LD		= $(CROSS)ld
export AR		= $(CROSS)ar
export RANLIB	= $(CROSS)ranlib


# MACRO for cleaning object -files
ifeq ($(TERM),xterm)
#-- make under Mobaxterm
export RM 		:= rm
export COPY 	:= cp
else
#-- make under Windows CMD
export RM 		:= del
export COPY 	:= copy
endif



#--- SHELL settings
ECHO := echo
PRINT_NEW_LINE = @$(ECHO) ...
PRINT_GOAL_OK = @$(ECHO) "*****  built successfully! ******"
PRINT_INSTALL_OK = @$(ECHO) "*****  install successfully! ******"

