#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "hardware/hw_utils.h"

file_fifo_t *ff_new(const int32_t size) {
  file_fifo_t *ff = (file_fifo_t *)malloc(sizeof(file_fifo_t));
  if (ff == NULL) {
    return NULL;
  }
  ff->queue = (int8_t *)malloc(size);
  if (ff->queue == NULL) {
    ff->size = 0;
    return NULL;
  } else {
    ff->size = size;
    ff_clear(ff);
    return ff;
  }
}

void ff_close(file_fifo_t *const ff) {
  if (ff->queue != NULL) {
    free(ff->queue);
  }
  ff->cnt = 0;
  ff->size = 0;
}

void ff_clear(file_fifo_t *const ff) {
  ff->cnt = 0;
  memset(ff->queue, 0, ff->size);
}

bool ff_add_byte(file_fifo_t *const ff, const void *const record,
                 const int32_t record_sz) {
  if (ff->size < (ff->cnt + record_sz)) {
    int32_t new_sz = 1 << (int32_t)ceil(ff->cnt + log2(record_sz));
    ff->queue = realloc(ff->queue, new_sz);
    if (ff->queue == NULL) {
      return false;
    }
  }

  int8_t *const p = ff->queue + ff->cnt;
  memcpy(p, record, record_sz);
  ff->cnt += record_sz;

  return true;
}

bool ff_dump(file_fifo_t *const ff, FILE *const fp) {
  if (ff->cnt == 0) {
    return false;
  }
  fwrite(ff->queue, ff->cnt, 1, fp);
  ff_clear(ff);
  return true;
}

hw_deque_t *__hw_deque_new(size_t item_size, int32_t size) {
  hw_deque_t *deque;

  if (size < 2) {
    return NULL;
  }
  deque = malloc(sizeof(hw_deque_t));

  if (!deque) {
    return NULL;
  }

  deque->data = calloc((size + 1) * item_size, sizeof(int8_t));

  if (!deque->data) {
    free(deque);
    return NULL;
  }

  deque->item_size = item_size;
  deque->size = size + 1;
  deque->begin = 0;
  deque->end = 0;
  return deque;
}

void hw_deque_destroy(hw_deque_t *deque) {
  free(deque->data);
  free(deque);
}

void *hw_deque_begin(const hw_deque_t *deque) {
  if (hw_deque_empty(deque)) {
    return NULL;
  }

  int8_t *p_data = (int8_t *)deque->data;
  return p_data + deque->begin * deque->item_size;
}

void *hw_deque_end(const hw_deque_t *deque) {
  if (hw_deque_empty(deque)) {
    return NULL;
  }

  int8_t *p_data = (int8_t *)deque->data;
  return p_data + deque->end * deque->item_size;
}

void *hw_deque_iter_inc(hw_deque_t *deque, void *ptr) {
  int8_t *p_data = deque->data;
  assertf(ptr >= (void*)p_data && ptr <= (void*)(p_data + deque->size * deque->item_size))
    fprintf(stderr, "ptr is not belong to deque\n");
  int index = ((int8_t *)ptr - p_data) / deque->item_size;
  index = (index + 1) % deque->size;
  return p_data + index * deque->item_size;
}

void *hw_deque_iter_dec(hw_deque_t *deque, void *ptr) {
  int8_t *p_data = deque->data;
  assertf(ptr >= (void*)p_data && ptr <= (void*)(p_data + deque->size * deque->item_size))
    fprintf(stderr, "ptr is not belong to deque\n");
  int index = ((int8_t *)ptr - p_data) / deque->item_size;
  index = (index + deque->size - 1) % deque->size;
  return p_data + index * deque->item_size;
}

void hw_deque_clear(hw_deque_t *deque) {
  deque->begin = deque->end = 0;
  memset(deque->data, 0, (deque->size + 1) * deque->item_size);
}

hw_res_e hw_deque_push_front(hw_deque_t *deque, const void *data) {
  assertf(data != NULL)
    fprintf(stderr, "data to push is NULL\n");
  int8_t *p_data = deque->data;

  if (hw_deque_full(deque)) {
    return RET_FAILED;
  }

  deque->begin = (deque->begin + deque->size - 1) % deque->size;
  p_data += deque->begin * deque->item_size;
  memcpy(p_data, data, deque->item_size);

  return RET_SUCCESS;
}

hw_res_e hw_deque_push_back(hw_deque_t *deque, const void *data) {
  assertf(data != NULL)
    fprintf(stderr, "data to push is NULL\n");
  int8_t *p_data = deque->data;

  if (hw_deque_full(deque)) {
    return RET_FAILED;
  }

  p_data += deque->end * deque->item_size;
  memcpy(p_data, data, deque->item_size);
  deque->end = (deque->end + 1) % deque->size;

  return RET_SUCCESS;
}

hw_res_e hw_deque_pop_front(hw_deque_t *deque, void *data) {
  assertf(data != NULL)
    fprintf(stderr, "data to push is NULL\n");
  int8_t *p_data = deque->data;

  if (hw_deque_empty(deque)) {
    return RET_FAILED;
  }

  p_data += deque->begin * deque->item_size;
  memcpy(data, p_data, deque->item_size);
  deque->begin = (deque->begin + 1) % deque->size;

  return RET_SUCCESS;
}

hw_res_e hw_deque_pop_back(hw_deque_t *deque, void *data) {
  assertf(data != NULL)
    fprintf(stderr, "data to push is NULL\n");
  int8_t *p_data = deque->data;

  if (hw_deque_empty(deque)) {
    return RET_FAILED;
  }

  deque->end = (deque->end + deque->size - 1) % deque->size;
  p_data += deque->end * deque->item_size;
  memcpy(data, p_data, deque->item_size);

  return RET_SUCCESS;
}

int32_t hw_deque_size(const hw_deque_t *deque) {
  return (deque->end + deque->size - deque->begin) % deque->size;
}

bool hw_deque_full(const hw_deque_t *deque) {
  return hw_deque_size(deque) == (deque->size - 1);
}

bool hw_deque_empty(const hw_deque_t *deque) { return !hw_deque_size(deque); }
