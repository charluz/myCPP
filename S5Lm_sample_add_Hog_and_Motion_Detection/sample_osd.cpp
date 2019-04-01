#include "sample_osd.hpp"


#define MAX_ENCODE_STREAM_NUM	(IAV_STREAM_MAX_NUM_ALL)
#define MAX_OVERLAY_AREA_NUM	(MAX_NUM_OVERLAY_AREA)
#define OVERLAY_CLUT_NUM		(16)
#define OVERLAY_CLUT_SIZE		(1024)
#define OVERLAY_CLUT_OFFSET		(0)
#define OVERLAY_YUV_OFFSET		(OVERLAY_CLUT_NUM * OVERLAY_CLUT_SIZE)
#define CLUT_OUTLINE_OFFSET		(OVERLAY_CLUT_NUM / 2)

#define STRING_SIZE				(255)
#define FIXED_DIGIT_WIDTH(x)	(2 * (x))
#define TOTAL_DIGIT_NUM			(10)
#define NOT_CONVERTED			(-1)

#define UARTMSG_BYTE_SIZE	64

#ifndef LINE_SPACING
#define LINE_SPACING(x)		((x) * 3 / 2)
#endif

#ifndef ROUND_UP
#define ROUND_UP(size, align) (((size) + ((align) - 1)) & ~((align) - 1))
#endif

#ifndef ROUND_DOWN
#define ROUND_DOWN(size, align) ((size) & ~((align) - 1))
#endif

#ifndef VERIFY_STREAMID
#define VERIFY_STREAMID(x)   do {		\
			if (((x) < 0) || ((x) >= MAX_ENCODE_STREAM_NUM)) {	\
				printf ("stream id wrong %d \n", (x));			\
				return -1; 	\
			}	\
		} while(0)
#endif

#ifndef VERIFY_AREAID
#define VERIFY_AREAID(x)	do {		\
			if (((x) < 0) || ((x) >= MAX_OVERLAY_AREA_NUM)) {	\
				printf("area id wrong %d, not in range [0~3].\n", (x));	\
				return -1;	\
			}	\
		} while (0)
#endif

#ifndef VERIFY_PTR
#define VERIFY_PTR(x)	do {		\
			if ((x) == NULL) {	\
				printf("Wrong msg format, please re-draw all!\n");	\
				return -1;	\
			}	\
		} while (0)
#endif

#define	ROTATE_BIT		(0)
#define	HFLIP_BIT		(1)
#define	VFLIP_BIT		(2)
#define	SET_BIT(x)		(1 << (x))

typedef enum rotate_type_s {
	CLOCKWISE_0 = 0,
	CLOCKWISE_90 = SET_BIT(ROTATE_BIT),
	CLOCKWISE_180 = SET_BIT(HFLIP_BIT) | SET_BIT(VFLIP_BIT),
	CLOCKWISE_270 = SET_BIT(HFLIP_BIT) | SET_BIT(VFLIP_BIT) | SET_BIT(ROTATE_BIT),
	AUTO_ROTATE,
} rotate_type_t;

typedef struct time_display_s {
	int enable;
	char time_string[STRING_SIZE];
	int osd_x[STRING_SIZE];	/* Digit's offset x to the osd */
	int osd_y[STRING_SIZE];	/* Digit's offset y to the osd */
	int max_width;					/* Maximum width of 10 digits as bitmap
									 * width in time display string. */
	u8 *digits_addr;				/* Used to store 10 digits' bitmap data */
	int digit_width;				/* One digit's width in digits_addr */
	int digit_height;				/* One digit's height */
} time_display_t;

typedef struct textbox_s {
	int enable;
	u16 x;
	u16 y;
	u16 width;
	u16 height;
	int color;
	int font_type;
	int font_size;
	int outline;
	int bold;
	int italic;
	int line_thickness; // Used to draw hollow box in uart
	rotate_type_t rotate;
	wchar_t content[STRING_SIZE];
	time_display_t time;
} textbox_t;

typedef struct stream_text_info_s {
	int enable;
	int win_width;
	int win_height;
	rotate_type_t rotate;
	textbox_t textbox[MAX_OVERLAY_AREA_NUM];
} stream_text_info_t;

