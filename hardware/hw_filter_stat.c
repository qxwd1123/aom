#include "hardware/hw_filter_stat.h"
#include "av1/common/cdef_block.h"

static hw_filter_stat_t *hw_filter_stat;

hw_deque_t *hw_filter_stat_init() {
  hw_deque_t *hw_filter_stat_deque =
      hw_deque_new(hw_filter_stat_t *, HW_FILTER_STAT_DEQUE_SIZE);
  if (hw_filter_stat_deque == NULL) return NULL;
  // hw_filter_stat_t *hw_filter_stat_cur = hw_filter_stat_init_internal(0);
  // if (hw_filter_stat_cur == NULL) return NULL;
  // hw_deque_push_back(hw_filter_stat_deque, &hw_filter_stat_cur);

  // hw_filter_stat = hw_filter_stat_cur;
  return hw_filter_stat_deque;
}

hw_filter_stat_t *hw_filter_stat_init_internal(int32_t frame_idx) {
  hw_filter_stat_t *hw_filter_stat_cur =
      (hw_filter_stat_t *)malloc(sizeof(hw_filter_stat_t));
  if (hw_filter_stat_cur == NULL) return NULL;
  hw_filter_stat_cur->common = (hw_common_t *)malloc(sizeof(hw_common_t));
  if (hw_filter_stat_cur->common == NULL) return NULL;
  hw_filter_stat_cur->lf_stat = (hw_lf_stat_t *)malloc(sizeof(hw_lf_stat_t));
  if (hw_filter_stat_cur->lf_stat == NULL) return NULL;
  hw_filter_stat_cur->cdef_stat =
      (hw_cdef_stat_t *)malloc(sizeof(hw_cdef_stat_t));
  if (hw_filter_stat_cur->cdef_stat == NULL) return NULL;
  hw_filter_stat_cur->up_stat = (hw_up_stat_t *)malloc(sizeof(hw_up_stat_t));
  if (hw_filter_stat_cur->up_stat == NULL) return NULL;
  hw_filter_stat_cur->lr_stat = (hw_lr_stat_t *)malloc(sizeof(hw_lr_stat_t));
  if (hw_filter_stat_cur->lr_stat == NULL) return NULL;
  hw_filter_stat_cur->fgs_stat = (hw_fgs_stat_t *)malloc(sizeof(hw_fgs_stat_t));
  if (hw_filter_stat_cur->fgs_stat == NULL) return NULL;

  hw_filter_stat_cur->frame_idx = frame_idx;

  memcpy(hw_filter_stat_cur->common, hw_global->hw_common, sizeof(hw_common_t));
  hw_filter_stat_cur->lf_stat->filter_level[0] = 0;
  hw_filter_stat_cur->lf_stat->filter_level[1] = 0;
  hw_filter_stat_cur->lf_stat->enable_lf = 0;
  hw_filter_stat_cur->lf_stat->lf_num_horiz =
      hw_filter_stat_cur->common->frame_width *
      (hw_filter_stat_cur->common->mi_rows);
  hw_filter_stat_cur->lf_stat->lf_num_vert =
      hw_filter_stat_cur->common->frame_height *
      (hw_filter_stat_cur->common->mi_cols);
  hw_filter_stat_cur->lf_stat->filter_length_vert = (int8_t *)calloc(
      hw_filter_stat_cur->lf_stat->lf_num_vert, sizeof(int8_t));
  hw_filter_stat_cur->lf_stat->filter_length_horiz = (int8_t *)calloc(
      hw_filter_stat_cur->lf_stat->lf_num_horiz, sizeof(int8_t));

  hw_filter_stat_cur->cdef_stat->enable_cdef = 0;
  hw_filter_stat_cur->cdef_stat->cdef_idx =
      (int8_t *)calloc(hw_filter_stat_cur->common->fb_num, sizeof(int8_t));
  hw_filter_stat_cur->cdef_stat->skip =
      (int8_t *)calloc(hw_filter_stat_cur->common->fb_num, sizeof(int8_t));
  hw_filter_stat_cur->cdef_stat->cdef_dir_cols =
      (hw_filter_stat_cur->common->mi_cols + 1) / 2;
  hw_filter_stat_cur->cdef_stat->cdef_dir_rows =
      (hw_filter_stat_cur->common->mi_rows + 1) / 2;
  hw_filter_stat_cur->cdef_stat->cdef_dir_num =
      hw_filter_stat_cur->cdef_stat->cdef_dir_cols *
      hw_filter_stat_cur->cdef_stat->cdef_dir_rows;
  hw_filter_stat_cur->cdef_stat->cdef_dir = (int8_t *)calloc(
      hw_filter_stat_cur->cdef_stat->cdef_dir_num, sizeof(int8_t));
  hw_filter_stat_cur->cdef_stat->cdef_dir_intra = (int8_t *)calloc(
      hw_filter_stat_cur->cdef_stat->cdef_dir_num, sizeof(int8_t));
  hw_filter_stat_cur->cdef_stat->cdef_block_cols = 0;
  hw_filter_stat_cur->cdef_stat->cdef_block_rows = 0;
  hw_filter_stat_cur->cdef_stat->cdef_block_bx = 0;
  hw_filter_stat_cur->cdef_stat->cdef_block_by = 0;
  hw_filter_stat_cur->cdef_stat->cdef_diff = (int32_t **)calloc(
      hw_filter_stat_cur->cdef_stat->cdef_dir_num, sizeof(int32_t *));

  hw_filter_stat_cur->enable_superres = 0;
  hw_filter_stat_cur->up_stat->enable_up = 0;

  hw_filter_stat_cur->lr_stat->enable_lr = 0;
  hw_filter_stat_cur->lr_stat->lr_num = 0;
  hw_filter_stat_cur->lr_stat->lr_type = NULL;

  hw_filter_stat_cur->film_grain_params_present = 0;
  hw_filter_stat_cur->fgs_stat->enable_fgs = 0;

  return hw_filter_stat_cur;
}

