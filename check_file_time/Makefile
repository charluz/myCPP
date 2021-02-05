#
# My Makefile for MingGW64 project.
#

#=== Compiler settings
#TOOLCHAIN = /mingw64/bin
# CC      = $(TOOLCHAIN)/gcc
CC		= gcc
# CFLAGS  = -O2 -Wall
CFLAGS  = -O2 -Wall
CFLAGS	+= -Wno-discarded-qualifiers
CFLAGS	+= -Wno-pointer-sign
CFLAGS	+= -Wno-stringop-overflow
CFLAGS	+= -Wno-unused-function
CFLAGS	+= -Wno-unused-variable
CFLAGS	+= -Wno-unused-but-set-variable


#=== Ar settings to build the library
# AR	    = $(TOOLCHAIN)/ar
# ARFLAGS = rcsv

#=== Implicit rules
#.SUFFIXES = .o .c .h .a

COMPILE.c=$(CC) $(CFLAGS) -c

.c.o:
	@(echo "")
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)


#=== Project programs
GOAL = check_ftime.exe

SRCS = main.c

OBJS = $(SRCS:.c=.o)
	# �� $SRCS �̩Ҧ� .c ������ .o

$(GOAL) : $(OBJS)
	@(echo "")
	@(echo "linking $@ ...")
	$(CC) -o $(GOAL) $(OBJS)
	@(echo "")
	@(echo "....... $@ OK !...");
	@(echo "")


.PHONY: clean veryclean check docs touch
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