typedef struct clut_s {
	u8 v;
	u8 u;
	u8 y;
	u8 alpha;
} clut_t;

typedef struct buffer_info_s {
	u16 buf_width;
	u16 buf_height;
	u16 sub_x;
	u16 sub_y;
	u16 sub_width;
	u16 sub_height;
	u8 *buf_base;
} buffer_info_t;


#define COLOR_NUM		8
static clut_t clut[COLOR_NUM];
//static clut_t clut[COLOR_NUM] = {
	/* alpha 0 is full transparent */
	//{ .y = 82, .u = 90, .v = 240, .alpha = 255 },	/* red */
	//{ .y = 41, .u = 240, .v = 110, .alpha = 255 },	/* blue */
	//{ .y = 12, .u = 128, .v = 128, .alpha = 255 },	/* black */
	//{ .y = 235, .u = 128, .v = 128, .alpha = 255 },	/* white */
	//{ .y = 145, .u = 54, .v = 34, .alpha = 255 },	/* green */
	//{ .y = 210, .u = 16, .v = 146, .alpha = 255 },	/* yellow */
	//{ .y = 170, .u = 166, .v = 16, .alpha = 255 },	/* cyan */
	//{ .y = 107, .u = 202, .v = 222, .alpha = 255 }, /* magenta */

//};


enum font_type {
	FONT_TYPE_ENGLISH		= 0,
	FONT_TYPE_CHINESE		= 1,
	FONT_TYPE_NUM
};

typedef struct pixel_type_s {
	u8 pixel_background;
	u8 pixel_font;
	u8 pixel_outline;
	u8 reserved;
} pixel_type_t;

pixel_type_t pixel_type;
//pixel_type_t pixel_type = {
//	.pixel_background = 30,
//	.pixel_outline = 1,
//	.pixel_font = 255,
//};

enum {
	SPECIFY_OUTLINE = 130,
	SPECIFY_BOLD,
	SPECIFY_ITALIC,
	SPECIFY_ROTATE,
	SPECIFY_STRING,
	TIME_DISPLAY,
	UART_CONTROL,
};


int fd_overlay;
int overlay_yuv_size;
u8 *overlay_clut_addr;
static stream_text_info_t stream_text_info[MAX_ENCODE_STREAM_NUM];
static struct iav_overlay_insert overlay_insert[MAX_ENCODE_STREAM_NUM];
static int flag_time_display = 0;
static int flag_uart_control = 0;



static int map_overlay(void)
{
	struct iav_querybuf querybuf;
	u8* addr = NULL;

	querybuf.buf = IAV_BUFFER_OVERLAY;
	if (ioctl(fd_overlay, IAV_IOC_QUERY_BUF, &querybuf) < 0) {
		printf("IAV_IOC_QUERY_BUF: OVERLAY\n");
		return -1;
	}

	addr = (u8*)mmap(NULL, querybuf.length,
							PROT_WRITE, MAP_SHARED,
							fd_overlay, querybuf.offset);
	if (addr == MAP_FAILED) {
		printf("mmap OVERLAY failed\n");
		return -1;
	}

	printf("\noverlay: start = %p, total size = 0x%lx ( bytes)\n",addr, querybuf.length);
	overlay_clut_addr = addr + OVERLAY_CLUT_OFFSET;
	overlay_yuv_size = (querybuf.length - OVERLAY_YUV_OFFSET)/MAX_ENCODE_STREAM_NUM;

	return 0;
}


