#ifndef __HW_GLOBAL_H
#define __HW_GLOBAL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "aom_util/debug_util.h"
#include "av1/common/av1_common_int.h"

typedef enum hw_res_e {
  RET_SUCCESS = 0,
  RET_FAILED = -1,
  RET_ERROR = -2,
} hw_res_e;

typedef struct hw_common_t {
  int32_t fb_cols;
  int32_t fb_rows;
  int32_t mi_cols;
  int32_t mi_rows;
  int32_t fb_num;
  int32_t frame_width;
  int32_t frame_height;
} hw_common_t;

typedef struct hw_global_t {
  int32_t frame_idx;
  char *input_fn;
  char *output_fn;
  void *hw_filter_stat_deque;
  hw_common_t *hw_common;
} hw_global_t;

extern hw_global_t *hw_global;

void hw_global_init();

void hw_global_destroy();

void hw_global_set_input_fn(char *input_fn);

void hw_global_set_mi_params(int32_t mi_rows, int32_t mi_cols);

void hw_global_set_frame_params(int32_t width, int32_t height);

void hw_global_frame_start();

void hw_global_frame_next();

void hw_global_frame_done();

#endif
