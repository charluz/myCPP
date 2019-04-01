/**
    Theme: Face Detection
    Compiler: Dev C++ 4.9.9.2
    Date: 100/04/26
    Author: ShengWen
    Blog: https://cg2010studio.wordpress.com/
*/


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
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <assert.h>


#include <basetypes.h>
#include "iav_ioctl.h"
#include "datatx_lib.h"
#include <signal.h>

#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
//#include "opencv2/cv.hpp"
#include "sample_osd.hpp"
#include <iostream>

#define	DETECT_SHM_NAME	"/shm_detect"

using namespace std;
using namespace cv;

typedef struct shm_cv_detect {
	unsigned	sample_motion_state;// for sample motion detection
} shm_cv_detect_t;


void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    /*CascadeClassifier& nestedCascade,*/
                    double scale, bool tryflip );

void sample_hog_human_detection( Mat& img);
void sample_motion_detectionn( Mat& img);

// the minimum object size
int min_face_height = 50;
int min_face_width = 50;
int fd_iav;
static void *dsp_mem = NULL;
static u32 dsp_size = 0;
static u32 dsp_buf_mapped = 0;
static void* dsp_user_buf = NULL;
static u32 dsp_user_buf_size = 0;
static int G_multi_vin_num = 1;
static int G_setting_canvas_flag = 0;
static int yuv_buffer_id = 0;
static int frame_count = 1;
static int non_block_read = 0;
static void* dsp_canvas_yuv_buf_mem[IAV_MAX_CANVAS_BUF_NUM];
static void* dsp_canvas_me_buf_mem[IAV_MAX_CANVAS_BUF_NUM];
static int dump_canvas_map = 0;
static int g_motion_state = 0;

#ifndef ROUND_UP
#define ROUND_UP(size, align) (((size) + ((align) - 1)) & ~((align) - 1))
#endif

#define MB_UNIT (16)
#define MAX_YUV_BUFFER_SIZE		(4096*3000)		// 4096x3000
#define MAX_ME_BUFFER_SIZE		(MAX_YUV_BUFFER_SIZE / 16)	// 1/16 of 4096x3000
#define MAX_FRAME_NUM

static int check_state(void)
{
	int state;
	if (ioctl(fd_iav, IAV_IOC_GET_IAV_STATE, &state) < 0) {
		perror("IAV_IOC_GET_IAV_STATE");
		exit(2);
	}

	if ((state != IAV_STATE_PREVIEW) && state != IAV_STATE_ENCODING) {
		printf("IAV is not in preview / encoding state, cannot get yuv buf!\n");
		return -1;
	}

	//printf("IAV_IOC_GET_IAV_STATE state =%d \n",state);

	return 0;
}

static int map_dsp_buffer(void)
{
	struct iav_querybuf querybuf ;

	querybuf.buf = IAV_BUFFER_DSP;
	if (ioctl(fd_iav, IAV_IOC_QUERY_BUF, &querybuf) < 0) {
		printf("IAV_IOC_QUERY_BUF");
		return -1;
	}

	dsp_size = querybuf.length;
	dsp_mem = mmap(NULL, dsp_size, PROT_READ, MAP_SHARED, fd_iav,
		querybuf.offset);
	if (dsp_mem == MAP_FAILED) {
		printf("mmap (%d) failed: %s\n");
		return -1;
	}
	dsp_buf_mapped = 1;

	querybuf.buf = IAV_BUFFER_USR;
	if (ioctl(fd_iav, IAV_IOC_QUERY_BUF, &querybuf) < 0) {
		printf("IAV_IOC_QUERY_BUF");
		return -1;
	}
	dsp_user_buf_size = querybuf.length;

	//printf("memory buffer ->dsp_user_buf_size = %d \n",dsp_user_buf_size);
	if (dsp_user_buf_size) {
		printf("user buffer size(0x%x) > 0, GDMA will be used\n", dsp_user_buf_size);
		dsp_user_buf = mmap(NULL, dsp_user_buf_size, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd_iav, querybuf.offset);

		printf("mmap OPEN!!\n");

		if (dsp_user_buf == MAP_FAILED) {
			printf("mmap IAV_BUFFER_USR failed\n");
			return -1;
		}
	}

	return 0;
}

static int map_buffer(void)
{
	int ret = 0;

	ret = map_dsp_buffer();

	//printf("mmap buffer it's ok ! \n");

	return ret;
}

