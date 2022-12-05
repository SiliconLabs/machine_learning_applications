/***************************************************************************//**
 * @file mlx90640.c
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

#include "mlx90640.h"
#include "mlx90640_i2c.h"
#include "math.h"
#include "app_log.h"
#include "sl_sleeptimer.h"
#include "arm_math.h"
static paramsMLX90640 mlx90640;

// -----------------------------------------------------------------------------
//                        Local function declarations
// -----------------------------------------------------------------------------
static void ExtractVDDParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractPTATParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractGainParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractTgcParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractResolutionParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractKsTaParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractKsToParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractAlphaParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractOffsetParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractKtaPixelParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractKvPixelParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractCPParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static void ExtractCILCParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
static int ExtractDeviatingPixels(uint16_t *eeData, paramsMLX90640 *mlx90640);
static int CheckAdjacentPixels(uint16_t pix1, uint16_t pix2);
static float GetMedian(float *values, int n);
static int IsPixelBad(uint16_t pixel, paramsMLX90640 *params);
static int ValidateFrameData(uint16_t *frameData);
static int ValidateAuxData(uint16_t *auxData);

static float32_t fast_sqrt(float32_t v)
{
  float32_t out;
  arm_sqrt_f32(v, &out);
  return out;
}
// -----------------------------------------------------------------------------
//                           Function definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Initializes the MLX90640 driver
 ******************************************************************************/
sl_status_t mlx90640_init(sl_i2cspm_t *i2cspm_instance)
{
  int status;
  uint8_t bad_pixel_count = 0;
  uint16_t eeMLX90640[832];

  mlx90640_I2C_Init(i2cspm_instance);

  status = mlx90640_DumpEE(eeMLX90640);
  if (status != 0) {
    app_log("Failed to load system parameters of MLX90640");
  }

  bad_pixel_count = mlx90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (bad_pixel_count != 0) {
    app_log("Number of pixel errors: %d", bad_pixel_count);
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Requests and provides an array of temperatures for all 768 pixels as float values.
 ******************************************************************************/
sl_status_t mlx90640_get_image_array(float *pixel_array)
{
  float Ta;
  for (uint8_t x = 0 ; x < 2; x++) { //Read both sub-pages
    uint16_t mlx90640Frame[834];
    int status = mlx90640_GetFrameData(mlx90640Frame);
    if (status < 0) {
      app_log("GetFrame Error: %d", status);
      return -1;
    }

    mlx90640_GetTa(mlx90640Frame, &mlx90640, &Ta);
    float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
    float emissivity = MLX90640_CONFIG_EMISSIVITY;

    mlx90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, pixel_array);
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Requests and provides the contents of the EEPROM.
 ******************************************************************************/
sl_status_t mlx90640_DumpEE(uint16_t *eeData)
{
  return mlx90640_I2CRead(0x2400, 832, eeData);
}

/***************************************************************************//**
 * Waits for a new data to be available with a given slave address
 ******************************************************************************/
sl_status_t mlx90640_SynchFrame(void)
{
  uint16_t dataReady = 0;
  uint16_t statusRegister;
  int error = SL_STATUS_FAIL;

  error = mlx90640_I2CWrite(0x8000, 0x0030);
  if (error == SL_STATUS_FAIL) {
    return SL_STATUS_FAIL;
  }

  while (dataReady == 0) {
    error = mlx90640_I2CRead(0x8000, 1, &statusRegister);
    if (error != 0) {
      return error;
    }
    dataReady = statusRegister & 0x0008;
  }

  return 0;
}

/***************************************************************************//**
 * Uses the global reset command described in the I2C standard to trigger a measurement.
 * !! Note that this function will reset all devices on the same I2C bus that support this command. !!
 ******************************************************************************/
sl_status_t mlx90640_TriggerMeasurement()
{
  int error = 1;
  uint16_t ctrlReg;

  error = mlx90640_I2CRead(0x800D, 1, &ctrlReg);

  if ( error != 0) {
    return error;
  }

  ctrlReg |= 0x8000;
  error = mlx90640_I2CWrite(0x800D, ctrlReg);

  if ( error != 0) {
    return error;
  }

  error = mlx90640_I2CGeneralReset();

  if ( error != 0) {
    return error;
  }

  error = mlx90640_I2CRead(0x800D, 1, &ctrlReg);

  if ( error != 0) {
    return error;
  }

  if ((ctrlReg & 0x8000) != 0) {
    return -9;
  }

  return 0;
}

/***************************************************************************//**
 * Reads all the necessary frame data from the device.
 ******************************************************************************/
sl_status_t mlx90640_GetFrameData(uint16_t *frameData)
{
  uint16_t dataReady = 0;
  uint16_t controlRegister1;
  uint16_t statusRegister;
  int error = 1;
  uint16_t data[64];
  uint8_t cnt = 0;

  while (dataReady == 0) {
    error = mlx90640_I2CRead(0x8000, 1, &statusRegister);
    if (error != 0) {
      return error;
    }
    dataReady = statusRegister & 0x0008;
  }

  error = mlx90640_I2CWrite(0x8000, 0x0030);
  if (error == -1) {
    return error;
  }

  error = mlx90640_I2CRead(0x0400, 768, frameData);
  if (error != 0) {
    return error;
  }

  error = mlx90640_I2CRead(0x0700, 64, data);
  if (error != 0) {
    return error;
  }

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);
  frameData[832] = controlRegister1;
  frameData[833] = statusRegister & 0x0001;

  if (error != 0) {
    return error;
  }

  error = ValidateAuxData(data);
  if (error == 0) {
    for (cnt = 0; cnt < 64; cnt++) {
      frameData[cnt + 768] = data[cnt];
    }
  }

  error = ValidateFrameData(frameData);
  if (error != 0) {
    return error;
  }

  return frameData[833];
}

/***************************************************************************//**
 * Validates frame data
 ******************************************************************************/
int ValidateFrameData(uint16_t *frameData)
{
  uint8_t line = 0;

  for (int i = 0; i < 768; i += 32) {
    if ((frameData[i] == 0x7FFF) && (line % 2 == frameData[833])) {
      return -8;
    }
    line = line + 1;
  }

  return 0;
}

/***************************************************************************//**
 * Validates auxiliary data
 ******************************************************************************/
int ValidateAuxData(uint16_t *auxData)
{
  if (auxData[0] == 0x7FFF) {
    return -8;
  }

  for (int i = 8; i < 19; i++) {
    if (auxData[i] == 0x7FFF) {
      return -8;
    }
  }

  for (int i = 20; i < 23; i++) {
    if (auxData[i] == 0x7FFF) {
      return -8;
    }
  }

  for (int i = 24; i < 33; i++) {
    if (auxData[i] == 0x7FFF) {
      return -8;
    }
  }

  for (int i = 40; i < 51; i++) {
    if (auxData[i] == 0x7FFF) {
      return -8;
    }
  }

  for (int i = 52; i < 55; i++) {
    if (auxData[i] == 0x7FFF) {
      return -8;
    }
  }

  for (int i = 56; i < 64; i++) {
    if (auxData[i] == 0x7FFF) {
      return -8;
    }
  }

  return 0;
}

/***************************************************************************//**
 * Extracts the parameters from a given EEPROM data array and stores values
 ******************************************************************************/
sl_status_t mlx90640_ExtractParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int error = 0;

  ExtractVDDParameters(eeData, mlx90640);
  ExtractPTATParameters(eeData, mlx90640);
  ExtractGainParameters(eeData, mlx90640);
  ExtractTgcParameters(eeData, mlx90640);
  ExtractResolutionParameters(eeData, mlx90640);
  ExtractKsTaParameters(eeData, mlx90640);
  ExtractKsToParameters(eeData, mlx90640);
  ExtractCPParameters(eeData, mlx90640);
  ExtractAlphaParameters(eeData, mlx90640);
  ExtractOffsetParameters(eeData, mlx90640);
  ExtractKtaPixelParameters(eeData, mlx90640);
  ExtractKvPixelParameters(eeData, mlx90640);
  ExtractCILCParameters(eeData, mlx90640);
  error = ExtractDeviatingPixels(eeData, mlx90640);

  return error;
}

/***************************************************************************//**
 * Writes the desired resolution value in order to change the current resolution
 ******************************************************************************/
sl_status_t mlx90640_SetResolution(uint8_t resolution)
{
  uint16_t controlRegister1;
  int value;
  int error;

  value = (resolution & 0x03) << 10;

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);

  if (error == 0) {
    value = (controlRegister1 & 0xF3FF) | value;
    error = mlx90640_I2CWrite(0x800D, value);
  }

  return error;
}

