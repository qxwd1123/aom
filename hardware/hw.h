#ifndef __HW_H
#define __HW_H

#include <stdint.h>
#include <stdbool.h>
#include "av1/common/av1_common_int.h"

void hw_global_init();
void hw_global_destroy();
void hw_global_set_input_fn(char *input_fn);
void hw_global_set_mi_params(int32_t fb_rows, int32_t fb_cols);
void hw_global_set_frame_params(int32_t width, int32_t height);
void hw_global_frame_start();
void hw_global_frame_next();
void hw_global_frame_done();

void hw_filter_stat_lf_update_frame();
void hw_filter_stat_cdef_update_seq(int8_t enable_cdef);
void hw_filter_stat_cdef_update_block(CommonModeInfoParams *mi_params);
void hw_filter_stat_cdef_update_dir(int32_t fbc, int32_t fbr, int32_t *dir);
void hw_filter_stat_cdef_update_diff_pos_64(int32_t fbc, int32_t fbr);
void hw_filter_stat_cdef_update_diff_pos_8(int32_t bx, int32_t by);
void hw_filter_stat_cdef_update_diff(int32_t *partial);
void hw_filter_stat_cdef_dir_intra(int32_t col, int32_t row, BLOCK_SIZE bsize,
                                   PREDICTION_MODE mode);
void hw_filter_stat_up_update_seq(int8_t enable_superres);
void hw_filter_stat_up_update_frame(int8_t use_superres);
void hw_filter_stat_lr_update_frame(int8_t do_loop_restoration);
void hw_filter_stat_lr_set_num_units(int32_t nunits);
void hw_filter_stat_lr_update_block(int32_t runit_idx,
                                    RestorationType restoration_type);
void hw_filter_stat_fgs_update_seq(int8_t film_grain_params_present);
void hw_filter_stat_fgs_update_frame(int8_t apply_graim);

void hw_filter_stat_lf_update_block(int8_t dir, int32_t filter_length,
                                    int32_t curr_x, int32_t curr_y);

#endif