static int get_multi_vin_num(void)
{
	struct iav_sys_res_generic system_resource;

	// system resource
	memset(&system_resource, 0, sizeof(struct iav_sys_res_generic));
	system_resource.encode_mode = DSP_CURRENT_MODE;
	if (ioctl(fd_iav, IAV_IOC_GET_SYSTEM_RESOURCE_GENERIC, &system_resource) < 0) {
		perror("IAV_IOC_GET_SYSTEM_RESOURCE_GENERIC\n");
		return -1;
	}

	G_multi_vin_num = system_resource.chan_num;
	G_setting_canvas_flag = system_resource.canvas_enable_flag;

	//printf("G_multi_vin_num = %d \n",G_multi_vin_num);
	//printf("G_setting_canvas_flag = %d \n",G_setting_canvas_flag);

	return 0;
}

static void* get_buffer_base(int buf_id, int me_flag)
{
	void* addr = NULL;

	if (buf_id < 0 || buf_id >= IAV_MAX_CANVAS_BUF_NUM) {
		printf("Invaild canvas buf ID %d!\n", buf_id);
		return NULL;
	}
	if (dump_canvas_map & (1 << buf_id)) {
		printf("addr ## dsp_canvas_yuv_buf_mem[buf_id]\n");
		if (me_flag) {
			addr = dsp_canvas_me_buf_mem[buf_id];
		} else {
			addr = dsp_canvas_yuv_buf_mem[buf_id];
		}
	} else {
		printf("addr ## dsp_mem\n");
		addr = dsp_mem;
	}
	return addr;
}

Mat img1(720, 1280, CV_8U, Scalar(100));
static int save_yuv_luma_buffer(void* output, struct iav_yuvbufdesc *yuv_desc)
{
	int i;
	void *in = NULL;
	void *out = NULL;
	void *base = NULL, *y_addr = NULL;
	unsigned char *buff = NULL;

	if (yuv_desc->pitch < yuv_desc->width) {
		printf("pitch size smaller than width!\n");
		return -1;
	}

	if (dsp_user_buf_size && dsp_buf_mapped) {
		base = dsp_user_buf;
		y_addr = base;
	} else {
		base = get_buffer_base(yuv_desc->buf_id, 0);
		y_addr = base + yuv_desc->y_addr_offset;
	}

	if (base == NULL) {
		printf("Invalid buffer address for buffer %d YUV!"
			" Map YUV buffer from DSP first.\n", yuv_desc->buf_id);
		return -1;
	}

	if (yuv_desc->pitch == yuv_desc->width) {
		memcpy(output, y_addr, yuv_desc->width * yuv_desc->height);
	} else {
		in = y_addr;
		out = output;
		for (i = 0; i < yuv_desc->height; i++) {		//row
			memcpy(out, in, yuv_desc->width);
			in += yuv_desc->pitch;
			out += yuv_desc->width;
		}
	}

	//memcpy(&img1.data,output, yuv_desc->width * yuv_desc->height);
	buff = (unsigned char *)output;

	memcpy(img1.data, buff, 720*1280);
	//printf("## cols = %d ,rows = %d ##\n",img1.cols,img1.rows);
	//for(int i =0;i<img1.cols;i++){
	//	for(int j=0;j<img1.rows;j++){
	//		img1.at<uchar>(j,i) =  *(buff+j*img1.cols+i);
	//	}
	//}
	//imwrite ("/tmp/output1.jpg", img1);

	return 0;
}

static int save_yuv_data( struct iav_yuvbufdesc *yuv_desc,void * luma)
{
	static int pts_prev = 0, pts = 0;
	int luma_size, chroma_size;

	luma_size = yuv_desc->width * yuv_desc->height;
	if (yuv_desc->format == IAV_YUV_FORMAT_YUV420) {
		chroma_size = luma_size / 2;
	} else if (yuv_desc->format == IAV_YUV_FORMAT_YUV422) {
		chroma_size = luma_size;
	} else {
		printf("Error: Unrecognized yuv data format from DSP!\n");
		return -1;
	}

	if (save_yuv_luma_buffer(luma, yuv_desc) < 0) {
		printf("Failed to save luma data into buffer !\n");
		return -1;
	}

	return 0;
}