/***************************************************************************//**
 * Provides the current resolution.
 ******************************************************************************/
sl_status_t mlx90640_GetCurResolution(uint16_t *currentResolution)
{
  uint16_t controlRegister1;
  int resolutionRAM;
  int error;

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);
  if (error != 0) {
    return SL_STATUS_FAIL;
  }
  resolutionRAM = (controlRegister1 & 0x0C00) >> 10;

  *currentResolution = resolutionRAM;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Writes the desired resolution value in order to change the current refresh rate
 ******************************************************************************/
sl_status_t mlx90640_SetRefreshRate(uint8_t refreshRate)
{
  uint16_t controlRegister1;
  int value;
  int error;

  value = (refreshRate & 0x07) << 7;

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);
  if (error == 0) {
    value = (controlRegister1 & 0xFC7F) | value;
    error = mlx90640_I2CWrite(0x800D, value);
  }

  return error;
}

/***************************************************************************//**
 * Provides the current refresh rate.
 ******************************************************************************/
sl_status_t mlx90640_GetRefreshRate(uint16_t *current_rate)
{
  uint16_t controlRegister1;
  int refreshRate;
  int error;

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);
  if (error != 0) {
    return SL_STATUS_FAIL;
  }
  refreshRate = (controlRegister1 & 0x0380) >> 7;

  *current_rate = refreshRate;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Sets device to interleaved mode
 ******************************************************************************/
sl_status_t mlx90640_SetInterleavedMode()
{
  uint16_t controlRegister1;
  int value;
  int error;

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);

  if (error == 0) {
    value = (controlRegister1 & 0xEFFF);
    error = mlx90640_I2CWrite(0x800D, value);
  }

  return error;
}

/***************************************************************************//**
 * Sets device to chess mode
 ******************************************************************************/
sl_status_t mlx90640_SetChessMode()
{
  uint16_t controlRegister1;
  int value;
  int error;

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);

  if (error == 0) {
    value = (controlRegister1 | 0x1000);
    error = mlx90640_I2CWrite(0x800D, value);
  }

  return error;
}

/***************************************************************************//**
 * Provides the current working mode.
 ******************************************************************************/
