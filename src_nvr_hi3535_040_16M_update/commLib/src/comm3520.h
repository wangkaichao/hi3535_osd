#ifndef comm3520_h
#define comm3520_h

/* comm3520.h */

typedef struct __frame_head_t
{
	unsigned long device_type;
	unsigned long frame_size;
	unsigned long frame_no;
	unsigned char video_resolution;
	unsigned char frame_type;
	unsigned char frame_rate;
	unsigned char video_standard;
	unsigned long sec;
	unsigned long usec;
	unsigned long long pts;
}frame_head_t;

typedef struct __frame_t
{
    frame_head_t frame_head;
    unsigned char *frame_data;
}frame_t;

#endif
