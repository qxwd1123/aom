/*
 *  Copyright (c) 2019, Alliance for Open Media. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <arm_neon.h>

#include "config/aom_dsp_rtcd.h"
#include "aom/aom_integer.h"
#include "aom_dsp/arm/mem_neon.h"
#include "aom_dsp/arm/transpose_neon.h"

static INLINE void hadamard_4x4_one_pass(int16x4_t *a0, int16x4_t *a1,
                                         int16x4_t *a2, int16x4_t *a3) {
  const int16x4_t b0 = vhadd_s16(*a0, *a1);
  const int16x4_t b1 = vhsub_s16(*a0, *a1);
  const int16x4_t b2 = vhadd_s16(*a2, *a3);
  const int16x4_t b3 = vhsub_s16(*a2, *a3);

  *a0 = vadd_s16(b0, b2);
  *a1 = vadd_s16(b1, b3);
  *a2 = vsub_s16(b0, b2);
  *a3 = vsub_s16(b1, b3);
}

void aom_hadamard_4x4_neon(const int16_t *src_diff, ptrdiff_t src_stride,
                           tran_low_t *coeff) {
  int16x4_t a0 = vld1_s16(src_diff);
  int16x4_t a1 = vld1_s16(src_diff + src_stride);
  int16x4_t a2 = vld1_s16(src_diff + 2 * src_stride);
  int16x4_t a3 = vld1_s16(src_diff + 3 * src_stride);

  hadamard_4x4_one_pass(&a0, &a1, &a2, &a3);

  transpose_s16_4x4d(&a0, &a1, &a2, &a3);

  hadamard_4x4_one_pass(&a0, &a1, &a2, &a3);

  store_s16_to_tran_low(coeff, a0);
  store_s16_to_tran_low(coeff + 4, a1);
  store_s16_to_tran_low(coeff + 8, a2);
  store_s16_to_tran_low(coeff + 12, a3);
}

static void hadamard8x8_one_pass(int16x8_t *a0, int16x8_t *a1, int16x8_t *a2,
                                 int16x8_t *a3, int16x8_t *a4, int16x8_t *a5,
                                 int16x8_t *a6, int16x8_t *a7) {
  const int16x8_t b0 = vaddq_s16(*a0, *a1);
  const int16x8_t b1 = vsubq_s16(*a0, *a1);
  const int16x8_t b2 = vaddq_s16(*a2, *a3);
  const int16x8_t b3 = vsubq_s16(*a2, *a3);
  const int16x8_t b4 = vaddq_s16(*a4, *a5);
  const int16x8_t b5 = vsubq_s16(*a4, *a5);
  const int16x8_t b6 = vaddq_s16(*a6, *a7);
  const int16x8_t b7 = vsubq_s16(*a6, *a7);

  const int16x8_t c0 = vaddq_s16(b0, b2);
  const int16x8_t c1 = vaddq_s16(b1, b3);
  const int16x8_t c2 = vsubq_s16(b0, b2);
  const int16x8_t c3 = vsubq_s16(b1, b3);
  const int16x8_t c4 = vaddq_s16(b4, b6);
  const int16x8_t c5 = vaddq_s16(b5, b7);
  const int16x8_t c6 = vsubq_s16(b4, b6);
  const int16x8_t c7 = vsubq_s16(b5, b7);

  *a0 = vaddq_s16(c0, c4);
  *a1 = vsubq_s16(c2, c6);
  *a2 = vsubq_s16(c0, c4);
  *a3 = vaddq_s16(c2, c6);
  *a4 = vaddq_s16(c3, c7);
  *a5 = vsubq_s16(c3, c7);
  *a6 = vsubq_s16(c1, c5);
  *a7 = vaddq_s16(c1, c5);
}

void aom_hadamard_8x8_neon(const int16_t *src_diff, ptrdiff_t src_stride,
                           tran_low_t *coeff) {
  int16x8_t a0 = vld1q_s16(src_diff);
  int16x8_t a1 = vld1q_s16(src_diff + src_stride);
  int16x8_t a2 = vld1q_s16(src_diff + 2 * src_stride);
  int16x8_t a3 = vld1q_s16(src_diff + 3 * src_stride);
  int16x8_t a4 = vld1q_s16(src_diff + 4 * src_stride);
  int16x8_t a5 = vld1q_s16(src_diff + 5 * src_stride);
  int16x8_t a6 = vld1q_s16(src_diff + 6 * src_stride);
  int16x8_t a7 = vld1q_s16(src_diff + 7 * src_stride);

  hadamard8x8_one_pass(&a0, &a1, &a2, &a3, &a4, &a5, &a6, &a7);

  transpose_s16_8x8(&a0, &a1, &a2, &a3, &a4, &a5, &a6, &a7);

  hadamard8x8_one_pass(&a0, &a1, &a2, &a3, &a4, &a5, &a6, &a7);

  // Skip the second transpose because it is not required.

  store_s16q_to_tran_low(coeff + 0, a0);
  store_s16q_to_tran_low(coeff + 8, a1);
  store_s16q_to_tran_low(coeff + 16, a2);
  store_s16q_to_tran_low(coeff + 24, a3);
  store_s16q_to_tran_low(coeff + 32, a4);
  store_s16q_to_tran_low(coeff + 40, a5);
  store_s16q_to_tran_low(coeff + 48, a6);
  store_s16q_to_tran_low(coeff + 56, a7);
}

void aom_hadamard_lp_8x8_neon(const int16_t *src_diff, ptrdiff_t src_stride,
                              int16_t *coeff) {
  int16x8_t a0 = vld1q_s16(src_diff);
  int16x8_t a1 = vld1q_s16(src_diff + src_stride);
  int16x8_t a2 = vld1q_s16(src_diff + 2 * src_stride);
  int16x8_t a3 = vld1q_s16(src_diff + 3 * src_stride);
  int16x8_t a4 = vld1q_s16(src_diff + 4 * src_stride);
  int16x8_t a5 = vld1q_s16(src_diff + 5 * src_stride);
  int16x8_t a6 = vld1q_s16(src_diff + 6 * src_stride);
  int16x8_t a7 = vld1q_s16(src_diff + 7 * src_stride);

  hadamard8x8_one_pass(&a0, &a1, &a2, &a3, &a4, &a5, &a6, &a7);

  transpose_s16_8x8(&a0, &a1, &a2, &a3, &a4, &a5, &a6, &a7);

  hadamard8x8_one_pass(&a0, &a1, &a2, &a3, &a4, &a5, &a6, &a7);

  // Skip the second transpose because it is not required.

  vst1q_s16(coeff + 0, a0);
  vst1q_s16(coeff + 8, a1);
  vst1q_s16(coeff + 16, a2);
  vst1q_s16(coeff + 24, a3);
  vst1q_s16(coeff + 32, a4);
  vst1q_s16(coeff + 40, a5);
  vst1q_s16(coeff + 48, a6);
  vst1q_s16(coeff + 56, a7);
}

void aom_hadamard_lp_8x8_dual_neon(const int16_t *src_diff,
                                   ptrdiff_t src_stride, int16_t *coeff) {
  for (int i = 0; i < 2; i++) {
    aom_hadamard_lp_8x8_neon(src_diff + (i * 8), src_stride, coeff + (i * 64));
  }
}

void aom_hadamard_lp_16x16_neon(const int16_t *src_diff, ptrdiff_t src_stride,
                                int16_t *coeff) {
  /* Rearrange 16x16 to 8x32 and remove stride.
   * Top left first. */
  aom_hadamard_lp_8x8_neon(src_diff + 0 + 0 * src_stride, src_stride,
                           coeff + 0);
  /* Top right. */
  aom_hadamard_lp_8x8_neon(src_diff + 8 + 0 * src_stride, src_stride,
                           coeff + 64);
  /* Bottom left. */
  aom_hadamard_lp_8x8_neon(src_diff + 0 + 8 * src_stride, src_stride,
                           coeff + 128);
  /* Bottom right. */
  aom_hadamard_lp_8x8_neon(src_diff + 8 + 8 * src_stride, src_stride,
                           coeff + 192);

  for (int i = 0; i < 64; i += 8) {
    const int16x8_t a0 = vld1q_s16(coeff + 0);
    const int16x8_t a1 = vld1q_s16(coeff + 64);
    const int16x8_t a2 = vld1q_s16(coeff + 128);
    const int16x8_t a3 = vld1q_s16(coeff + 192);

    const int16x8_t b0 = vhaddq_s16(a0, a1);
    const int16x8_t b1 = vhsubq_s16(a0, a1);
    const int16x8_t b2 = vhaddq_s16(a2, a3);
    const int16x8_t b3 = vhsubq_s16(a2, a3);

    const int16x8_t c0 = vaddq_s16(b0, b2);
    const int16x8_t c1 = vaddq_s16(b1, b3);
    const int16x8_t c2 = vsubq_s16(b0, b2);
    const int16x8_t c3 = vsubq_s16(b1, b3);

    vst1q_s16(coeff + 0, c0);
    vst1q_s16(coeff + 64, c1);
    vst1q_s16(coeff + 128, c2);
    vst1q_s16(coeff + 192, c3);

    coeff += 8;
  }
}

