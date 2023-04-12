/***************************************************************************//**
 * @file
 * @brief Top level application functions
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

#include <stdio.h>
#include "accelerometer.h"
#include "sl_status.h"
#include "sl_htm.h"
#include "sl_htm_utils.h"

// Accelerometer data
static acc_data_t imu_data_prev = { 0, 0, 0 };
static acc_data_t imu_data_current = { 0, 0, 0 };
static acc_data_t imu_data_normalized = { 0, 0, 0 };

// Flag to indicate if there is new data to process
static bool data_to_process = false;

// Called when the IMU has data available using gpio interrupt.
static void on_data_available(uint8_t int_id, void *ctx)
{
  (void) int_id;
  (void) ctx;
  imu_data_prev = imu_data_current;
  sl_status_t status = accelerometer_read(&imu_data_current);
  if (status == SL_STATUS_FAIL) {
    return;
  }
  data_to_process = true;
}

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/

// Input SDR width, must be divisible by 3
#define SDR_WIDTH 27

// SDR instances
static sl_htm_sdr_t sdr_x;
static sl_htm_sdr_t sdr_y;
static sl_htm_sdr_t sdr_z;
static sl_htm_sdr_t input_sdr;
static sl_htm_sdr_t sp_sdr;

// SP and TM instances
static sl_htm_tm_t tm;
static sl_htm_sp_t sp;
void app_init(void)
{
  // Initialize SDRs, x y and z will be encoded into one input sdr, hence the division by 3.
  sl_htm_sdr_init(&sdr_x, SDR_WIDTH, SDR_WIDTH / 3);
  sl_htm_sdr_init(&sdr_y, SDR_WIDTH, SDR_WIDTH / 3);
  sl_htm_sdr_init(&sdr_z, SDR_WIDTH, SDR_WIDTH / 3);
  sl_htm_sdr_init(&input_sdr, SDR_WIDTH, SDR_WIDTH);
  sl_htm_sdr_init(&sp_sdr, 6, 6);

  // Initialize SP and TM parameters
  sl_htm_sp_init_default_params(&sp.parameters);
  sp.parameters.potential_radius = 8;
  sp.parameters.potential_pct = 0.2f;
  sp.parameters.sparsity = 0.3f;
  sp.parameters.permanence_threshold = 50;
  sp.parameters.permanence_increment = 10;
  sp.parameters.permanence_decrement = 10;

  sl_htm_tm_init_default_params(&tm.parameters);
  tm.parameters.synapse_permanence_increment = 30;
  tm.parameters.synapse_permanence_decrement = 30;
  tm.parameters.num_cells_per_column = 4;

  // Initialize SP and TM
  sl_htm_sp_init(&sp, SDR_WIDTH, SDR_WIDTH, sp_sdr.width, sp_sdr.height);

  sl_htm_tm_init(&tm, sp_sdr.width, sp_sdr.height);

  // Initialize accelerometer
  sl_status_t status = accelerometer_setup(on_data_available);
  if (status != SL_STATUS_OK) {
    printf("FAIL: accelerometer_init() returned 0x%04lx\n", status);
    EFM_ASSERT(false);
  }
}

// Track statistics
static uint16_t t = 0;
static float anomaly_sum = 0;
static size_t anomaly_count = 0;
/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  // Check if there is new data to process
  if (!data_to_process) {
    return;
  }

  // Increment time
  t++;

  // Process data
  imu_data_normalized.x = imu_data_current.x - imu_data_prev.x;
  imu_data_normalized.y = imu_data_current.y - imu_data_prev.y;
  imu_data_normalized.z = imu_data_current.z - imu_data_prev.z;

  // Normalize data to [-1, 1]
  imu_data_normalized.x /= 4000;
  imu_data_normalized.y /= 4000;
  imu_data_normalized.z /= 4000;

  // Encode data
  sl_htm_encoder_simple_number(imu_data_normalized.x, -1.0f, 1.0f, 9, &sdr_x);
  sl_htm_encoder_simple_number(imu_data_normalized.y, -1.0f, 1.0f, 9, &sdr_y);
  sl_htm_encoder_simple_number(imu_data_normalized.z, -1.0f, 1.0f, 9, &sdr_z);

  // Clear input sdr
  sl_htm_sdr_clear(&input_sdr);

  // Insert data into input sdr
  sl_htm_sdr_insert(&input_sdr, &sdr_x, 0, SDR_WIDTH / 3 * 0);
  sl_htm_sdr_insert(&input_sdr, &sdr_y, 0, SDR_WIDTH / 3 * 1);
  sl_htm_sdr_insert(&input_sdr, &sdr_z, 0, SDR_WIDTH / 3 * 2);

  // Execute SP and TM
  float anomaly_score = 0;
  sl_htm_sp_execute(&sp, &input_sdr, &sp_sdr, true);
  anomaly_score = sl_htm_tm_execute(&tm, &sp_sdr, true);

  // Update stats
  anomaly_sum += anomaly_score;
  anomaly_count++;

  // Print stats every 10th iteration
  if (t % 10 == 0) {
    float anomaly_avg = anomaly_sum / anomaly_count;

    // Print stats
    sl_htm_sp_print(&sp);
    printf("SDR Memory size: %u\n", sl_htm_sdr_memory_size(&input_sdr));
    printf("SP Memory size: %u\n", sl_htm_sp_memory_size(&sp));
    printf("TM Memory size: %u\n", sl_htm_tm_memory_size(&tm));
    printf("[t=%u] Anomaly score: %f [x: %f, y: %f, z: %f]\n", t, anomaly_avg, imu_data_normalized.x, imu_data_normalized.y, imu_data_normalized.z);

    // This one is a special case of the above, and is picked up by the visualization script.
    printf("anom_score:%f\n", anomaly_avg);

    // Reset stats
    anomaly_sum = 0;
    anomaly_count = 0;
  }

  // Data has been processed, wait until new data is available
  data_to_process = false;
}
void HardFault_Handler(void)
{
  printf("FAIL: Hard Fault\n");
  while (1);
}