static int capture_yuv(int buff_id, int count)
{
	int i, buf;
	int non_stop = 0;
	void * luma = NULL;
	struct iav_yuvbufdesc yuv_desc;
	struct iav_querydesc query_desc;
	struct iav_yuv_cap *yuv_cap;
	struct iav_gdma_copy gdma_copy;
	int yuv_buffer_size = 0;
	int rval = 0;

	luma = malloc(MAX_YUV_BUFFER_SIZE);
	if (luma == NULL) {
		printf("Not enough memory for preview capture !\n");
		rval = -1;
		return rval;
	}

	memset(luma, 1, MAX_YUV_BUFFER_SIZE);

	if (count == 0) {
		non_stop = 1;
	}

	memset(&query_desc, 0, sizeof(query_desc));
	query_desc.qid = IAV_DESC_CANVAS;
	buf = buff_id;
	query_desc.arg.canvas.canvas_id = buf;

	if (!non_block_read) {
		query_desc.arg.canvas.non_block_flag &= ~IAV_BUFCAP_NONBLOCK;
	} else {
		query_desc.arg.canvas.non_block_flag |= IAV_BUFCAP_NONBLOCK;
	}

	if (ioctl(fd_iav, IAV_IOC_QUERY_DESC, &query_desc) < 0) {
		perror("IAV_IOC_QUERY_DESC");
		rval = -1;
		return rval;
	}

	memset(&yuv_desc, 0, sizeof(yuv_desc));
	yuv_cap = &query_desc.arg.canvas.yuv;
	yuv_desc.buf_id = buf;
	yuv_desc.y_addr_offset = yuv_cap->y_addr_offset;
	yuv_desc.uv_addr_offset = yuv_cap->uv_addr_offset;
	yuv_desc.pitch = yuv_cap->pitch;
	yuv_desc.width = yuv_cap->width;
	yuv_desc.height = yuv_cap->height;
	yuv_desc.seq_num = yuv_cap->seq_num;
	yuv_desc.format = yuv_cap->format;
	yuv_desc.mono_pts = yuv_cap->mono_pts;

	//printf("yuv_cap->y_addr_offset = %d \n",yuv_cap->y_addr_offset);
	//printf("yuv_cap->uv_addr_offset = %d \n",yuv_cap->uv_addr_offset);
	//printf("yuv_cap->pitch = %d \n",yuv_cap->pitch);
	//printf("yuv_cap->width = %d \n",yuv_cap->width);
	//printf("yuv_cap->height = %d \n",yuv_cap->height);
	//printf("yuv_cap->seq_num = %d \n",yuv_cap->seq_num);
	//printf("yuv_cap->format = %d \n",yuv_cap->format);
	//printf("yuv_cap->mono_pts = %d \n",yuv_cap->mono_pts);

	if ((yuv_desc.y_addr_offset == 0) || (yuv_desc.uv_addr_offset == 0)) {
		printf("YUV buffer [%d] address is NULL! \n", buf);
		rval = -1;
		return rval;
	}

	/* do GDMA copy if user buffer size > 0 */
	if (dsp_user_buf_size && dsp_buf_mapped) {
		yuv_buffer_size = yuv_desc.pitch * ROUND_UP(yuv_desc.height, 16) * 2;

		printf("yuv_buffer_size = %d \n",yuv_buffer_size);
		printf("dsp_user_buf_size = %d >>ROUND_UP(yuv_desc.height, 16)=%d \n",dsp_user_buf_size,ROUND_UP(yuv_desc.height, 16));
		if (dsp_user_buf_size < (u32)yuv_buffer_size) {
			printf("The size(0x%x) of IAV_BUFFER_USR is less than preview buffer size(0x%x). "
				"You need more!\n", dsp_user_buf_size, yuv_buffer_size);
			rval = -1;
			return rval;
		}


		gdma_copy.src_offset = yuv_desc.y_addr_offset;
		gdma_copy.dst_offset = 0;
		gdma_copy.src_pitch = yuv_desc.pitch;
		gdma_copy.dst_pitch = yuv_desc.pitch;
		gdma_copy.width = yuv_desc.width;
		gdma_copy.height = yuv_desc.height;
		gdma_copy.src_mmap_type = IAV_BUFFER_DSP;
		gdma_copy.dst_mmap_type = IAV_BUFFER_USR;
		if (ioctl(fd_iav, IAV_IOC_GDMA_COPY, &gdma_copy) < 0) {
			perror("IAV_IOC_GDMA_COPY");
		}

		gdma_copy.src_offset = yuv_desc.uv_addr_offset;
		gdma_copy.dst_offset = yuv_desc.pitch * ROUND_UP(yuv_desc.height, 16);
		gdma_copy.src_pitch = yuv_desc.pitch;
		gdma_copy.dst_pitch = yuv_desc.pitch;
		gdma_copy.width = yuv_desc.width;
		gdma_copy.height = yuv_desc.height;
		gdma_copy.src_mmap_type = IAV_BUFFER_DSP;
		gdma_copy.dst_mmap_type = IAV_BUFFER_USR;
		if (ioctl(fd_iav, IAV_IOC_GDMA_COPY, &gdma_copy) < 0) {
			perror("IAV_IOC_GDMA_COPY");
		}

	}

	if (save_yuv_data(&yuv_desc, luma) < 0) {
		printf("Failed to save YUV data of buf [%d].\n", buf);
		rval = -1;
		return rval;
	}

	if (luma)
		free(luma);
	//printf(" capture_yuv end !!! \n");
	return rval;
}