sl_status_t mlx90640_GetCurMode(uint8_t *cur_mode)
{
  uint16_t controlRegister1;
  int modeRAM;
  int error;

  error = mlx90640_I2CRead(0x800D, 1, &controlRegister1);
  if (error != 0) {
    return error;
  }
  modeRAM = (controlRegister1 & 0x1000) >> 12;

  *cur_mode = modeRAM;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Calculates the object temperatures for all 768 pixel.
 ******************************************************************************/
sl_status_t mlx90640_CalculateTo(uint16_t *frameData, const paramsMLX90640 *params, float emissivity, float tr, float *result)
{
  float vdd;
  float ta;
  float ta4;
  float tr4;
  float taTr;
  float gain;
  float irDataCP[2];
  float irData;
  float alphaCompensated;
  uint8_t mode;
  int8_t ilPattern;
  int8_t chessPattern;
  int8_t pattern;
  int8_t conversionPattern;
  float Sx;
  float To;
  float alphaCorrR[4];
  int8_t range;
  uint16_t subPage;
  float ktaScale;
  float kvScale;
  float alphaScale;
  float kta;
  float kv;

  subPage = frameData[833];
  mlx90640_GetVdd(frameData, params, &vdd);
  mlx90640_GetTa(frameData, params, &ta);

  ta4 = (ta + 273.15);
  ta4 = ta4 * ta4;
  ta4 = ta4 * ta4;
  tr4 = (tr + 273.15);
  tr4 = tr4 * tr4;
  tr4 = tr4 * tr4;
  taTr = tr4 - (tr4 - ta4) / emissivity;

  ktaScale = pow(2, (double)params->ktaScale);
  kvScale = pow(2, (double)params->kvScale);
  alphaScale = pow(2, (double)params->alphaScale);

  alphaCorrR[0] = 1 / (1 + params->ksTo[0] * 40);
  alphaCorrR[1] = 1;
  alphaCorrR[2] = (1 + params->ksTo[1] * params->ct[2]);
  alphaCorrR[3] = alphaCorrR[2] * (1 + params->ksTo[2] * (params->ct[3] - params->ct[2]));

//------------------------- Gain calculation -----------------------------------
  gain = frameData[778];
  if (gain > 32767) {
    gain = gain - 65536;
  }
  gain = params->gainEE / gain;

//------------------------- To calculation -------------------------------------
  mode = (frameData[832] & 0x1000) >> 5;

  irDataCP[0] = frameData[776];
  irDataCP[1] = frameData[808];
  for ( int i = 0; i < 2; i++) {
    if (irDataCP[i] > 32767) {
      irDataCP[i] = irDataCP[i] - 65536;
    }
    irDataCP[i] = irDataCP[i] * gain;
  }
  irDataCP[0] = irDataCP[0] - params->cpOffset[0] * (1 + params->cpKta * (ta - 25)) * (1 + params->cpKv * (vdd - 3.3));
  if ( mode ==  params->calibrationModeEE) {
    irDataCP[1] = irDataCP[1] - params->cpOffset[1] * (1 + params->cpKta * (ta - 25)) * (1 + params->cpKv * (vdd - 3.3));
  } else {
    irDataCP[1] = irDataCP[1] - (params->cpOffset[1] + params->ilChessC[0]) * (1 + params->cpKta * (ta - 25)) * (1 + params->cpKv * (vdd - 3.3));
  }

  for (int pixelNumber = 0; pixelNumber < 768; pixelNumber++) {
    ilPattern = pixelNumber / 32 - (pixelNumber / 64) * 2;
    chessPattern = ilPattern ^ (pixelNumber - (pixelNumber / 2) * 2);
    conversionPattern = ((pixelNumber + 2) / 4 - (pixelNumber + 3) / 4 + (pixelNumber + 1) / 4 - pixelNumber / 4) * (1 - 2 * ilPattern);

    if (mode == 0) {
      pattern = ilPattern;
    } else {
      pattern = chessPattern;
    }

    if (pattern == frameData[833]) {
      irData = frameData[pixelNumber];
      if (irData > 32767) {
        irData = irData - 65536;
      }
      irData = irData * gain;

      kta = params->kta[pixelNumber] / ktaScale;
      kv = params->kv[pixelNumber] / kvScale;
      irData = irData - params->offset[pixelNumber] * (1 + kta * (ta - 25)) * (1 + kv * (vdd - 3.3));

      if (mode !=  params->calibrationModeEE) {
        irData = irData + params->ilChessC[2] * (2 * ilPattern - 1) - params->ilChessC[1] * conversionPattern;
      }

      irData = irData - params->tgc * irDataCP[subPage];
      irData = irData / emissivity;

      alphaCompensated = SCALEALPHA * alphaScale / params->alpha[pixelNumber];
      alphaCompensated = alphaCompensated * (1 + params->KsTa * (ta - 25));

      Sx = alphaCompensated * alphaCompensated * alphaCompensated * (irData + alphaCompensated * taTr);
      Sx = fast_sqrt(fast_sqrt(Sx)) * params->ksTo[1];

      To = fast_sqrt(fast_sqrt(irData / (alphaCompensated * (1 - params->ksTo[1] * 273.15) + Sx) + taTr)) - 273.15;

      if (To < params->ct[1]) {
        range = 0;
      } else if (To < params->ct[2]) {
        range = 1;
      } else if (To < params->ct[3]) {
        range = 2;
      } else {
        range = 3;
      }

      To = fast_sqrt(fast_sqrt(irData / (alphaCompensated * alphaCorrR[range] * (1 + params->ksTo[range] * (To - params->ct[range]))) + taTr)) - 273.15;

      result[pixelNumber] = To;
    }
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Calculates values for all 768 pixels - not absolute temperature!
 ******************************************************************************/
sl_status_t mlx90640_GetImage(uint16_t *frameData, const paramsMLX90640 *params, float *result)
{
  float vdd;
  float ta;
  float gain;
  float irDataCP[2];
  float irData;
  float alphaCompensated;
  uint8_t mode;
  int8_t ilPattern;
  int8_t chessPattern;
  int8_t pattern;
  int8_t conversionPattern;
  float image;
  uint16_t subPage;
  float ktaScale;
  float kvScale;
  float kta;
  float kv;

  subPage = frameData[833];
  mlx90640_GetVdd(frameData, params, &vdd);
  mlx90640_GetTa(frameData, params, &ta);

  ktaScale = pow(2, (double)params->ktaScale);
  kvScale = pow(2, (double)params->kvScale);

//------------------------- Gain calculation -----------------------------------
  gain = frameData[778];
  if (gain > 32767) {
    gain = gain - 65536;
  }

  gain = params->gainEE / gain;

//------------------------- Image calculation -------------------------------------
  mode = (frameData[832] & 0x1000) >> 5;

  irDataCP[0] = frameData[776];
  irDataCP[1] = frameData[808];
  for ( int i = 0; i < 2; i++) {
    if (irDataCP[i] > 32767) {
      irDataCP[i] = irDataCP[i] - 65536;
    }
    irDataCP[i] = irDataCP[i] * gain;
  }
  irDataCP[0] = irDataCP[0] - params->cpOffset[0] * (1 + params->cpKta * (ta - 25)) * (1 + params->cpKv * (vdd - 3.3));
  if ( mode ==  params->calibrationModeEE) {
    irDataCP[1] = irDataCP[1] - params->cpOffset[1] * (1 + params->cpKta * (ta - 25)) * (1 + params->cpKv * (vdd - 3.3));
  } else {
    irDataCP[1] = irDataCP[1] - (params->cpOffset[1] + params->ilChessC[0]) * (1 + params->cpKta * (ta - 25)) * (1 + params->cpKv * (vdd - 3.3));
  }

  for ( int pixelNumber = 0; pixelNumber < 768; pixelNumber++) {
    ilPattern = pixelNumber / 32 - (pixelNumber / 64) * 2;
    chessPattern = ilPattern ^ (pixelNumber - (pixelNumber / 2) * 2);
    conversionPattern = ((pixelNumber + 2) / 4 - (pixelNumber + 3) / 4 + (pixelNumber + 1) / 4 - pixelNumber / 4) * (1 - 2 * ilPattern);

    if (mode == 0) {
      pattern = ilPattern;
    } else {
      pattern = chessPattern;
    }

    if (pattern == frameData[833]) {
      irData = frameData[pixelNumber];
      if (irData > 32767) {
        irData = irData - 65536;
      }
      irData = irData * gain;

      kta = params->kta[pixelNumber] / ktaScale;
      kv = params->kv[pixelNumber] / kvScale;
      irData = irData - params->offset[pixelNumber] * (1 + kta * (ta - 25)) * (1 + kv * (vdd - 3.3));

      if (mode !=  params->calibrationModeEE) {
        irData = irData + params->ilChessC[2] * (2 * ilPattern - 1) - params->ilChessC[1] * conversionPattern;
      }

      irData = irData - params->tgc * irDataCP[subPage];

      alphaCompensated = params->alpha[pixelNumber];

      image = irData * alphaCompensated;

      result[pixelNumber] = image;
    }
  }
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Provides the current VDD of the device
 ******************************************************************************/
sl_status_t mlx90640_GetVdd(uint16_t *frameData, const paramsMLX90640 *params, float *vdd)
{
  float temp_vdd;
  float resolutionCorrection;

  int resolutionRAM;

  temp_vdd = frameData[810];
  if (temp_vdd > 32767) {
    temp_vdd = temp_vdd - 65536;
  }
  resolutionRAM = (frameData[832] & 0x0C00) >> 10;
  resolutionCorrection = pow(2, (double)params->resolutionEE) / pow(2, (double)resolutionRAM);
  temp_vdd = (resolutionCorrection * temp_vdd - params->vdd25) / params->kVdd + 3.3;

  *vdd = temp_vdd;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Provides the current Ta of the device
 ******************************************************************************/
sl_status_t mlx90640_GetTa(uint16_t *frameData, const paramsMLX90640 *params, float *ta)
{
  float ptat;
  float ptatArt;
  float vdd;
  float temp_ta;

  mlx90640_GetVdd(frameData, params, &vdd);

  ptat = frameData[800];
  if (ptat > 32767) {
    ptat = ptat - 65536;
  }

  ptatArt = frameData[768];
  if (ptatArt > 32767) {
    ptatArt = ptatArt - 65536;
  }
  ptatArt = (ptat / (ptat * params->alphaPTAT + ptatArt)) * pow(2, (double)18);

  temp_ta = (ptatArt / (1 + params->KvPTAT * (vdd - 3.3)) - params->vPTAT25);
  temp_ta = temp_ta / params->KtPTAT + 25;

  *ta = temp_ta;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Provides the sub-page for a selected frame data
 ******************************************************************************/
sl_status_t mlx90640_GetSubPageNumber(uint16_t *frameData)
{
  return frameData[833];
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Corrects the values of the broken pixels and/or the outlier pixels.
 ******************************************************************************/
sl_status_t mlx90640_BadPixelsCorrection(uint16_t *pixels, float *to, int mode, paramsMLX90640 *params)
{
  float ap[4];
  uint8_t pix;
  uint8_t line;
  uint8_t column;

  pix = 0;
  while (pixels[pix] != 0xFFFF) {
    line = pixels[pix] >> 5;
    column = pixels[pix] - (line << 5);

    if (mode == 1) {
      if (line == 0) {
        if (column == 0) {
          to[pixels[pix]] = to[33];
        } else if (column == 31) {
          to[pixels[pix]] = to[62];
        } else {
          to[pixels[pix]] = (to[pixels[pix] + 31] + to[pixels[pix] + 33]) / 2.0;
        }
      } else if (line == 23) {
        if (column == 0) {
          to[pixels[pix]] = to[705];
        } else if (column == 31) {
          to[pixels[pix]] = to[734];
        } else {
          to[pixels[pix]] = (to[pixels[pix] - 33] + to[pixels[pix] - 31]) / 2.0;
        }
      } else if (column == 0) {
        to[pixels[pix]] = (to[pixels[pix] - 31] + to[pixels[pix] + 33]) / 2.0;
      } else if (column == 31) {
        to[pixels[pix]] = (to[pixels[pix] - 33] + to[pixels[pix] + 31]) / 2.0;
      } else {
        ap[0] = to[pixels[pix] - 33];
        ap[1] = to[pixels[pix] - 31];
        ap[2] = to[pixels[pix] + 31];
        ap[3] = to[pixels[pix] + 33];
        to[pixels[pix]] = GetMedian(ap, 4);
      }
    } else {
      if (column == 0) {
        to[pixels[pix]] = to[pixels[pix] + 1];
      } else if (column == 1 || column == 30) {
        to[pixels[pix]] = (to[pixels[pix] - 1] + to[pixels[pix] + 1]) / 2.0;
      } else if (column == 31) {
        to[pixels[pix]] = to[pixels[pix] - 1];
      } else {
        if (IsPixelBad(pixels[pix] - 2, params) == 0 && IsPixelBad(pixels[pix] + 2, params) == 0) {
          ap[0] = to[pixels[pix] + 1] - to[pixels[pix] + 2];
          ap[1] = to[pixels[pix] - 1] - to[pixels[pix] - 2];
          if (fabs(ap[0]) > fabs(ap[1])) {
            to[pixels[pix]] = to[pixels[pix] - 1] + ap[1];
          } else {
            to[pixels[pix]] = to[pixels[pix] + 1] + ap[0];
          }
        } else {
          to[pixels[pix]] = (to[pixels[pix] - 1] + to[pixels[pix] + 1]) / 2.0;
        }
      }
    }
    pix = pix + 1;
  }
  return SL_STATUS_OK;
}

//------------------------------------------------------------------------------

void ExtractVDDParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int16_t kVdd;
  int16_t vdd25;

  kVdd = eeData[51];

  kVdd = (eeData[51] & 0xFF00) >> 8;
  if (kVdd > 127) {
    kVdd = kVdd - 256;
  }
  kVdd = 32 * kVdd;
  vdd25 = eeData[51] & 0x00FF;
  vdd25 = ((vdd25 - 256) << 5) - 8192;

  mlx90640->kVdd = kVdd;
  mlx90640->vdd25 = vdd25;
}

//------------------------------------------------------------------------------

void ExtractPTATParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  float KvPTAT;
  float KtPTAT;
  int16_t vPTAT25;
  float alphaPTAT;

  KvPTAT = (eeData[50] & 0xFC00) >> 10;
  if (KvPTAT > 31) {
    KvPTAT = KvPTAT - 64;
  }
  KvPTAT = KvPTAT / 4096;

  KtPTAT = eeData[50] & 0x03FF;
  if (KtPTAT > 511) {
    KtPTAT = KtPTAT - 1024;
  }
  KtPTAT = KtPTAT / 8;

  vPTAT25 = eeData[49];

  alphaPTAT = (eeData[16] & 0xF000) / pow(2, (double)14) + 8.0f;

  mlx90640->KvPTAT = KvPTAT;
  mlx90640->KtPTAT = KtPTAT;
  mlx90640->vPTAT25 = vPTAT25;
  mlx90640->alphaPTAT = alphaPTAT;
}

//------------------------------------------------------------------------------

void ExtractGainParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int16_t gainEE;

  gainEE = eeData[48];
  if (gainEE > 32767) {
    gainEE = gainEE - 65536;
  }

  mlx90640->gainEE = gainEE;
}

//------------------------------------------------------------------------------

void ExtractTgcParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  float tgc;
  tgc = eeData[60] & 0x00FF;
  if (tgc > 127) {
    tgc = tgc - 256;
  }
  tgc = tgc / 32.0f;

  mlx90640->tgc = tgc;
}

//------------------------------------------------------------------------------

void ExtractResolutionParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  uint8_t resolutionEE;
  resolutionEE = (eeData[56] & 0x3000) >> 12;

  mlx90640->resolutionEE = resolutionEE;
}

//------------------------------------------------------------------------------

void ExtractKsTaParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  float KsTa;
  KsTa = (eeData[60] & 0xFF00) >> 8;
  if (KsTa > 127) {
    KsTa = KsTa - 256;
  }
  KsTa = KsTa / 8192.0f;

  mlx90640->KsTa = KsTa;
}

//------------------------------------------------------------------------------

void ExtractKsToParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int32_t KsToScale;
  int8_t step;

  step = ((eeData[63] & 0x3000) >> 12) * 10;

  mlx90640->ct[0] = -40;
  mlx90640->ct[1] = 0;
  mlx90640->ct[2] = (eeData[63] & 0x00F0) >> 4;
  mlx90640->ct[3] = (eeData[63] & 0x0F00) >> 8;

  mlx90640->ct[2] = mlx90640->ct[2] * step;
  mlx90640->ct[3] = mlx90640->ct[2] + mlx90640->ct[3] * step;
  mlx90640->ct[4] = 400;

  KsToScale = (eeData[63] & 0x000F) + 8;
  KsToScale = 1UL << KsToScale;

  mlx90640->ksTo[0] = eeData[61] & 0x00FF;
  mlx90640->ksTo[1] = (eeData[61] & 0xFF00) >> 8;
  mlx90640->ksTo[2] = eeData[62] & 0x00FF;
  mlx90640->ksTo[3] = (eeData[62] & 0xFF00) >> 8;

  for (int i = 0; i < 4; i++) {
    if (mlx90640->ksTo[i] > 127) {
      mlx90640->ksTo[i] = mlx90640->ksTo[i] - 256;
    }
    mlx90640->ksTo[i] = mlx90640->ksTo[i] / KsToScale;
  }

  mlx90640->ksTo[4] = -0.0002;
}

