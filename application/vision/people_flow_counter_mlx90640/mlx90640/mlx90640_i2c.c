/***************************************************************************//**
 * @file mlx90640_i2c.c
 * @brief Application Logic Source File
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided \'as-is\', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 *
 * # EXPERIMENTAL QUALITY
 * This code has not been formally tested and is provided as-is. It is not
 * suitable for production environments. In addition, this code will not be
 * maintained and there may be no bug maintenance planned for these resources.
 * Silicon Labs may update projects from time to time.
 *
 ******************************************************************************/
#include "mlx90640_i2c.h"
#include "stdio.h"
#define MLX90640_DEFAULT_I2C_ADDR   0x33

static sl_i2cspm_t *i2cspm;
static uint8_t i2c_addr = MLX90640_DEFAULT_I2C_ADDR;

/***************************************************************************//**
 * Assigns an I2CSPM instance for the driver to use
 ******************************************************************************/
sl_status_t mlx90640_I2C_Init(sl_i2cspm_t *i2cspm_instance)
{
  i2cspm = i2cspm_instance;
  i2c_addr = MLX90640_DEFAULT_I2C_ADDR;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Issues an I2C general reset
 ******************************************************************************/
sl_status_t mlx90640_I2CGeneralReset(void)
{
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  uint8_t cmd[2] = { 0x00, 0x06 };

  seq.addr = i2c_addr;
  seq.flags = I2C_FLAG_WRITE;
  seq.buf[0].len = 2;
  seq.buf[0].data = cmd;

  ret = I2CSPM_Transfer(i2cspm, &seq);

  if (ret != i2cTransferDone) {
    return -1;
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Initiates an I2C read of the device
 ******************************************************************************/
sl_status_t mlx90640_I2CRead(uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
  uint8_t i2cData[1664] = { 0 };
  uint16_t counter = 0;
  uint16_t i = 0;
  uint16_t *p = data;
  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;

  uint8_t cmd[2] = { 0, 0 };
  cmd[0] = startAddress >> 8;
  cmd[1] = startAddress & 0x00FF;

  seq.addr = i2c_addr << 1;
  seq.flags = I2C_FLAG_WRITE_READ;
  seq.buf[0].len = 2;
  seq.buf[0].data = cmd;
  seq.buf[1].len = 2 * nMemAddressRead;
  seq.buf[1].data = i2cData;
  ret = I2CSPM_Transfer(i2cspm, &seq);

  if (ret != i2cTransferDone) {
    return SL_STATUS_FAIL;
  }

  for (counter = 0; counter < nMemAddressRead; counter++) {
    i = counter << 1;
    *p++ = (uint16_t)i2cData[i] * 256 + (uint16_t)i2cData[i + 1];
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Sets I2C base frequency to a given setting
 ******************************************************************************/
void mlx90640_I2CFreqSet(int freq)
{
  I2C_BusFreqSet(i2cspm, 0, freq, i2cClockHLRStandard); //Todo or 1000*freq?
}

/***************************************************************************//**
 * Initiates an I2C write to the device
 ******************************************************************************/
sl_status_t mlx90640_I2CWrite(uint16_t writeAddress, uint16_t data)
{
  uint8_t cmd[4] = { 0, 0, 0, 0 };
  static uint16_t dataCheck;

  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef i2c_ret;
  int ret;

  cmd[0] = writeAddress >> 8;
  cmd[1] = writeAddress & 0x00FF;
  cmd[2] = data >> 8;
  cmd[3] = data & 0x00FF;

  seq.addr = i2c_addr << 1;
  seq.flags = I2C_FLAG_WRITE;
  seq.buf[0].len = 4;
  seq.buf[0].data = cmd;

  i2c_ret = I2CSPM_Transfer(i2cspm, &seq);

  if (i2c_ret != i2cTransferDone) {
    return SL_STATUS_FAIL;
  }

  ret = mlx90640_I2CRead(writeAddress, 1, &dataCheck);

  if (ret != 0) {
    return SL_STATUS_FAIL;
  }

  if (dataCheck != data) {
    return SL_STATUS_FAIL;   //Todo data error?
  }

  return SL_STATUS_OK;
}
