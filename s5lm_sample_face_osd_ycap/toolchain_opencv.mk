#**********************************************************************
# Note : This file shall be inclued in Makefile following toolchain.mk !!
# Author : Charles Wu
#**********************************************************************



#-----------------------------------------------------------
## OpenCV V3.2
#-----------------------------------------------------------
PREBUILD_ARCH	:= $(SDK_AMBA_ROOT)/prebuild/third-party/armv8-a
CV32_ROOT		= $(PREBUILD_ARCH)/opencv

CV32_INCLUDE	= 
CV32_INCLUDE	+= $(CV32_ROOT)/include
#CV32_INCLUDE	+= $(CV32_ROOT)/include/opencv2
#CV32_INCLUDE	+= $(CV32_ROOT)/include/opencv

#CV32_INCLUDE	+= $(CV32_ROOT)/include/core
#CV32_INCLUDE	+= $(CV32_ROOT)/include/core/hal


CV32_LIB		= $(CV32_ROOT)/usr/lib

CV32_LDLIBS		= opencv_core
CV32_LDLIBS		+= opencv_calib3d
CV32_LDLIBS		+= opencv_videoio
CV32_LDLIBS		+= opencv_video
CV32_LDLIBS		+= opencv_features2d
CV32_LDLIBS		+= opencv_flann
CV32_LDLIBS		+= opencv_highgui
CV32_LDLIBS		+= opencv_imgcodecs
CV32_LDLIBS		+= opencv_imgproc
CV32_LDLIBS		+= opencv_ml
CV32_LDLIBS		+= opencv_objdetect
CV32_LDLIBS		+= opencv_photo
CV32_LDLIBS		+= opencv_shape


#*************  OpenCV  Header   *************
TOOLCHAIN_INCLUDE	+= $(CV32_INCLUDE)


#*************  OpenCV Library   *************
TOOLCHAIN_LIB		+= $(CV32_LIB)
TOOLCHAIN_LDLIBRARY	+= $(CV32_LDLIBS)

TOOLCHAIN_LDFLAGS	+= -Wl,-rpath-link=$(CV32_LIB) \
                        -Wl,-rpath-link=$(PREBUILD_ARCH)/libjpeg-turbo/usr/lib \
                        -Wl,-rpath-link=$(PREBUILD_ARCH)/libpng/usr/lib \
                        -Wl,-rpath-link=$(PREBUILD_ARCH)/zlib/usr/lib