void aom_hadamard_16x16_neon(const int16_t *src_diff, ptrdiff_t src_stride,
                             tran_low_t *coeff) {
  /* Rearrange 16x16 to 8x32 and remove stride.
   * Top left first. */
  aom_hadamard_8x8_neon(src_diff + 0 + 0 * src_stride, src_stride, coeff + 0);
  /* Top right. */
  aom_hadamard_8x8_neon(src_diff + 8 + 0 * src_stride, src_stride, coeff + 64);
  /* Bottom left. */
  aom_hadamard_8x8_neon(src_diff + 0 + 8 * src_stride, src_stride, coeff + 128);
  /* Bottom right. */
  aom_hadamard_8x8_neon(src_diff + 8 + 8 * src_stride, src_stride, coeff + 192);

  for (int i = 0; i < 64; i += 16) {
    const int16x8_t a00 = load_tran_low_to_s16q(coeff + 0);
    const int16x8_t a01 = load_tran_low_to_s16q(coeff + 64);
    const int16x8_t a02 = load_tran_low_to_s16q(coeff + 128);
    const int16x8_t a03 = load_tran_low_to_s16q(coeff + 192);

    const int16x8_t b00 = vhaddq_s16(a00, a01);
    const int16x8_t b01 = vhsubq_s16(a00, a01);
    const int16x8_t b02 = vhaddq_s16(a02, a03);
    const int16x8_t b03 = vhsubq_s16(a02, a03);

    const int16x8_t c00 = vaddq_s16(b00, b02);
    const int16x8_t c01 = vaddq_s16(b01, b03);
    const int16x8_t c02 = vsubq_s16(b00, b02);
    const int16x8_t c03 = vsubq_s16(b01, b03);

    const int16x8_t a10 = load_tran_low_to_s16q(coeff + 8 + 0);
    const int16x8_t a11 = load_tran_low_to_s16q(coeff + 8 + 64);
    const int16x8_t a12 = load_tran_low_to_s16q(coeff + 8 + 128);
    const int16x8_t a13 = load_tran_low_to_s16q(coeff + 8 + 192);

    const int16x8_t b10 = vhaddq_s16(a10, a11);
    const int16x8_t b11 = vhsubq_s16(a10, a11);
    const int16x8_t b12 = vhaddq_s16(a12, a13);
    const int16x8_t b13 = vhsubq_s16(a12, a13);

    const int16x8_t c10 = vaddq_s16(b10, b12);
    const int16x8_t c11 = vaddq_s16(b11, b13);
    const int16x8_t c12 = vsubq_s16(b10, b12);
    const int16x8_t c13 = vsubq_s16(b11, b13);

    store_s16_to_tran_low(coeff + 0 + 0, vget_low_s16(c00));
    store_s16_to_tran_low(coeff + 0 + 4, vget_low_s16(c10));
    store_s16_to_tran_low(coeff + 0 + 8, vget_high_s16(c00));
    store_s16_to_tran_low(coeff + 0 + 12, vget_high_s16(c10));

    store_s16_to_tran_low(coeff + 64 + 0, vget_low_s16(c01));
    store_s16_to_tran_low(coeff + 64 + 4, vget_low_s16(c11));
    store_s16_to_tran_low(coeff + 64 + 8, vget_high_s16(c01));
    store_s16_to_tran_low(coeff + 64 + 12, vget_high_s16(c11));

    store_s16_to_tran_low(coeff + 128 + 0, vget_low_s16(c02));
    store_s16_to_tran_low(coeff + 128 + 4, vget_low_s16(c12));
    store_s16_to_tran_low(coeff + 128 + 8, vget_high_s16(c02));
    store_s16_to_tran_low(coeff + 128 + 12, vget_high_s16(c12));

    store_s16_to_tran_low(coeff + 192 + 0, vget_low_s16(c03));
    store_s16_to_tran_low(coeff + 192 + 4, vget_low_s16(c13));
    store_s16_to_tran_low(coeff + 192 + 8, vget_high_s16(c03));
    store_s16_to_tran_low(coeff + 192 + 12, vget_high_s16(c13));

    coeff += 16;
  }
}

