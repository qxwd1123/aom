#ifndef __HW_UTILS_H
#define __HW_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <inttypes.h>
#include "hardware/hw_global.h"

#define assertf(x) for (; !(x); assert(x))

typedef struct file_fifo_t {
  int32_t size;
  int8_t *queue;
  int32_t cnt;
} file_fifo_t;

file_fifo_t *ff_new(const int32_t size);

void ff_close(file_fifo_t *const ff);

void ff_clear(file_fifo_t *const ff);

bool ff_push(file_fifo_t *const ff, const void *const record);

bool ff_pop(file_fifo_t *const ff, void *const record);

typedef struct hw_deque_t {
  int8_t *data;
  size_t item_size;
  int32_t size;
  int32_t begin;
  int32_t end;
} hw_deque_t;

#define hw_deque_new(type, size) __hw_deque_new(sizeof(type), size)
hw_deque_t *__hw_deque_new(size_t item_size, int32_t size);
void hw_deque_destroy(hw_deque_t *deque);
void *hw_deque_begin(const hw_deque_t *deque);
void *hw_deque_end(const hw_deque_t *deque);
void *hw_deque_iter_inc(hw_deque_t *deque, void *ptr);
void *hw_deque_iter_dec(hw_deque_t *deque, void *ptr);
void hw_deque_clear(hw_deque_t *deque);
hw_res_e hw_deque_push_back(hw_deque_t *deque, const void *data);
hw_res_e hw_deque_push_front(hw_deque_t *deque, const void *data);
hw_res_e hw_deque_pop_front(hw_deque_t *deque, void *data);
hw_res_e hw_deque_pop_back(hw_deque_t *deque, void *data);
int32_t hw_deque_size(const hw_deque_t *deque);
bool hw_deque_full(const hw_deque_t *deque);
bool hw_deque_empty(const hw_deque_t *deque);

#ifdef __cplusplus
}
#endif

#endif /* __CQUEUE_H */
