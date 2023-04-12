/***************************************************************************//**
 * @file
 * @brief HTM Implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef SL_HTM_SDR_H
#define SL_HTM_SDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sl_htm_utils.h"

typedef struct {
  bool *bits;
  uint8_t width;
  uint8_t height;
  uint16_t num_active_bits;
} sl_htm_sdr_t;

bool sl_htm_sdr_get_bit(sl_htm_sdr_t *sdr, uint16_t index);
void sl_htm_sdr_set_bit(sl_htm_sdr_t *sdr, uint16_t index, bool value);

void sl_htm_sdr_init(sl_htm_sdr_t *sdr, uint8_t width, uint8_t height);
void sl_htm_sdr_clear(sl_htm_sdr_t *sdr);
void sl_htm_sdr_randomize(sl_htm_sdr_t *sdr, float sparsity);

void sl_htm_sdr_insert(sl_htm_sdr_t* target_sdr, sl_htm_sdr_t* source_sdr, uint8_t x, uint8_t y);

void sl_htm_sdr_print(sl_htm_sdr_t* sdr);

size_t sl_htm_sdr_memory_size(sl_htm_sdr_t *sdr);

#ifdef __cplusplus
}
#endif

#endif // SL_HTM_SDR_H