static int set_overlay_config(void)
{

	int i, j, win_width, win_height, total_size;
	u32 overlay_data_offset;
	struct iav_overlay_area *area;
	textbox_t *box;

	for (i = 0; i < MAX_ENCODE_STREAM_NUM; i++) {
		if (stream_text_info[i].enable) {
			overlay_data_offset = OVERLAY_YUV_OFFSET + overlay_yuv_size * i;
			win_width = stream_text_info[i].win_width;
			win_height = stream_text_info[i].win_height;
			for (j = 0, total_size = 0; j < MAX_OVERLAY_AREA_NUM; j++) {
				area = &overlay_insert[i].area[j];
				box = &stream_text_info[i].textbox[j];

				if (box->enable) {
					if (stream_text_info[i].rotate &
							SET_BIT(ROTATE_BIT)) {
						area->width = box->height = ROUND_UP(box->height, 32);
						area->height = box->width = ROUND_UP(box->width, 4);
					} else {
						area->width = box->width = ROUND_UP(box->width, 32);
						area->height = box->height = ROUND_UP(box->height, 4);
					}
					switch (stream_text_info[i].rotate) {
					case CLOCKWISE_0:
						area->start_x = box->x = ROUND_DOWN(box->x, 2);
						area->start_y = box->y = ROUND_DOWN(box->y, 4);
						break;
					case CLOCKWISE_90:
						area->start_x = box->y = ROUND_DOWN(box->y, 2);
						area->start_y = ROUND_DOWN(
								win_width - box->x - box->width, 4);
						box->x = win_width - box->width - area->start_y;
						break;
					case CLOCKWISE_180:
						area->start_x = ROUND_DOWN(
								win_width - box->x - box->width, 2);
						area->start_y = ROUND_DOWN(
								win_height - box->y - box->height, 4);
						box->x = win_width - box->width - area->start_x;
						box->y = win_height - box->height -  area->start_y;
						break;
					case CLOCKWISE_270:
						area->start_x = ROUND_DOWN(
								win_height - box->y - box->height, 2);
						area->start_y = box->x = ROUND_DOWN(box->x, 4);
						box->y = win_height - box->height -  area->start_x;
						break;
					default:
						printf("unknown rotate type\n");
						return -1;
					}

					area->pitch = area->width;
					area->enable = 1;
					area->total_size = area->pitch * area->height;
					area->clut_addr_offset = (box->outline == 0 ? box->color :
							box->color + CLUT_OUTLINE_OFFSET) * OVERLAY_CLUT_SIZE;
					area->data_addr_offset = overlay_data_offset + total_size;

					printf("Stream %c Area %d [x %d, y %d, w %d, h %d,"
							" size %d],clut_addr_offset:%d,data_addr_offset:%d\n", 'A' + i, j, area->start_x,
							area->start_y, area->width, area->height,
							area->total_size,area->clut_addr_offset,area->data_addr_offset);

					total_size += area->total_size;
					if (total_size > overlay_yuv_size) {
						printf("The total OSD size is %d (should be <= %d).\n",
								total_size, overlay_yuv_size);
						return -1;
					}
				}
			}
		}
	}
	return 0;

}

static int fill_overlay_clut(void)
{
	clut_t * clut_data = (clut_t *)overlay_clut_addr;
	int i, j, outline_clut;

	for (i = 0; i < CLUT_OUTLINE_OFFSET && i < sizeof(clut); i++) {
		/* CLUT (0 ~ CLUT_OUTLINE_OFFSET) is for non outline font.*/
		for (j = 0; j < 256; j++) {
			clut_data[(i << 8) + j].y = 82;//clut[i].y;
			clut_data[(i << 8) + j].u = 90;//clut[i].u;
			clut_data[(i << 8) + j].v = 240;//clut[i].v;
			clut_data[(i << 8) + j].alpha = j;
		}

		/* CLUT (CLUT_OUTLINE_OFFSET ~ OVERLAY_CLUT_NUM) is outline font.*/
		outline_clut = i + CLUT_OUTLINE_OFFSET;
		for (j = 0; j < 256; j++) {
			clut_data[(outline_clut << 8) + j].y = 82;//clut[i].y;
			clut_data[(outline_clut << 8) + j].u = 90;//clut[i].u;
			clut_data[(outline_clut << 8) + j].v = 240;//clut[i].v;
			clut_data[(outline_clut << 8) + j].alpha = j;
		}
		clut_data[(outline_clut << 8) + pixel_type.pixel_outline].y = (
				clut[i].y > 128 ? 16 : 235);
		clut_data[(outline_clut << 8) + pixel_type.pixel_outline].u = 128;
		clut_data[(outline_clut << 8) + pixel_type.pixel_outline].v = 128;
		clut_data[(outline_clut << 8) + pixel_type.pixel_outline].alpha = 255;

	}

	return 0;
}