//------------------------------------------------------------------------------

void ExtractAlphaParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int accRow[24];
  int accColumn[32];
  int p = 0;
  int alphaRef;
  uint8_t alphaScale;
  uint8_t accRowScale;
  uint8_t accColumnScale;
  uint8_t accRemScale;
  float alphaTemp[768];
  float temp;

  accRemScale = eeData[32] & 0x000F;
  accColumnScale = (eeData[32] & 0x00F0) >> 4;
  accRowScale = (eeData[32] & 0x0F00) >> 8;
  alphaScale = ((eeData[32] & 0xF000) >> 12) + 30;
  alphaRef = eeData[33];

  for (int i = 0; i < 6; i++) {
    p = i * 4;
    accRow[p + 0] = (eeData[34 + i] & 0x000F);
    accRow[p + 1] = (eeData[34 + i] & 0x00F0) >> 4;
    accRow[p + 2] = (eeData[34 + i] & 0x0F00) >> 8;
    accRow[p + 3] = (eeData[34 + i] & 0xF000) >> 12;
  }

  for (int i = 0; i < 24; i++) {
    if (accRow[i] > 7) {
      accRow[i] = accRow[i] - 16;
    }
  }

  for (int i = 0; i < 8; i++) {
    p = i * 4;
    accColumn[p + 0] = (eeData[40 + i] & 0x000F);
    accColumn[p + 1] = (eeData[40 + i] & 0x00F0) >> 4;
    accColumn[p + 2] = (eeData[40 + i] & 0x0F00) >> 8;
    accColumn[p + 3] = (eeData[40 + i] & 0xF000) >> 12;
  }

  for (int i = 0; i < 32; i++) {
    if (accColumn[i] > 7) {
      accColumn[i] = accColumn[i] - 16;
    }
  }

  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 32; j++) {
      p = 32 * i + j;
      alphaTemp[p] = (eeData[64 + p] & 0x03F0) >> 4;
      if (alphaTemp[p] > 31) {
        alphaTemp[p] = alphaTemp[p] - 64;
      }
      alphaTemp[p] = alphaTemp[p] * (1 << accRemScale);
      alphaTemp[p] = (alphaRef + (accRow[i] << accRowScale) + (accColumn[j] << accColumnScale) + alphaTemp[p]);
      alphaTemp[p] = alphaTemp[p] / pow(2, (double)alphaScale);
      alphaTemp[p] = alphaTemp[p] - mlx90640->tgc * (mlx90640->cpAlpha[0] + mlx90640->cpAlpha[1]) / 2;
      alphaTemp[p] = SCALEALPHA / alphaTemp[p];
    }
  }

  temp = alphaTemp[0];
  for (int i = 1; i < 768; i++) {
    if (alphaTemp[i] > temp) {
      temp = alphaTemp[i];
    }
  }

  alphaScale = 0;
  while (temp < 32767.4) {
    temp = temp * 2;
    alphaScale = alphaScale + 1;
  }

  for (int i = 0; i < 768; i++) {
    temp = alphaTemp[i] * pow(2, (double)alphaScale);
    mlx90640->alpha[i] = (temp + 0.5);
  }
  mlx90640->alphaScale = alphaScale;
}

