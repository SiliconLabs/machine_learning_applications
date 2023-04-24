/***************************************************************************//**
 * @file
 * @brief Initalization and main functionality for application
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef MAGIC_WAND_H
#define MAGIC_WAND_H
#include "config.h"
#include "bluetooth.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct model_output {
  float gesture[GESTURE_COUNT];
} model_output_t;
/***************************************************************************//**
 * @brief
 *   Initialize the application.
 ******************************************************************************/
void magic_wand_init(void);

/***************************************************************************//**
 * @brief
 *   Runs the data gathering and inference loop. This should be called
 *   repeatedly from the main function.
 ******************************************************************************/
void magic_wand_action(void);

#ifdef __cplusplus
}
#endif

#endif // MAGIC_WAND_H