static void fill_overlay_data(const buffer_info_t *src, buffer_info_t *dst,
                              rotate_type_t rotate)
{
	u8 *sp, *dp, *src_base, *dst_base;
	int col, row;
	const u16 width = src->sub_width, height = src->buf_height;
	const u16 src_width = src->buf_width, src_height = src->buf_height;
	const u16 dst_width = dst->buf_width, dst_height = dst->buf_height;
	const u16 sx = src->sub_x, sy = src->sub_y;
	const u16 dx = dst->sub_x, dy = dst->sub_y;
	src_base = src->buf_base;
	dst_base = dst->buf_base;

	if (sx + width > src_width || sy + height > src_height) {
		printf("fill_overlay_data error.\n");
		return;
	}

	sp = src_base + sy * src_width + sx;

	switch (rotate) {
	case CLOCKWISE_0:
		dp = dst_base + dy * dst_width + dx;
		for (row = 0; row < height; row++) {
			memcpy(dp, sp, width);
			sp += src_width;
			dp += dst_width;
		}
		break;
	case CLOCKWISE_90:
		dp = dst_base + (dst_height - dx - 1) * dst_width + dy;
		for (row = 0; row < height; row++) {
			for (col = 0; col < width; col++) {
				*(dp - col * dst_width) = *(sp + col);
			}
			sp += src_width;
			dp++;
		}
		break;
	case CLOCKWISE_180:
		dp = dst_base + (dst_height - dy - 1) * dst_width + dst_width - dx - 1;
		for (row = 0; row < height; row++) {
			for (col = 0; col < width; col++) {
				*(dp - col) = *(sp + col);
			}
			sp += src_width;
			dp -= dst_width;
		}
		break;
	case CLOCKWISE_270:
		dp = dst_base + dx * dst_width + dst_width - dy - 1;
		for (row = 0; row < height; row++) {
			for (col = 0; col < width; col++) {
				*(dp + col * dst_width) = *(sp + col);
			}
			sp += src_width;
			dp--;
		}
		break;
	default:
		printf("Unknown rotate type.\n");
		break;
	}

	if (rotate & SET_BIT(ROTATE_BIT)) {
		dst->sub_width = src->sub_height;
		dst->sub_height = src->sub_width;
	} else {
		dst->sub_width = src->sub_width;
		dst->sub_height = src->sub_height;
	}
}

static int text_string_convert(int stream_id, int area_id)
{
	int i, remain_height, offset_x, offset_y;
	buffer_info_t boxbuf, overlay;
	u8 *line_addr, *boxbuf_addr;
	//bitmap_info_t bitmap_info;
	wchar_t *p;
	struct iav_overlay_area *area = &overlay_insert[stream_id].area[area_id];
	textbox_t *box = &stream_text_info[stream_id].textbox[area_id];
	const u16 line_width = box->width;
	const u16 line_height = LINE_SPACING(box->font_size);

	if (NULL == (boxbuf_addr = (u8 *)malloc(area->total_size))) {
		perror("text_string_convert: malloc\n");
		return -1;
	}
	//memset(boxbuf_addr, pixel_type.pixel_background, area->total_size);
	memset(boxbuf_addr, 128, area->total_size);

	offset_x = offset_y = 0;
	line_addr = boxbuf_addr;

#if 0
	for (i = 0, p = box->content;i < wcslen(box->content); i++, p++) {
		if (offset_x + box->font_size > line_width) {
			/*
			 * If there is enough memory for more lines, try to write on a new
			 * line. Otherwise, stop converting.
			 */
			offset_y += line_height;
			remain_height = box->height - offset_y;
			if (remain_height < line_height)
				break;
			line_addr += line_width * line_height;
			offset_x = 0;
		}

		/* Remember the offset of letters in time string */
		if (box->time.enable) {
			box->time.osd_x[i] = offset_x;
			box->time.osd_y[i] = offset_y;
		}
		//if (text2bitmap_convert_character(*p, line_addr, line_height,
		//		line_width, offset_x, &bitmap_info) < 0) {
		//	free(boxbuf_addr);
		//	return -1;
		//}
		if (box->time.enable && isdigit(*p)) {
			/*
			 * If the character is a digit in time string, use the max digit
			 * bitmap width as horizontal offset.
			 */
			offset_x += box->time.max_width;
		} else {
			offset_x += bitmap_info.width;
		}
	}
#endif

	overlay.buf_base = overlay_clut_addr + area->data_addr_offset;
	overlay.buf_height = area->height;
	overlay.buf_width = area->width;
	overlay.sub_x = overlay.sub_y = 0;
	boxbuf.buf_base = boxbuf_addr;
	boxbuf.buf_height = boxbuf.sub_height = box->height;
	boxbuf.buf_width = boxbuf.sub_width = box->width;
	boxbuf.sub_x = boxbuf.sub_y = 0;
	fill_overlay_data(&boxbuf, &overlay, stream_text_info[stream_id].rotate);
	free(boxbuf_addr);
	return 0;
}