/***************************************************************************//**
 *
 ******************************************************************************/
void ExtractOffsetParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int occRow[24];
  int occColumn[32];
  int p = 0;
  int16_t offsetRef;
  uint8_t occRowScale;
  uint8_t occColumnScale;
  uint8_t occRemScale;

  occRemScale = (eeData[16] & 0x000F);
  occColumnScale = (eeData[16] & 0x00F0) >> 4;
  occRowScale = (eeData[16] & 0x0F00) >> 8;
  offsetRef = eeData[17];
  if (offsetRef > 32767) {
    offsetRef = offsetRef - 65536;
  }

  for (int i = 0; i < 6; i++) {
    p = i * 4;
    occRow[p + 0] = (eeData[18 + i] & 0x000F);
    occRow[p + 1] = (eeData[18 + i] & 0x00F0) >> 4;
    occRow[p + 2] = (eeData[18 + i] & 0x0F00) >> 8;
    occRow[p + 3] = (eeData[18 + i] & 0xF000) >> 12;
  }

  for (int i = 0; i < 24; i++) {
    if (occRow[i] > 7) {
      occRow[i] = occRow[i] - 16;
    }
  }

  for (int i = 0; i < 8; i++) {
    p = i * 4;
    occColumn[p + 0] = (eeData[24 + i] & 0x000F);
    occColumn[p + 1] = (eeData[24 + i] & 0x00F0) >> 4;
    occColumn[p + 2] = (eeData[24 + i] & 0x0F00) >> 8;
    occColumn[p + 3] = (eeData[24 + i] & 0xF000) >> 12;
  }

  for (int i = 0; i < 32; i++) {
    if (occColumn[i] > 7) {
      occColumn[i] = occColumn[i] - 16;
    }
  }

  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 32; j++) {
      p = 32 * i + j;
      mlx90640->offset[p] = (eeData[64 + p] & 0xFC00) >> 10;
      if (mlx90640->offset[p] > 31) {
        mlx90640->offset[p] = mlx90640->offset[p] - 64;
      }
      mlx90640->offset[p] = mlx90640->offset[p] * (1 << occRemScale);
      mlx90640->offset[p] = (offsetRef + (occRow[i] << occRowScale) + (occColumn[j] << occColumnScale) + mlx90640->offset[p]);
    }
  }
}