int test_yuvcap_init(void){

	if ((fd_iav = open("/dev/iav", O_RDWR, 0)) < 0) {
		perror("/dev/iav");
		return -1;
	}

	if (check_state() < 0)
		return -1;

	if (map_buffer() < 0)
		return -1;

	if (get_multi_vin_num() < 0) {
		perror("get_multi_vin_num");
		return -1;
	}

	return 0;
}

int test_yuvcap_flow(void){

#if 0
	if ((fd_iav = open("/dev/iav", O_RDWR, 0)) < 0) {
		perror("/dev/iav");
		return -1;
	}

	if (check_state() < 0)
			return -1;

	if (map_buffer() < 0)
			return -1;

	if (get_multi_vin_num() < 0) {
		perror("get_multi_vin_num");
		return -1;
	}
#endif

	//yuv_buffer_id = 1<<2;
	yuv_buffer_id=2;

	double t = 0;
	t = (double)getTickCount();
	capture_yuv(yuv_buffer_id, frame_count);

	t = (double)getTickCount() - t;
	printf( " capture_yuv -> detection time = %g ms \n", t*1000/getTickFrequency());

	return 0;
}


shm_cv_detect_t *p_shm_cv_detect;
void shared_memory_flow(void){
	static int init_share = 0;
	int shm_fd;
	void *addr;
	size_t	shm_size;


	if(init_share == 0){
		/* create the shared memory segment */
		shm_fd = shm_open(DETECT_SHM_NAME, O_CREAT | O_RDWR, 0666);
		if (shm_fd < 0) {
			return;
		}

		shm_size = sizeof(p_shm_cv_detect);
		/* configure the size of the shared memory */
		ftruncate(shm_fd, shm_size);
		addr = (void *)mmap(NULL, shm_size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
		p_shm_cv_detect = (shm_cv_detect_t *)addr;
		if (p_shm_cv_detect==MAP_FAILED) {
			perror("app_shm_mmap");
			return;
		}

		init_share = 1;
	}

	p_shm_cv_detect->sample_motion_state = g_motion_state;

	//printf("++++++++++++++++++++++++++++++++++++++++\n");
	//p_shm_cv_detect->sample_motion_state = 196;
	//printf("sample_motion_state = %d \n", p_shm_cv_detect->sample_motion_state);


}

shm_cv_detect_t *p_shm_cv_detect2;

void shared_memory_flow_get(void){
	static int init_share = 0;
	void *addr;
	int shm_fd;
	size_t	shm_size;


	if(init_share == 0){
		/* create the shared memory segment */
		shm_fd = shm_open(DETECT_SHM_NAME, O_CREAT | O_RDWR, 0666);
		if (shm_fd < 0) {
			return;
		}

		shm_size = sizeof(p_shm_cv_detect2);
		/* configure the size of the shared memory */
		ftruncate(shm_fd, shm_size);
		addr = (void *)mmap(NULL, shm_size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
		p_shm_cv_detect2 = (shm_cv_detect_t *)addr;
		if (p_shm_cv_detect2==MAP_FAILED) {
			perror("app_shm_mmap");
			return;
		}

		init_share = 1;
	}


	//printf("++++++++++++++++++++++++++++++++++++++++\n");
	//p_shm_cv_detect->sample_motion_state = 196;
	printf("sample_motion_state = %d \n", p_shm_cv_detect2->sample_motion_state);

}

int main( int argc , char ** argv )
{
	static int count_frame = 0;
	string cascade_name = "./haarcascades/haarcascade_frontalface_default.xml";
	//string cascade_name = "./haarcascades/haarcascade_upperbody.xml";
	CascadeClassifier cascade;

	//if ( !cascade.load(cascade_name) )	{
	//	cerr << "ERROR: Could not load cascade classifier" << endl;
	//	return -1;
	//}

	//sample_osd_main_flow_init();
	test_yuvcap_init();



	//test_yuvcap_flow();

	//imwrite ("/tmp/output1.jpg", img1);
	//printf("yuv_cap_end ~ \n");


	//cout <<"--- C" << endl;

	//CascadeClassifier cascade;

	//if ( !cascade.load(cascade_name) )	{
	//	cerr << "ERROR: Could not load cascade classifier" << endl;
	//	return -1;
	//}

	//cout <<"--- D" << endl;

	while(1){
		test_yuvcap_flow();
		//shared_memory_flow_get();
		if( !img1.empty() ) {
				//detectAndDraw( img1, cascade, /*nestedCascade,*/ 1.0 /*scale*/, 0 /*tryflip*/);
				//resize( img1, img1, Size(480,320), 0, 0, INTER_LINEAR );
				//sample_hog_human_detection(img1);
				sample_motion_detectionn(img1);
				//waitKey(0);
		}

		//sample_osd_main_flow();

#if 1
		count_frame++;
		if(count_frame == 30){
			if(g_motion_state == 0){
				g_motion_state = 2;
			}
			shared_memory_flow();
			return EXIT_SUCCESS;
		}else{
			shared_memory_flow();
		}


		if(g_motion_state == 1){
			printf("### motion detection END !###\n");
			return EXIT_SUCCESS;
		}
#endif
	}


    return EXIT_SUCCESS;
}



void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    /*CascadeClassifier& nestedCascade,*/
                    double scale, bool tryflip )
{
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    Mat gray, smallImg;

    //cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / scale;
    //resize( gray, smallImg, Size(320,480), fx, fx, INTER_LINEAR );
    //resize( img, smallImg, Size(480,320), 0, 0, INTER_LINEAR );
    equalizeHist( img, img );

	t = (double)getTickCount();
    cascade.detectMultiScale( img, faces,
        1.1, 0/*2*/, 0
		//|CASCADE_DO_CANNY_PRUNING
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(40, 40) );

//    if ( tryflip ) {
//        flip(smallImg, smallImg, 1);
//        cascade.detectMultiScale( smallImg, faces2,
//                                 1.1, 2, 0
//                                 //|CASCADE_FIND_BIGGEST_OBJECT
//                                 //|CASCADE_DO_ROUGH_SEARCH
//                                 |CASCADE_SCALE_IMAGE,
//                                 Size(30, 30) );
//        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); ++r ) {
//            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
//        }
//    }
//
    t = (double)getTickCount() - t;
    printf( "detection time = %g ms, Face num = %lu \n", t*1000/getTickFrequency(), faces.size());

    int index = 0;
    if(faces.size() > 0){
    	sample_osd_clear_all();
    }

    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r.width/r.height;
//printf("Face[%d] --> aratio= %g \n", i);
       // if ( 0.75 < aspect_ratio && aspect_ratio < 1.3 ) {
        if(1){
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);

            if(center.x <= 0){
            	center.x = 0;
            }

            if(center.y <= 0){
                center.y = 0;
            }

            if(index>=10){
                return ;
            }

            printf("1>>index = %d 320p ROI(X,Y)=>(%d,%d)\n",index,center.x,center.y);
            center.x = ((center.x*1920)/1280) -75;
            center.y = ((center.y*1080)/720) -75;
            printf("index = %d 320->1080 ROI(X,Y)=>(%d,%d)\n",index,center.x,center.y);
            sample_osd_enable(index,center.x,center.y,1);
            index++;
            //sample_osd_enable(i,center.x,center.y,1);
            //sample_osd_enable(0,200,124,1);
            //sample_osd_enable(1,400,400,1);
            //sample_osd_enable(2,233,267,1);
            //sample_osd_enable(3,443,207,1);


            radius = cvRound((r.width + r.height)*0.25*scale);
            //circle( img, center, radius, color, 2, 8, 0 );
            circle( smallImg, center, radius, color, 2, 8, 0 );
        }
        else {
            //rectangle( img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
            //           cvPoint(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
            //           color, 3, 8, 0);

            rectangle( smallImg, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
                         cvPoint(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                         color, 3, 8, 0);
        }

#if 0
        if ( nestedCascade.empty() ) {
            continue;
        }
        smallImgROI = smallImg( r );
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            //|CASCADE_DO_CANNY_PRUNING
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) );
        for ( size_t j = 0; j < nestedObjects.size(); j++ )
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((r.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((r.y + nr.y + nr.height*0.5)*scale);
            radius = cvRound((nr.width + nr.height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
#endif
    }
cout << "--- X" << endl;
//    imshow( "result", img ); waitKey(0);
    //imwrite("/tmp/result.jpg", img);
    //imwrite("/tmp/result.jpg", smallImg);
cout << "--- Y" << endl;
}

void sample_hog_human_detection( Mat& img){

	Mat Frame;
	resize( img, Frame, Size(480,320), 0, 0, INTER_LINEAR );

	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	vector<Rect> found, found_filtered;
	double t = (double)getTickCount();
	// run the detector with default parameters. to get a higher hit-rate
	// (and more false alarms, respectively), decrease the hitThreshold and
	// groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
	hog.detectMultiScale(Frame, found, 0, Size(8, 8), Size(32, 32), 1.05, 2);
	t = (double)getTickCount() - t;
	//printf("H,W => (%d,%d)\n",Frame.rows,Frame.cols);
	printf("tdetection time = %gms\n", t*1000. / cv::getTickFrequency());
	//printf("found.size => %d \n",found.size());



	int i =0;
	int j =0;
	for ( i = 0; i < found.size(); i++)
	        {
	            Rect r = found[i];
	            for (j = 0; j < found.size(); j++)
	                if (j != i && (r & found[j]) == r)
	                    break;
	            if (j == found.size())
	                found_filtered.push_back(r);
	        }
	        for (i = 0; i < found_filtered.size(); i++)
	        {
	            Rect r = found_filtered[i];
	            // the HOG detector returns slightly larger rectangles than the real objects.
	            // so we slightly shrink the rectangles to get a nicer output.
	            r.x += cvRound(r.width*0.1);
	            r.width = cvRound(r.width*0.8);
	            r.y += cvRound(r.height*0.07);
	            r.height = cvRound(r.height*0.8);

	            //printf("center x,y(%d,%d) ->width %d ->height %d \n",r.x,r.y,r.width,r.height);
	            rectangle(Frame, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
	        }

}

Mat Bg_frame;
void sample_motion_detectionn( Mat& img){
	static int init_motion = 0;
	//Mat Bg_frame;
	Mat result;
	Rect bounding_rect;
	int i = 0;

	Mat Frame;
	//printf("#### MOTION DETECTION ####\n");
	resize( img, Frame, Size(480,320), 0, 0, INTER_LINEAR );

	//imwrite ("/tmp/oraingenal_FRAME.jpg",Frame);

	if(init_motion == 0){
		Frame.copyTo(Bg_frame);
		blur(Bg_frame, Bg_frame, Size(5,5));
		//imwrite ("/tmp/Bg_frame.jpg",Bg_frame);
		init_motion = 1;
	}

    double t = (double)getTickCount();

	blur(Frame, Frame, Size(5,5));
	absdiff(Bg_frame,Frame,result);
	threshold(result, result, 20, 255, THRESH_BINARY);
	Mat erodeStruct = getStructuringElement(MORPH_RECT,Size(5,5));
	erode (result, result, erodeStruct);
	erode (result, result, erodeStruct);
	dilate(result, result, erodeStruct);
	dilate(result, result, erodeStruct);
	vector<vector<Point>> contours;
	findContours(result,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

	t = (double)getTickCount() - t;

	printf("tdetection time = %gms\n", t*1000. / cv::getTickFrequency());

	vector<vector<Point> >::iterator itc= contours.begin();
	int big_index = 0;
	int big_size = 0;
	i = 0;
	while (itc!=contours.end()) {
		//Create bounding rect of object
			bounding_rect = boundingRect(contours[i]);
			bounding_rect.x = bounding_rect.x;
			bounding_rect.y = bounding_rect.y;
			bounding_rect.width = bounding_rect.width;
			bounding_rect.height = bounding_rect.height;

			if(bounding_rect.width > (480/8)){
				if(g_motion_state == 0){
					g_motion_state = 1;
				}
				rectangle(Frame, bounding_rect, Scalar(128,128,128), 2, 8, 0);
				//rectangle(result, bounding_rect, Scalar(128), 2, 8, 0);

				//if(big_size <= bounding_rect.height){
				//big_size = bounding_rect.height;
				//big_index = i;
			}

			//}
			++itc;
			++i;
	}

	//imwrite ("/tmp/FRAME.jpg", Frame);
	//imwrite ("/tmp/result.jpg", result);

}


