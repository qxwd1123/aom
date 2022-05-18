#ifndef __HW_FILTER_STAT_H
#define __HW_FILTER_STAT_H

#include "hw_global.h"
#include "hw_utils.h"

#define HW_FILTER_STAT_DEQUE_SIZE (1 << 16) - 1
typedef struct hw_lf_stat_t {
  int32_t filter_level[2];
  int8_t enable_lf;
  int32_t lf_num_vert;
  int32_t lf_num_horiz;
  int8_t *filter_length_vert;
  int8_t *filter_length_horiz;
} hw_lf_stat_t;

typedef struct hw_cdef_stat_t {
  int8_t enable_cdef;
  int8_t *cdef_idx;
  int8_t *skip;
  int32_t cdef_dir_cols;
  int32_t cdef_dir_rows;
  int32_t cdef_dir_num;
  int8_t *cdef_dir;
  int8_t *cdef_dir_intra;
  int32_t cdef_block_cols;
  int32_t cdef_block_rows;
  int32_t cdef_block_bx;
  int32_t cdef_block_by;
  int32_t **cdef_diff;
} hw_cdef_stat_t;

typedef struct hw_up_stat_t {
  int8_t enable_up;
} hw_up_stat_t;

typedef struct hw_lr_stat_t {
  int8_t enable_lr;
  int32_t lr_num;
  RestorationType *lr_type;
} hw_lr_stat_t;

typedef struct hw_fgs_stat_t {
  int8_t enable_fgs;
} hw_fgs_stat_t;

typedef struct hw_filter_stat_t {
  int32_t frame_idx;
  hw_common_t *common;
  hw_lf_stat_t *lf_stat;
  hw_cdef_stat_t *cdef_stat;
  int8_t enable_superres;
  hw_up_stat_t *up_stat;
  hw_lr_stat_t *lr_stat;
  int8_t film_grain_params_present;
  hw_fgs_stat_t *fgs_stat;
} hw_filter_stat_t;

hw_deque_t *hw_filter_stat_init();

hw_filter_stat_t *hw_filter_stat_init_internal(int32_t frame_idx);

void hw_filter_stat_destroy();

void hw_filter_stat_lf_update_frame();
void hw_filter_stat_lf_update_block(int8_t dir, int32_t filter_length,
                                    int32_t curr_x, int32_t curr_y);

void hw_filter_stat_cdef_update_seq(int8_t enable_cdef);
void hw_filter_stat_cdef_update_block(CommonModeInfoParams *mi_params);
void hw_filter_stat_cdef_update_dir(int32_t fbc, int32_t fbr, int32_t *dir);
void hw_filter_stat_cdef_update_diff_pos_64(int32_t fbc, int32_t fbr);
void hw_filter_stat_cdef_update_diff_pos_8(int32_t bx, int32_t by);
void hw_filter_stat_cdef_update_diff(int32_t *partial);
void hw_filter_stat_cdef_dir_intra(int32_t blk_col, int32_t blk_row,
                                   BLOCK_SIZE bsize, PREDICTION_MODE mode);

void hw_filter_stat_up_update_seq(int8_t enable_superres);
void hw_filter_stat_up_update_frame(int8_t use_superres);

void hw_filter_stat_lr_update_frame(int8_t do_loop_restoration);
void hw_filter_stat_lr_set_num_units(int32_t nunits);
void hw_filter_stat_lr_update_block(int32_t runit_idx,
                                    RestorationType restoration_type);

void hw_filter_stat_fgs_update_seq(int8_t film_grain_params_present);
void hw_filter_stat_fgs_update_frame(int8_t apply_graim);

void hw_filter_stat_frame_next(int32_t frame_idx);
void hw_filter_stat_frame_done();

#endif