void aom_hadamard_32x32_neon(const int16_t *src_diff, ptrdiff_t src_stride,
                             tran_low_t *coeff) {
  /* Top left first. */
  aom_hadamard_16x16_neon(src_diff + 0 + 0 * src_stride, src_stride, coeff + 0);
  /* Top right. */
  aom_hadamard_16x16_neon(src_diff + 16 + 0 * src_stride, src_stride,
                          coeff + 256);
  /* Bottom left. */
  aom_hadamard_16x16_neon(src_diff + 0 + 16 * src_stride, src_stride,
                          coeff + 512);
  /* Bottom right. */
  aom_hadamard_16x16_neon(src_diff + 16 + 16 * src_stride, src_stride,
                          coeff + 768);

  for (int i = 0; i < 256; i += 4) {
    const int32x4_t a0 = vld1q_s32(coeff);
    const int32x4_t a1 = vld1q_s32(coeff + 256);
    const int32x4_t a2 = vld1q_s32(coeff + 512);
    const int32x4_t a3 = vld1q_s32(coeff + 768);

    const int32x4_t b0 = vshrq_n_s32(vaddq_s32(a0, a1), 2);
    const int32x4_t b1 = vshrq_n_s32(vsubq_s32(a0, a1), 2);
    const int32x4_t b2 = vshrq_n_s32(vaddq_s32(a2, a3), 2);
    const int32x4_t b3 = vshrq_n_s32(vsubq_s32(a2, a3), 2);

    const int32x4_t c0 = vaddq_s32(b0, b2);
    const int32x4_t c1 = vaddq_s32(b1, b3);
    const int32x4_t c2 = vsubq_s32(b0, b2);
    const int32x4_t c3 = vsubq_s32(b1, b3);

    vst1q_s32(coeff + 0, c0);
    vst1q_s32(coeff + 256, c1);
    vst1q_s32(coeff + 512, c2);
    vst1q_s32(coeff + 768, c3);

    coeff += 4;
  }
}
