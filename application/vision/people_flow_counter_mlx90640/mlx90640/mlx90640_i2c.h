/***************************************************************************//**
 * @file mlx90640_i2c.h
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

#ifndef MLX90640_I2C_H
#define MLX90640_I2C_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "sl_status.h"
#include "sl_i2cspm.h"

/***************************************************************************//**
 * @brief
 * Assigns an I2CSPM instance for the driver to use
 *
 * @param[in] i2cspm_instace - Pointer to the I2CSPM instance
 ******************************************************************************/
sl_status_t mlx90640_I2C_Init(sl_i2cspm_t *i2cspm_instance);

/***************************************************************************//**
 * @brief
 * Issues an I2C general reset
 ******************************************************************************/
sl_status_t mlx90640_I2CGeneralReset(void);

/***************************************************************************//**
 * @brief
 * Initiates an I2C read of the device
 *
 * @param[in] startAddress - EEPROM memory address of the device to read out from
 * @param[in] nMemAddressRead - Length of the requested data
 * @param[out] data - pointer to an array where the received data will be stored
 ******************************************************************************/
sl_status_t mlx90640_I2CRead(uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);

/***************************************************************************//**
 * @brief
 * Initiates an I2C write to the device
 *
 * @param[in] writeAddress - EEPROM memory address of the device to write to
 * @param[in] data - 16bit data to send to the device
 ******************************************************************************/
sl_status_t mlx90640_I2CWrite(uint16_t writeAddress, uint16_t data);

/***************************************************************************//**
 * @brief
 * Sets I2C base frequency to a given setting
 *
 * @param[in] freq - new frequency in Hz to set for the I2C base frequency
 ******************************************************************************/
void mlx90640_I2CFreqSet(int freq);
#ifdef __cplusplus
}
#endif
#endif // MLX90640_I2C_H
