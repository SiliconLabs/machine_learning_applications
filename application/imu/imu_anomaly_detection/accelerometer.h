/***************************************************************************//**
 * @file
 * @brief Functionality for reading accelerometer data from IMU
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

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include "sl_status.h"
#include "gpiointerrupt.h"
#include "sl_imu.h"
#include "em_gpio.h"
#include "em_core.h"
#include "app_log.h"

// The expected accelerometer data sample frequency
#define ACCELEROMETER_FREQ      25
#define ACCELEROMETER_CHANNELS   3

// Accelerometer data structure used by the model
typedef struct {
  float x;
  float y;
  float z;
} acc_data_t;

/***************************************************************************//**
 * @brief
 *   Configure accelerometer to read data regularly to an internal buffer.
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 ******************************************************************************/
sl_status_t accelerometer_setup(GPIOINT_IrqCallbackPtrExt_t callbackPtr);

/***************************************************************************//**
 * @brief
 *   Read data from accelerometer buffer into the machine model input buffer.
 *
 * @param dst
 *   Machine learning model input buffer.
 *
 * @param n
 *   Number of elements to read
 *
 * @return
 *   SL_STATUS_OK on success, other value on failure.
 ******************************************************************************/
sl_status_t accelerometer_read(acc_data_t* dst);

#endif // ACCELEROMETER_H