/***************************************************************************//**
 *
 ******************************************************************************/
void ExtractKtaPixelParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int p = 0;
  int8_t KtaRC[4];
  int8_t KtaRoCo;
  int8_t KtaRoCe;
  int8_t KtaReCo;
  int8_t KtaReCe;
  uint8_t ktaScale1;
  uint8_t ktaScale2;
  uint8_t split;
  float ktaTemp[768];
  float temp;

  KtaRoCo = (eeData[54] & 0xFF00) >> 8;
  if (KtaRoCo > 127) {
    KtaRoCo = KtaRoCo - 256;
  }
  KtaRC[0] = KtaRoCo;

  KtaReCo = (eeData[54] & 0x00FF);
  if (KtaReCo > 127) {
    KtaReCo = KtaReCo - 256;
  }
  KtaRC[2] = KtaReCo;

  KtaRoCe = (eeData[55] & 0xFF00) >> 8;
  if (KtaRoCe > 127) {
    KtaRoCe = KtaRoCe - 256;
  }
  KtaRC[1] = KtaRoCe;

  KtaReCe = (eeData[55] & 0x00FF);
  if (KtaReCe > 127) {
    KtaReCe = KtaReCe - 256;
  }
  KtaRC[3] = KtaReCe;

  ktaScale1 = ((eeData[56] & 0x00F0) >> 4) + 8;
  ktaScale2 = (eeData[56] & 0x000F);

  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 32; j++) {
      p = 32 * i + j;
      split = 2 * (p / 32 - (p / 64) * 2) + p % 2;
      ktaTemp[p] = (eeData[64 + p] & 0x000E) >> 1;
      if (ktaTemp[p] > 3) {
        ktaTemp[p] = ktaTemp[p] - 8;
      }
      ktaTemp[p] = ktaTemp[p] * (1 << ktaScale2);
      ktaTemp[p] = KtaRC[split] + ktaTemp[p];
      ktaTemp[p] = ktaTemp[p] / pow(2, (double)ktaScale1);
    }
  }

  temp = fabs(ktaTemp[0]);
  for (int i = 1; i < 768; i++) {
    if (fabs(ktaTemp[i]) > temp) {
      temp = fabs(ktaTemp[i]);
    }
  }

  ktaScale1 = 0;
  while (temp < 63.4) {
    temp = temp * 2;
    ktaScale1 = ktaScale1 + 1;
  }

  for (int i = 0; i < 768; i++) {
    temp = ktaTemp[i] * pow(2, (double)ktaScale1);
    if (temp < 0) {
      mlx90640->kta[i] = (temp - 0.5);
    } else {
      mlx90640->kta[i] = (temp + 0.5);
    }
  }
  mlx90640->ktaScale = ktaScale1;
}

