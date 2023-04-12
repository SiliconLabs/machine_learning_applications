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

#include "sl_htm.h"

static sl_htm_tm_t sl_htm_tm_instance;
static sl_htm_sp_t sl_htm_sp_instance;
static sl_htm_sdr_t sl_htm_sp_sdr;

void sl_htm_init(uint16_t input_width, uint16_t input_height, uint16_t width, uint16_t height, sl_htm_sp_parameters_t params_sp, sl_htm_tm_parameters_t params_tm)
{
  sl_htm_tm_instance.parameters = params_tm;
  sl_htm_sp_instance.parameters = params_sp;
  sl_htm_sp_init(&sl_htm_sp_instance, input_width, input_height, width, height);
  sl_htm_tm_init(&sl_htm_tm_instance, width, height);
  sl_htm_sdr_init(&sl_htm_sp_sdr, width, height);
}

float sl_htm_execute(sl_htm_sdr_t* input_sdr, bool learn)
{
  sl_htm_sp_execute(&sl_htm_sp_instance, input_sdr, &sl_htm_sp_sdr, learn);
  float anomaly_score = 0;
  anomaly_score = sl_htm_tm_execute(&sl_htm_tm_instance, &sl_htm_sp_sdr, learn);

  return anomaly_score;
}

sl_htm_sp_t* sl_htm_get_sp()
{
  return &sl_htm_sp_instance;
}
sl_htm_tm_t* sl_htm_get_tm()
{
  return &sl_htm_tm_instance;
}