static int text_osd_insert(int stream_id)
{
	int area_id;

#if 1
	for (area_id = 0; area_id  < MAX_OVERLAY_AREA_NUM; area_id ++) {
		if (stream_text_info[stream_id].textbox[area_id].enable) {
			// Set font attribute
			//if (text_set_font(stream_id, area_id) < 0)
			//	return -1;
			// Convert 10 digits for later reuse.
			//if (stream_text_info[stream_id].textbox[area_id].time.enable) {
			//	if (digits_convert(stream_id, area_id) < 0)
			//		return -1;
			//}
			// Convert string and fill the overlay data.
			if (text_string_convert(stream_id, area_id) < 0)
				return -1;
		}
	}
#endif


	overlay_insert[stream_id].enable = 1;
	overlay_insert[stream_id].id = stream_id;

	if (ioctl(fd_overlay, IAV_IOC_SET_OVERLAY_INSERT,
			&overlay_insert[stream_id]) < 0) {
		perror("IAV_IOC_SET_OVERLAY_INSERT");
		return -1;
	}

	return 0;
}

void sample_osd_main_flow_init(void){

	int stream_id = 0;
	int i = 0;
	int area_id = 1;


	stream_text_info[stream_id].enable = 1;
	stream_text_info[stream_id].textbox[0].enable = 0;
	stream_text_info[stream_id].textbox[0].x = 0;
	stream_text_info[stream_id].textbox[0].y = 0;
	stream_text_info[stream_id].textbox[0].width = 150;
	stream_text_info[stream_id].textbox[0].height = 150;
	stream_text_info[stream_id].textbox[0].font_size = 50; //i'dont know how use it

	stream_text_info[stream_id].textbox[1].enable = 0;
	stream_text_info[stream_id].textbox[1].x = 200;
	stream_text_info[stream_id].textbox[1].y = 0;
	stream_text_info[stream_id].textbox[1].width = 150;
	stream_text_info[stream_id].textbox[1].height = 150;
	stream_text_info[stream_id].textbox[1].font_size = 50;

	stream_text_info[stream_id].textbox[2].enable = 0;
	stream_text_info[stream_id].textbox[2].x = 400;
	stream_text_info[stream_id].textbox[2].y = 0;
	stream_text_info[stream_id].textbox[2].width = 150;
	stream_text_info[stream_id].textbox[2].height = 150;
	stream_text_info[stream_id].textbox[2].font_size = 50;

	stream_text_info[stream_id].textbox[3].enable = 0;
	stream_text_info[stream_id].textbox[3].x = 600;
	stream_text_info[stream_id].textbox[3].y = 0;
	stream_text_info[stream_id].textbox[3].width = 150;
	stream_text_info[stream_id].textbox[3].height = 150;
	stream_text_info[stream_id].textbox[3].font_size = 50;

	stream_text_info[stream_id].textbox[4].enable = 0;
	stream_text_info[stream_id].textbox[4].x = 800;
	stream_text_info[stream_id].textbox[4].y = 0;
	stream_text_info[stream_id].textbox[4].width = 150;
	stream_text_info[stream_id].textbox[4].height = 150;
	stream_text_info[stream_id].textbox[4].font_size = 50;

	stream_text_info[stream_id].textbox[5].enable = 0;
	stream_text_info[stream_id].textbox[5].x = 0;
	stream_text_info[stream_id].textbox[5].y = 200;
	stream_text_info[stream_id].textbox[5].width = 150;
	stream_text_info[stream_id].textbox[5].height = 150;
	stream_text_info[stream_id].textbox[5].font_size = 50;

	stream_text_info[stream_id].textbox[6].enable = 0;
	stream_text_info[stream_id].textbox[6].x = 0;
	stream_text_info[stream_id].textbox[6].y = 400;
	stream_text_info[stream_id].textbox[6].width = 150;
	stream_text_info[stream_id].textbox[6].height = 150;
	stream_text_info[stream_id].textbox[6].font_size = 50;

	stream_text_info[stream_id].textbox[7].enable = 0;
	stream_text_info[stream_id].textbox[7].x = 0;
	stream_text_info[stream_id].textbox[7].y = 600;
	stream_text_info[stream_id].textbox[7].width = 150;
	stream_text_info[stream_id].textbox[7].height = 150;
	stream_text_info[stream_id].textbox[7].font_size = 50;

	stream_text_info[stream_id].textbox[8].enable = 0;
	stream_text_info[stream_id].textbox[8].x = 0;
	stream_text_info[stream_id].textbox[8].y = 800;
	stream_text_info[stream_id].textbox[8].width = 150;
	stream_text_info[stream_id].textbox[8].height = 150;
	stream_text_info[stream_id].textbox[8].font_size = 50;

	stream_text_info[stream_id].textbox[9].enable = 0;
	stream_text_info[stream_id].textbox[9].x = 600;
	stream_text_info[stream_id].textbox[9].y = 600;
	stream_text_info[stream_id].textbox[9].width = 150;
	stream_text_info[stream_id].textbox[9].height = 150;
	stream_text_info[stream_id].textbox[9].font_size = 50;

	if ((fd_overlay = open("/dev/iav", O_RDWR, 0)) < 0) {
			printf("/dev/iav");
		}


	if (map_overlay() < 0)
		printf("map_overlay error !! \n");

	if (set_overlay_config() < 0)
		printf("set_overlay_config error !! \n");

	if (fill_overlay_clut() < 0)
		printf("fill_overlay_clut error !! \n");

	for (i = 0; i < MAX_ENCODE_STREAM_NUM; ++i) {
		if (stream_text_info[i].enable) {
			if (text_osd_insert(i) < 0) {
				printf("Text insert for stream %d error!\n", i);
				//goto TEXTINSERT_EXIT;
			}
		}
	}


}