/***************************************************************************//**
 *
 ******************************************************************************/
void ExtractKvPixelParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  int p = 0;
  int8_t KvT[4];
  int8_t KvRoCo;
  int8_t KvRoCe;
  int8_t KvReCo;
  int8_t KvReCe;
  uint8_t kvScale;
  uint8_t split;
  float kvTemp[768];
  float temp;

  KvRoCo = (eeData[52] & 0xF000) >> 12;
  if (KvRoCo > 7) {
    KvRoCo = KvRoCo - 16;
  }
  KvT[0] = KvRoCo;

  KvReCo = (eeData[52] & 0x0F00) >> 8;
  if (KvReCo > 7) {
    KvReCo = KvReCo - 16;
  }
  KvT[2] = KvReCo;

  KvRoCe = (eeData[52] & 0x00F0) >> 4;
  if (KvRoCe > 7) {
    KvRoCe = KvRoCe - 16;
  }
  KvT[1] = KvRoCe;

  KvReCe = (eeData[52] & 0x000F);
  if (KvReCe > 7) {
    KvReCe = KvReCe - 16;
  }
  KvT[3] = KvReCe;

  kvScale = (eeData[56] & 0x0F00) >> 8;

  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 32; j++) {
      p = 32 * i + j;
      split = 2 * (p / 32 - (p / 64) * 2) + p % 2;
      kvTemp[p] = KvT[split];
      kvTemp[p] = kvTemp[p] / pow(2, (double)kvScale);
    }
  }

  temp = fabs(kvTemp[0]);
  for (int i = 1; i < 768; i++) {
    if (fabs(kvTemp[i]) > temp) {
      temp = fabs(kvTemp[i]);
    }
  }

  kvScale = 0;
  while (temp < 63.4) {
    temp = temp * 2;
    kvScale = kvScale + 1;
  }

  for (int i = 0; i < 768; i++) {
    temp = kvTemp[i] * pow(2, (double)kvScale);
    if (temp < 0) {
      mlx90640->kv[i] = (temp - 0.5);
    } else {
      mlx90640->kv[i] = (temp + 0.5);
    }
  }
  mlx90640->kvScale = kvScale;
}

/***************************************************************************//**
 *
 ******************************************************************************/
void ExtractCPParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  float alphaSP[2];
  int16_t offsetSP[2];
  float cpKv;
  float cpKta;
  uint8_t alphaScale;
  uint8_t ktaScale1;
  uint8_t kvScale;

  alphaScale = ((eeData[32] & 0xF000) >> 12) + 27;

  offsetSP[0] = (eeData[58] & 0x03FF);
  if (offsetSP[0] > 511) {
    offsetSP[0] = offsetSP[0] - 1024;
  }

  offsetSP[1] = (eeData[58] & 0xFC00) >> 10;
  if (offsetSP[1] > 31) {
    offsetSP[1] = offsetSP[1] - 64;
  }
  offsetSP[1] = offsetSP[1] + offsetSP[0];

  alphaSP[0] = (eeData[57] & 0x03FF);
  if (alphaSP[0] > 511) {
    alphaSP[0] = alphaSP[0] - 1024;
  }
  alphaSP[0] = alphaSP[0] /  pow(2, (double)alphaScale);

  alphaSP[1] = (eeData[57] & 0xFC00) >> 10;
  if (alphaSP[1] > 31) {
    alphaSP[1] = alphaSP[1] - 64;
  }
  alphaSP[1] = (1 + alphaSP[1] / 128) * alphaSP[0];

  cpKta = (eeData[59] & 0x00FF);
  if (cpKta > 127) {
    cpKta = cpKta - 256;
  }
  ktaScale1 = ((eeData[56] & 0x00F0) >> 4) + 8;
  mlx90640->cpKta = cpKta / pow(2, (double)ktaScale1);

  cpKv = (eeData[59] & 0xFF00) >> 8;
  if (cpKv > 127) {
    cpKv = cpKv - 256;
  }
  kvScale = (eeData[56] & 0x0F00) >> 8;
  mlx90640->cpKv = cpKv / pow(2, (double)kvScale);

  mlx90640->cpAlpha[0] = alphaSP[0];
  mlx90640->cpAlpha[1] = alphaSP[1];
  mlx90640->cpOffset[0] = offsetSP[0];
  mlx90640->cpOffset[1] = offsetSP[1];
}

