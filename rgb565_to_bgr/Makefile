#
# My Makefile for MingGW32 project.
#

RM = del

#=== Compiler settings
#TOOLCHAIN = /c/Mingw64/mingw64/bin
TOOLCHAIN = 
CC      = $(TOOLCHAIN)gcc
CFLAGS  = -O2 -Wall


#=== Ar settings to build the library
AR	    = $(TOOLCHAIN)ar
ARFLAGS = rcsv

#=== Implicit rules
#.SUFFIXES = .o .c .h .a

COMPILE.c=$(CC) $(CFLAGS) -c

%o:%c
	@(echo "--- a ---")
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)


#=== Project programs
GOAL = rgb565_to_bgr.exe

SRCS = main.c bin2bmp.c

OBJS = $(SRCS:.c=.o)

all: $(GOAL)
	@echo "***** Building $(GOAL) completes successfully! ***"

$(GOAL) : $(OBJS)
#	@echo "---- GOAL --- "
	@echo "linking $@ ..."
	$(CC) -o $(GOAL) $(OBJS)
	@echo
	@echo "....... $@ OK !...";
	@echo


.PHONY: clean veryclean check docs touch $(GOAL)
touch:
	touch $(GOAL)

clean:
	$(RM) $(OBJS) $(GOAL) $(GOALLIB)

veryclean:
	$(RM) $(OBJS) $(GOAL) $(GOALLIB)
	$(RM) dump_conf.txt

docs:
	@(cd doc ; $(MAKE))

check:
	@(cd test ; $(MAKE))
