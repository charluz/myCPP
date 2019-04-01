#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <sched.h>
//#include <linux/fb.h>
//#include <sys/mman.h>
//#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <assert.h>

#include <basetypes.h>
#include "iav_ioctl.h"
#include "datatx_lib.h"
#include <signal.h>

#include <iostream>

int sample_osd_main_flow(void);
void sample_osd_main_flow_init(void);
void sample_osd_enable(int area_index,u16 x,u16 y,u16 enable);
void sample_osd_clear_all(void);