/***************************************************************************//**
 *
 ******************************************************************************/
void ExtractCILCParameters(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  float ilChessC[3];
  uint8_t calibrationModeEE;

  calibrationModeEE = (eeData[10] & 0x0800) >> 4;
  calibrationModeEE = calibrationModeEE ^ 0x80;

  ilChessC[0] = (eeData[53] & 0x003F);
  if (ilChessC[0] > 31) {
    ilChessC[0] = ilChessC[0] - 64;
  }
  ilChessC[0] = ilChessC[0] / 16.0f;

  ilChessC[1] = (eeData[53] & 0x07C0) >> 6;
  if (ilChessC[1] > 15) {
    ilChessC[1] = ilChessC[1] - 32;
  }
  ilChessC[1] = ilChessC[1] / 2.0f;

  ilChessC[2] = (eeData[53] & 0xF800) >> 11;
  if (ilChessC[2] > 15) {
    ilChessC[2] = ilChessC[2] - 32;
  }
  ilChessC[2] = ilChessC[2] / 8.0f;

  mlx90640->calibrationModeEE = calibrationModeEE;
  mlx90640->ilChessC[0] = ilChessC[0];
  mlx90640->ilChessC[1] = ilChessC[1];
  mlx90640->ilChessC[2] = ilChessC[2];
}

/***************************************************************************//**
 *
 ******************************************************************************/
int ExtractDeviatingPixels(uint16_t *eeData, paramsMLX90640 *mlx90640)
{
  uint16_t pixCnt = 0;
  uint16_t brokenPixCnt = 0;
  uint16_t outlierPixCnt = 0;
  int warn = 0;
  int i;

  for (pixCnt = 0; pixCnt < 5; pixCnt++) {
    mlx90640->brokenPixels[pixCnt] = 0xFFFF;
    mlx90640->outlierPixels[pixCnt] = 0xFFFF;
  }

  pixCnt = 0;
  while (pixCnt < 768 && brokenPixCnt < 5 && outlierPixCnt < 5) {
    if (eeData[pixCnt + 64] == 0) {
      mlx90640->brokenPixels[brokenPixCnt] = pixCnt;
      brokenPixCnt = brokenPixCnt + 1;
    } else if ((eeData[pixCnt + 64] & 0x0001) != 0) {
      mlx90640->outlierPixels[outlierPixCnt] = pixCnt;
      outlierPixCnt = outlierPixCnt + 1;
    }
    pixCnt = pixCnt + 1;
  }

  if (brokenPixCnt > 4) {
    warn = -3;
  } else if (outlierPixCnt > 4) {
    warn = -4;
  } else if ((brokenPixCnt + outlierPixCnt) > 4) {
    warn = -5;
  } else {
    for (pixCnt = 0; pixCnt < brokenPixCnt; pixCnt++) {
      for (i = pixCnt + 1; i < brokenPixCnt; i++) {
        warn = CheckAdjacentPixels(mlx90640->brokenPixels[pixCnt], mlx90640->brokenPixels[i]);
        if (warn != 0) {
          return warn;
        }
      }
    }

    for (pixCnt = 0; pixCnt < outlierPixCnt; pixCnt++) {
      for (i = pixCnt + 1; i < outlierPixCnt; i++) {
        warn = CheckAdjacentPixels(mlx90640->outlierPixels[pixCnt], mlx90640->outlierPixels[i]);
        if (warn != 0) {
          return warn;
        }
      }
    }

    for (pixCnt = 0; pixCnt < brokenPixCnt; pixCnt++) {
      for (i = 0; i < outlierPixCnt; i++) {
        warn = CheckAdjacentPixels(mlx90640->brokenPixels[pixCnt], mlx90640->outlierPixels[i]);
        if (warn != 0) {
          return warn;
        }
      }
    }
  }
  return warn;
}

/***************************************************************************//**
 *
 ******************************************************************************/
int CheckAdjacentPixels(uint16_t pix1, uint16_t pix2)
{
  int pixPosDif;

  pixPosDif = pix1 - pix2;
  if (pixPosDif > -34 && pixPosDif < -30) {
    return -6;
  }
  if (pixPosDif > -2 && pixPosDif < 2) {
    return -6;
  }
  if (pixPosDif > 30 && pixPosDif < 34) {
    return -6;
  }

  return 0;
}

/***************************************************************************//**
 *
 ******************************************************************************/
float GetMedian(float *values, int n)
{
  float temp;

  for (int i = 0; i < n - 1; i++) {
    for (int j = i + 1; j < n; j++) {
      if (values[j] < values[i]) {
        temp = values[i];
        values[i] = values[j];
        values[j] = temp;
      }
    }
  }

  if (n % 2 == 0) {
    return ((values[n / 2] + values[n / 2 - 1]) / 2.0);
  } else {
    return values[n / 2];
  }
}

/***************************************************************************//**
 *
 ******************************************************************************/
int IsPixelBad(uint16_t pixel, paramsMLX90640 *params)
{
  for (int i = 0; i < 5; i++) {
    if (pixel == params->outlierPixels[i] || pixel == params->brokenPixels[i]) {
      return 1;
    }
  }

  return 0;
}
