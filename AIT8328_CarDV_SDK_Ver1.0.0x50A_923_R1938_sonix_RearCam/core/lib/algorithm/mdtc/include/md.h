/*
 * md.h
 *
 *  Created on: 2014/1/8
 *      Author: chris
 */

#ifndef MD_HPP_
#define MD_HPP_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MD_params_in_s
{
	//(0: disable, 1: enable)
	unsigned char enable;
	//(0 ~ 99)
    unsigned char size_perct_thd_min;
    //(1 ~ 100), must be larger than size_perct_thd_min
    unsigned char size_perct_thd_max;
    //(10, 20, 30, ..., 100), 100 is the most sensitive
    unsigned char sensitivity;
    //(1000 ~ 30000)
    unsigned short learn_rate;
} MD_params_in_t;

typedef struct MD_params_out_s
{
    unsigned char md_result;
    unsigned int obj_cnt;
} MD_params_out_t;

extern unsigned int MD_GetLibVersion(unsigned int* ver);
extern int MD_init(unsigned char* working_buf_ptr, int working_buf_len, unsigned short width, unsigned short height, unsigned char color);
extern int MD_run(const unsigned char* _ucImage);
extern int MD_set_detect_window(unsigned short lt_x, unsigned short lt_y, unsigned short rb_x, unsigned short rb_y, unsigned char w_div, unsigned char h_div);
extern int MD_get_detect_window_size(unsigned short* st_x, unsigned short* st_y, unsigned short* div_w, unsigned short* div_h);
extern int MD_set_window_params_in(unsigned char w_num, unsigned char h_num, const MD_params_in_t* param);
extern int MD_get_window_params_in(unsigned char w_num, unsigned char h_num, MD_params_in_t* param);
extern int MD_get_window_params_out(unsigned char w_num, unsigned char h_num, MD_params_out_t* param);
extern int MD_get_buffer_info(unsigned short width, unsigned short height, unsigned char color, unsigned char w_div, unsigned char h_div);
extern void MD_printk(char *fmt, ...);
extern void MD_set_time_ms(unsigned int time_diff);
extern void MD_get_Y_mean(unsigned int* mean);

#ifdef __cplusplus
}
#endif

#endif /* MD_HPP_ */
