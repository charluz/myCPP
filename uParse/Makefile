#
# My Makefile for MingGW32 project.
#

#=== Compiler settings
TOOLCHAIN = /c/MinGW/bin
CC      = $(TOOLCHAIN)/gcc
CFLAGS  = -O2 -Wall


#=== Ar settings to build the library
AR	    = $(TOOLCHAIN)/ar
ARFLAGS = rcsv

#=== Implicit rules
#.SUFFIXES = .o .c .h .a

COMPILE.c=$(CC) $(CFLAGS) -c

.c.o:
	@(echo "")
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)


#=== Project programs
GOAL = uParse.exe
GOALLIB = uparse.lib

SRCS = main.c \
	   prog_arg.c \
	   prog_dbg.c \
	   myut_llist.c \
	   myut_string.c \
	   myut_numeric.c \
	   plugin_service.c

OBJS = $(SRCS:.c=.o)
	# 把 $SRCS 裡所有 .c 都換成 .o

$(GOAL) : $(OBJS)
	@(echo "")
	@(echo "linking $@ ...")
	$(CC) -o $(GOAL) $(OBJS)
	@(echo "")
	@(echo "....... $@ OK !...");
	@(echo "")
	@(echo "building library $(GOALLIB) ...")
	$(AR) $(ARFLAGS) $(GOALLIB) $(OBJS)
	@(echo "")
	@(echo "....... $(GOALLIB) OK !...");


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