void hw_filter_stat_destroy() {
  if (hw_deque_empty(hw_global->hw_filter_stat_deque)) return;
  for (hw_filter_stat_t **hw_filter_stat_iter =
           hw_deque_begin(hw_global->hw_filter_stat_deque);
       hw_filter_stat_iter != hw_deque_end(hw_global->hw_filter_stat_deque);
       hw_filter_stat_iter = hw_deque_iter_inc(hw_global->hw_filter_stat_deque,
                                               hw_filter_stat_iter)) {
    hw_filter_stat_t *hw_filter_stat_cur = *hw_filter_stat_iter;
    free(hw_filter_stat_cur->lf_stat->filter_length_vert);
    free(hw_filter_stat_cur->lf_stat->filter_length_horiz);
    free(hw_filter_stat_cur->lf_stat);
    free(hw_filter_stat_cur->cdef_stat->cdef_idx);
    free(hw_filter_stat_cur->cdef_stat->skip);
    free(hw_filter_stat_cur->cdef_stat->cdef_dir);
    free(hw_filter_stat_cur->cdef_stat->cdef_dir_intra);
    for (int32_t row = 0; row < hw_filter_stat_cur->cdef_stat->cdef_dir_rows;
         row++) {
      for (int32_t col = 0; col < hw_filter_stat_cur->cdef_stat->cdef_dir_cols;
           col++) {
        int32_t *partial =
            hw_filter_stat_cur->cdef_stat
                ->cdef_diff[row * hw_filter_stat_cur->cdef_stat->cdef_dir_cols +
                            col];
        if (partial != NULL) free(partial);
      }
    }
    free(hw_filter_stat_cur->cdef_stat->cdef_diff);
    free(hw_filter_stat_cur->cdef_stat);
    free(hw_filter_stat_cur->up_stat);
    if (hw_filter_stat_cur->lr_stat->lr_type != NULL) {
      free(hw_filter_stat_cur->lr_stat->lr_type);
    }
    free(hw_filter_stat_cur->lr_stat);
    free(hw_filter_stat_cur->fgs_stat);
    free(hw_filter_stat_cur->common);
    free(hw_filter_stat_cur);
  }
  hw_deque_destroy(hw_global->hw_filter_stat_deque);
}

