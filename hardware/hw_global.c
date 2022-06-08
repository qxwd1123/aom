#include "hardware/hw_global.h"
#include "hardware/hw_filter_stat.h"
#include <unistd.h>
hw_global_t *hw_global;

void hw_global_init() {
  hw_global = (hw_global_t *)malloc(sizeof(hw_global_t));
  hw_global->frame_idx = 0;
  hw_global->input_fn = NULL;
  hw_global->output_fn = NULL;
  hw_global->hw_common = (hw_common_t *)malloc(sizeof(hw_common_t));
  hw_global->hw_common->fb_cols = 0;
  hw_global->hw_common->fb_rows = 0;
  hw_global->hw_common->fb_num = 0;
  hw_global->hw_filter_stat_deque = hw_filter_stat_init();
}

void hw_global_destroy() {
  hw_filter_stat_destroy();
  free(hw_global->input_fn);
  free(hw_global->output_fn);
  free(hw_global->hw_common);
  free(hw_global);
}

void hw_global_set_input_fn(char *input_fn) {
  hw_global->input_fn = malloc(strlen(input_fn) + 1);
  memcpy(hw_global->input_fn, input_fn, strlen(input_fn) + 1);
  char *pfnc = input_fn;
  for (char *pCur = input_fn; *pCur != '\0'; pCur++) {
    if (*pCur == '/') pfnc = pCur + 1;
  }
  char cwd[PATH_MAX] = { 0 };
  assertf(getcwd(cwd, sizeof(cwd)) != NULL) fprintf(stderr, "getcwd failed\n");
  hw_global->output_fn =
      calloc(strlen(pfnc) + strlen(cwd) + 13 + 1, sizeof(char));
  strcat(hw_global->output_fn, cwd);
  strcat(hw_global->output_fn, "/filter_stat/");
  strcat(hw_global->output_fn, pfnc);
}

void hw_global_set_frame_params(int32_t frame_width, int32_t frame_height) {
  hw_global->hw_common->frame_width = ALIGN_POWER_OF_TWO(frame_width, 3);
  hw_global->hw_common->frame_height = ALIGN_POWER_OF_TWO(frame_height, 3);
}

void hw_global_set_mi_params(int32_t mi_rows, int32_t mi_cols) {
  hw_global->hw_common->mi_cols = mi_cols;
  hw_global->hw_common->mi_rows = mi_rows;
  hw_global->hw_common->fb_cols = (mi_cols + MI_SIZE_64X64 - 1) / MI_SIZE_64X64;
  hw_global->hw_common->fb_rows = (mi_rows + MI_SIZE_64X64 - 1) / MI_SIZE_64X64;
  hw_global->hw_common->fb_num =
      hw_global->hw_common->fb_cols * hw_global->hw_common->fb_rows;
}

void hw_global_frame_start() {
  hw_global->frame_idx = 0;
  hw_global->hw_filter_stat_deque = hw_filter_stat_init();
}

void hw_global_frame_next() {
  hw_filter_stat_frame_next(hw_global->frame_idx);
  hw_global->frame_idx++;
}

void hw_global_frame_done() { hw_filter_stat_frame_done(); }
