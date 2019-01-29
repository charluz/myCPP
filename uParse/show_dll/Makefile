#
# To build customized report DLL for uParser
#

#=== Project programs
PROJ = my_show
EXPDLL = $(PROJ).dll
IMPLIB = $(PROJ).a
GOAL = $(EXPDLL)

SRCS = show_dll.c

OBJS = $(SRCS:.c=.o)
	# 把 $SRCS 裡所有 .c 都換成 .o

#=== Compiler settings
CC      = gcc
CFLAGS  = -O2 -Wall -c -DMINGW_DLL
SHAREFLAGS = -shared -Wl,--out-implib,$(IMPLIB) -o $(EXPDLL)


#=== Ar settings to build the library
AR	    = ar
ARFLAGS = rcv

#=== Implicit rules
#.SUFFIXES = .o .c .h .a

COMPILE.c=$(CC) $(CFLAGS) -c

.c.o:
	@(echo "")
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)


$(GOAL) : $(OBJS)
	@(echo "")
	@(echo "building dll $@ ...")
	$(CC) $(SHAREFLAGS) $(OBJS)
	@(echo "")
	@(echo "....... $@ OK !...");
#說明：$@ 代表 target，在這裡就是 $(GOAL)


.PHONY: clean veryclean check docs
clean:
	$(RM) $(OBJS) $(GOAL)

veryclean:
	$(RM) $(OBJS) $(GOAL)
	$(RM) dump_conf.txt

docs:
	@(cd doc ; $(MAKE))

check:
	@(cd test ; $(MAKE))