void hw_filter_stat_lf_update_frame(int32_t filter_level_0,
                                    int32_t filter_level_1) {
  hw_filter_stat->lf_stat->filter_level[0] = filter_level_0;
  hw_filter_stat->lf_stat->filter_level[1] = filter_level_1;
  hw_filter_stat->lf_stat->enable_lf =
      hw_filter_stat->lf_stat->filter_level[0] ||
      hw_filter_stat->lf_stat->filter_level[1];
}

void hw_filter_stat_lf_update_block(int8_t dir, int32_t filter_length,
                                    int32_t curr_x, int32_t curr_y) {
  if (dir == 0) {
    hw_filter_stat->lf_stat
        ->filter_length_vert[curr_x / MI_SIZE +
                             curr_y * (hw_filter_stat->common->mi_cols - 1)] =
        filter_length;
  } else {
    hw_filter_stat->lf_stat->filter_length_horiz
        [curr_x + curr_y / MI_SIZE * hw_filter_stat->common->frame_width] =
        filter_length;
  }
}

void hw_filter_stat_cdef_update_seq(int8_t enable_cdef) {
  hw_filter_stat->cdef_stat->enable_cdef = enable_cdef;
}

void hw_filter_stat_cdef_update_block(CommonModeInfoParams *mi_params) {
  const int32_t nb_row = hw_filter_stat->common->fb_rows;
  const int32_t nb_col = hw_filter_stat->common->fb_cols;
  for (int32_t row = 0; row < nb_row; row++) {
    for (int32_t col = 0; col < nb_col; col++) {
      const int32_t mbmi_cdef_strength =
          mi_params
              ->mi_grid_base[MI_SIZE_64X64 * row * mi_params->mi_stride +
                             MI_SIZE_64X64 * col]
              ->cdef_strength;
      hw_filter_stat->cdef_stat
          ->cdef_idx[row * hw_filter_stat->common->fb_cols + col] =
          mbmi_cdef_strength;
    }
  }
}

void hw_filter_stat_cdef_update_dir(int32_t fbc, int32_t fbr, int32_t *dir) {
  int32_t col_max = 8;
  int32_t row_max = 8;
  if (fbc == hw_filter_stat->common->fb_cols - 1)
    col_max = hw_filter_stat->cdef_stat->cdef_dir_cols % 8;
  if (fbr == hw_filter_stat->common->fb_rows - 1)
    row_max = hw_filter_stat->cdef_stat->cdef_dir_rows % 8;
  for (int32_t row = 0; row < row_max; row++) {
    for (int32_t col = 0; col < col_max; col++) {
      const int32_t cdef_dir = dir[row + 8 * col];
      hw_filter_stat->cdef_stat
          ->cdef_dir[fbr * hw_filter_stat->cdef_stat->cdef_dir_cols *
                         8 +
                     fbc + row * col_max + col] = cdef_dir;
    }
  }
}

void hw_filter_stat_cdef_update_diff_pos_64(int32_t fbc, int32_t fbr) {
  hw_filter_stat->cdef_stat->cdef_block_cols = fbc * 8;
  hw_filter_stat->cdef_stat->cdef_block_rows = fbr * 8;
}

void hw_filter_stat_cdef_update_diff_pos_8(int32_t bx, int32_t by) {
  hw_filter_stat->cdef_stat->cdef_block_bx = bx;
  hw_filter_stat->cdef_stat->cdef_block_by = by;
}

void hw_filter_stat_cdef_update_diff(int32_t *partial) {
  if (hw_filter_stat->cdef_stat->cdef_block_rows + hw_filter_stat->cdef_stat->cdef_block_by <
          hw_filter_stat->cdef_stat->cdef_dir_rows &&
      hw_filter_stat->cdef_stat->cdef_block_cols +hw_filter_stat->cdef_stat->cdef_block_bx <
          hw_filter_stat->cdef_stat->cdef_dir_cols) {
    int32_t *partial_p = (int32_t *)malloc(sizeof(int32_t) * 8 * 15);
    assertf(partial_p != NULL) fprintf(stderr, "partial_p malloc failed\n");

    memcpy(partial_p, partial, sizeof(int32_t) * 8 * 15);
    hw_filter_stat->cdef_stat
        ->cdef_diff[hw_filter_stat->cdef_stat->cdef_block_cols +
                    hw_filter_stat->cdef_stat->cdef_block_bx +
                    hw_filter_stat->cdef_stat->cdef_dir_cols *
                        (hw_filter_stat->cdef_stat->cdef_block_rows +
                         hw_filter_stat->cdef_stat->cdef_block_by)] = partial_p;
  }
}