void sample_osd_clear_all(void){
	for(int i =0;i<MAX_OVERLAY_AREA_NUM;i++){
		sample_osd_enable(i,0,0,0);
	}
}

void sample_osd_enable(int area_index,u16 x,u16 y,u16 enable){

	//printf("sample_osd_enable (x,y)(%d %d)\n",x,y);

	int stream_id = 0;
	stream_text_info[stream_id].textbox[area_index].enable = enable;
	stream_text_info[stream_id].textbox[area_index].x = x;
	stream_text_info[stream_id].textbox[area_index].y = y;


	overlay_insert[stream_id].area[area_index].start_x = x;
	overlay_insert[stream_id].area[area_index].start_y = y;

	overlay_insert[stream_id].area[area_index].enable = enable;

	overlay_insert[stream_id].enable = 1;
	overlay_insert[stream_id].id = stream_id;

}

int sample_osd_main_flow(void){
	int i = 0;
	int stream_id = 0;
	int area_id = 1;



	if (set_overlay_config() < 0)
		printf("set_overlay_config error !! \n");

	if (fill_overlay_clut() < 0)
		printf("fill_overlay_clut error !! \n");

	for (i = 0; i < MAX_ENCODE_STREAM_NUM; ++i) {
		if (stream_text_info[i].enable) {
			if (text_osd_insert(i) < 0) {
				printf("Text insert for stream %d error!\n", i);
				//goto TEXTINSERT_EXIT;
			}
		}
	}

	printf("apply Osd Roi ...\n");


}