static const int8_t mode_map[] = { 6, 2, 0, 4, 3, 5, 7, 1, 4, 6, 2 };

void hw_filter_stat_cdef_dir_intra(int32_t col, int32_t row, BLOCK_SIZE bsize,
                                   PREDICTION_MODE mode) {
  if (mode >= V_PRED && mode <= SMOOTH_H_PRED) {
    mode -= V_PRED;
    int32_t blk_col = col;
    int32_t blk_row = row;
    switch (bsize) {
      case BLOCK_4X4:
        if (blk_col % 8 == 0 && blk_row % 8 == 0) {
          hw_filter_stat->cdef_stat
              ->cdef_dir_intra[blk_col / 2 +
                               blk_row / 2 *
                                   hw_filter_stat->cdef_stat->cdef_dir_cols] =
              mode_map[mode];
        }
        break;
      case BLOCK_4X8:
        if (blk_col % 8 == 0) {
          hw_filter_stat->cdef_stat
              ->cdef_dir_intra[blk_col / 2 +
                               blk_row / 2 *
                                   hw_filter_stat->cdef_stat->cdef_dir_cols] =
              mode_map[mode];
        }
        break;
      case BLOCK_8X4:
        if (blk_row % 8 == 0) {
          hw_filter_stat->cdef_stat
              ->cdef_dir_intra[blk_col / 2 +
                               blk_row / 2 *
                                   hw_filter_stat->cdef_stat->cdef_dir_cols] =
              mode_map[mode];
        }
        break;
      case BLOCK_8X8:
        hw_filter_stat->cdef_stat
            ->cdef_dir_intra[blk_col / 2 +
                             blk_row / 2 *
                                 hw_filter_stat->cdef_stat->cdef_dir_cols] =
            mode_map[mode];
        break;
      case BLOCK_8X16:
        for (int32_t i = 0; i < 2; i++) {
          if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
              blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows)
            continue;
          hw_filter_stat->cdef_stat
              ->cdef_dir_intra[blk_col / 2 +
                               blk_row / 2 *
                                   hw_filter_stat->cdef_stat->cdef_dir_cols] =
              mode_map[mode];
          blk_row += 2;
        }
        break;
      case BLOCK_16X8:
        for (int32_t j = 0; j < 2; j++) {
          if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
              blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows)
            continue;
          hw_filter_stat->cdef_stat
              ->cdef_dir_intra[blk_col / 2 +
                               blk_row / 2 *
                                   hw_filter_stat->cdef_stat->cdef_dir_cols] =
              mode_map[mode];
          blk_col += 2;
        }
        break;
      case BLOCK_16X16:
        for (int32_t i = 0; i < 2; i++) {
          for (int32_t j = 0; j < 2; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 4;
          blk_row += 2;
        }
        break;
      case BLOCK_16X32:
        for (int32_t i = 0; i < 4; i++) {
          for (int32_t j = 0; j < 2; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 4;
          blk_row += 2;
        }
        break;
      case BLOCK_32X16:
        for (int32_t i = 0; i < 2; i++) {
          for (int32_t j = 0; j < 4; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 8;
          blk_row += 2;
        }
        break;
      case BLOCK_32X32:
        for (int32_t i = 0; i < 4; i++) {
          for (int32_t j = 0; j < 4; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 8;
          blk_row += 2;
        }
        break;
      case BLOCK_32X64:
        for (int32_t i = 0; i < 8; i++) {
          for (int32_t j = 0; j < 4; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 8;
          blk_row += 2;
        }
        break;
      case BLOCK_64X32:
        for (int32_t i = 0; i < 4; i++) {
          for (int32_t j = 0; j < 8; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 16;
          blk_row += 2;
        }
        break;
      case BLOCK_64X64:
        for (int32_t i = 0; i < 8; i++) {
          for (int32_t j = 0; j < 8; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 16;
          blk_row += 2;
        }
        break;
      case BLOCK_64X128:
        for (int32_t i = 0; i < 16; i++) {
          for (int32_t j = 0; j < 8; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 16;
          blk_row += 2;
        }
        break;
      case BLOCK_128X64:
        for (int32_t i = 0; i < 8; i++) {
          for (int32_t j = 0; j < 16; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 32;
          blk_row += 2;
        }
        break;
      case BLOCK_128X128:
        for (int32_t i = 0; i < 16; i++) {
          for (int32_t j = 0; j < 16; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 32;
          blk_row += 2;
        }
        break;
      case BLOCK_4X16:
        if (blk_col % 8 == 0) {
          for (int32_t i = 0; i < 2; i++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_row += 2;
          }
        }
        break;
      case BLOCK_16X4:
        if (blk_row % 8 == 0) {
          for (int32_t i = 0; i < 2; i++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
        }
        break;
      case BLOCK_8X32:
        for (int32_t i = 0; i < 4; i++) {
          if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
              blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
            blk_col += 2;
            continue;
          }
          hw_filter_stat->cdef_stat
              ->cdef_dir_intra[blk_col / 2 +
                               blk_row / 2 *
                                   hw_filter_stat->cdef_stat->cdef_dir_cols] =
              mode_map[mode];
          blk_row += 2;
        }
        break;
      case BLOCK_32X8:
        for (int32_t i = 0; i < 4; i++) {
          if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
              blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
            blk_col += 2;
            continue;
          }
          hw_filter_stat->cdef_stat
              ->cdef_dir_intra[blk_col / 2 +
                               blk_row / 2 *
                                   hw_filter_stat->cdef_stat->cdef_dir_cols] =
              mode_map[mode];
          blk_col += 2;
        }
        break;
      case BLOCK_16X64:
        for (int32_t i = 0; i < 8; i++) {
          for (int32_t j = 0; j < 2; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 4;
          blk_row += 2;
        }
        break;
      case BLOCK_64X16:
        for (int32_t i = 0; i < 2; i++) {
          for (int32_t j = 0; j < 8; j++) {
            if (blk_col / 2 >= hw_filter_stat->cdef_stat->cdef_dir_cols ||
                blk_row / 2 >= hw_filter_stat->cdef_stat->cdef_dir_rows) {
              blk_col += 2;
              continue;
            }
            hw_filter_stat->cdef_stat
                ->cdef_dir_intra[blk_col / 2 +
                                 blk_row / 2 *
                                     hw_filter_stat->cdef_stat->cdef_dir_cols] =
                mode_map[mode];
            blk_col += 2;
          }
          blk_col -= 16;
          blk_row += 2;
        }
        break;
      default: break;
    }
  }
}

void hw_filter_stat_up_update_seq(int8_t enable_superres) {
  hw_filter_stat->enable_superres = enable_superres;
}

void hw_filter_stat_up_update_frame(int8_t use_superres) {
  hw_filter_stat->up_stat->enable_up = use_superres;
}

void hw_filter_stat_lr_update_frame(int8_t do_loop_restoration) {
  hw_filter_stat->lr_stat->enable_lr = do_loop_restoration;
}

// void hw_filter_stat_lr_set_num_units(int32_t unit_size) {
//   int32_t hpertile = AOMMAX(
//       (hw_filter_stat->common->frame_width + (unit_size >> 1)) / unit_size,
//       1);
//   int32_t vpertile = AOMMAX(
//       (hw_filter_stat->common->frame_height + (unit_size >> 1)) / unit_size,
//       1);
//   hw_filter_stat->lr_stat->lr_num = hpertile * vpertile;
void hw_filter_stat_lr_set_num_units(int32_t nunits) {
  hw_filter_stat->lr_stat->lr_num = nunits;
  if (hw_filter_stat->lr_stat->lr_type != NULL)
    free(hw_filter_stat->lr_stat->lr_type);
  hw_filter_stat->lr_stat->lr_type = (RestorationType *)calloc(
      hw_filter_stat->lr_stat->lr_num, sizeof(RestorationType));
}

void hw_filter_stat_lr_update_block(int32_t runit_idx,
                                    RestorationType restoration_type) {
  hw_filter_stat->lr_stat->lr_type[runit_idx] = restoration_type;
}

void hw_filter_stat_fgs_update_seq(int8_t film_grain_params_present) {
  hw_filter_stat->film_grain_params_present = film_grain_params_present;
}

void hw_filter_stat_fgs_update_frame(int8_t apply_graim) {
  hw_filter_stat->fgs_stat->enable_fgs = apply_graim;
}

void hw_filter_stat_dump() {
  FILE *fp = fopen(hw_global->output_fn, "w+");
  assertf(fp != NULL)
      fprintf(stderr, "fopen %s output failed\n", hw_global->output_fn);
  if (hw_deque_empty(hw_global->hw_filter_stat_deque)) return;
  fprintf(fp, "dump filter stat of %s\n\n", hw_global->input_fn);
  for (void **hw_filter_stat_iter =
           hw_deque_begin(hw_global->hw_filter_stat_deque);
       hw_filter_stat_iter != hw_deque_end(hw_global->hw_filter_stat_deque);
       hw_filter_stat_iter = hw_deque_iter_inc(hw_global->hw_filter_stat_deque,
                                               hw_filter_stat_iter)) {
    hw_filter_stat_t *hw_filter_stat_cur = *hw_filter_stat_iter;
    fprintf(fp, "frame_idx: %d\n", hw_filter_stat_cur->frame_idx);

    fprintf(fp, "lf_stat:\n");
    fprintf(fp, "  enable_lf: %d\n", hw_filter_stat_cur->lf_stat->enable_lf);
    fprintf(fp, "  filter_level: %d %d\n",
            hw_filter_stat_cur->lf_stat->filter_level[0],
            hw_filter_stat_cur->lf_stat->filter_level[1]);
    fprintf(fp, "  filter_length_vert:\n");
    for (int32_t y = 0; y < hw_filter_stat_cur->common->frame_height; y++) {
      fprintf(fp, "    ");
      for (int32_t x = 0; x < hw_filter_stat_cur->common->mi_cols; x++) {
        fprintf(
            fp, "%2d ",
            hw_filter_stat_cur->lf_stat
                ->filter_length_vert[y * hw_filter_stat_cur->common->mi_cols +
                                     x]);
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "  filter_length_horiz:\n");
    for (int32_t y = 0; y < hw_filter_stat_cur->common->mi_rows; y++) {
      fprintf(fp, "    ");
      for (int32_t x = 0; x < hw_filter_stat_cur->common->frame_width; x++) {
        fprintf(fp, "%2d ",
                hw_filter_stat_cur->lf_stat->filter_length_horiz
                    [y * hw_filter_stat_cur->common->frame_width + x]);
      }
      fprintf(fp, "\n");
    }

    fprintf(fp, "cdef_stat:\n");
    fprintf(fp, "  enable_cdef: %d\n",
            hw_filter_stat_cur->cdef_stat->enable_cdef);
    fprintf(fp, "  cdef_idx:\n");
    for (int32_t row = 0; row < hw_filter_stat_cur->common->fb_rows; row++) {
      fprintf(fp, "    ");
      for (int32_t col = 0; col < hw_filter_stat_cur->common->fb_cols; col++) {
        fprintf(
            fp, "%d",
            hw_filter_stat_cur->cdef_stat
                ->cdef_idx[row * hw_filter_stat_cur->common->fb_cols + col]);
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "  skip:\n");
    for (int32_t row = 0; row < hw_filter_stat_cur->common->fb_rows; row++) {
      fprintf(fp, "    ");
      for (int32_t col = 0; col < hw_filter_stat_cur->common->fb_cols; col++) {
        fprintf(fp, "%d",
                hw_filter_stat_cur->cdef_stat
                    ->skip[row * hw_filter_stat_cur->common->fb_cols + col]);
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "  cdef_dir:\n");
    for (int32_t row = 0; row < hw_filter_stat_cur->cdef_stat->cdef_dir_rows;
         row++) {
      for (int32_t col = 0; col < hw_filter_stat_cur->cdef_stat->cdef_dir_cols;
           col++) {
        fprintf(fp, "    ");
        fprintf(
            fp, "%d ",
            hw_filter_stat_cur->cdef_stat
                ->cdef_dir[row * hw_filter_stat_cur->cdef_stat->cdef_dir_cols +
                           col]);
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "  cdef_dir_intra:\n");
    for (int32_t row = 0; row < hw_filter_stat_cur->cdef_stat->cdef_dir_rows;
         row++) {
      for (int32_t col = 0; col < hw_filter_stat_cur->cdef_stat->cdef_dir_cols;
           col++) {
        fprintf(fp, "    ");
        fprintf(fp, "%d ",
                hw_filter_stat_cur->cdef_stat->cdef_dir_intra
                    [row * hw_filter_stat_cur->cdef_stat->cdef_dir_cols + col]);
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "  cdef_diff:\n");
    for (int32_t row = 0;
         row < hw_filter_stat_cur->cdef_stat->cdef_dir_rows;
         row++) {
      for (int32_t col = 0;
           col < hw_filter_stat_cur->cdef_stat->cdef_dir_cols;
           col++) {
        int32_t(*partial)[15] =
            (int32_t(*)[15])hw_filter_stat_cur->cdef_stat
                ->cdef_diff[row * hw_filter_stat_cur->cdef_stat->cdef_dir_cols +
                            col];
        if (partial != NULL) {
          for (int i = 0; i < 8; i++) {
            fprintf(fp, "    ");
            if (i == 0 || i == 4) {
              for (int j = 0; j < 15; j++) {
                fprintf(fp, "%d ", partial[i][j]);
              }
              fprintf(fp, "\n");
            } else if (i == 2 || i == 6) {
              for (int j = 0; j < 8; j++) {
                fprintf(fp, "%d ", partial[i][j]);
              }
              fprintf(fp, "\n");
            } else {
              for (int j = 0; j < 11; j++) {
                fprintf(fp, "%d ", partial[i][j]);
              }
              fprintf(fp, "\n");
            }
          }
        }
      }
      fprintf(fp, "\n");
    }

    fprintf(fp, "up_stat:\n");
    fprintf(fp, "  enable_superres: %d\n", hw_filter_stat_cur->enable_superres);
    fprintf(fp, "  enable_up: %d\n", hw_filter_stat_cur->up_stat->enable_up);

    fprintf(fp, "lr_stat:\n");
    fprintf(fp, "  enable_lr: %d\n", hw_filter_stat_cur->lr_stat->enable_lr);
    fprintf(fp, "  lr_type:\n");
    for (int i = 0; i < hw_filter_stat_cur->lr_stat->lr_num; i++) {
      fprintf(fp, "    %d", hw_filter_stat_cur->lr_stat->lr_type[i]);
    }
    fprintf(fp, "\n");

    fprintf(fp, "fgs_stat:\n");
    fprintf(fp, "  film_grain_params_present: %d\n",
            hw_filter_stat_cur->film_grain_params_present);
    fprintf(fp, "  enable_fgs: %d\n\n",
            hw_filter_stat_cur->fgs_stat->enable_fgs);
  }
}

void hw_filter_stat_frame_done() { hw_filter_stat_dump(); }
void hw_filter_stat_frame_next(int32_t frame_idx) {
  hw_filter_stat_t *hw_filter_stat_cur =
      hw_filter_stat_init_internal(frame_idx);
  if (hw_filter_stat != NULL && hw_filter_stat->lr_stat->lr_num != 0) {
    hw_filter_stat_cur->lr_stat->lr_num = hw_filter_stat->lr_stat->lr_num;
    hw_filter_stat_cur->lr_stat->lr_type = (RestorationType *)calloc(
        hw_filter_stat_cur->lr_stat->lr_num, sizeof(RestorationType));
  }
  hw_deque_push_back(hw_global->hw_filter_stat_deque, &hw_filter_stat_cur);
  hw_filter_stat = hw_filter_stat_cur;
}
