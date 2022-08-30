/***************************************************************************//**
 * @file app.c
 * @brief Top level application functions
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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "em_cmu.h"
#include "em_common.h"
#include "sl_mic.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "sl_sleeptimer.h"

#include "app.h"
#include "sensory.h"
/***************************************************************************//**
 * Model configuration.
 *
 * You can update the model name in the include paths below to select a model:
 * - hello_gecko: Detects "Hello Gecko", "Bye Bye Gecko", "Gecko Green", and "Gecko Red".
 * - go_silabs: Detects "Go Silabs".
 * - voice_genie: Detects "Voice Genie".
 * - voice_genie_small: A smaller model for detecting "Voice Genie"
 ******************************************************************************/
#include "model/hello_gecko_small/net.h"
#include "model/hello_gecko_small/search.h"

/***************************************************************************//**
 * Definitions and local variables.
 ******************************************************************************/
#define DISPLAY_INFERENCE_TIME
#define RED_LED       sl_led_led0
#define GREEN_LED     sl_led_led1

static int16_t mic_buffer[2 * BRICK_SIZE_SAMPLES];
static sl_sleeptimer_timer_handle_t timer;

static void on_mic_data(const void *buffer, uint32_t frames);
static void process_brick_callback(recognition_context_t* context);
static void print_banner(uint32_t sample_rate_hz);

#if defined(DISPLAY_INFERENCE_TIME)
static uint32_t avg_processing_ticks = 0;
static uint32_t processing_step = 1;
#endif // defined(DISPLAY_INFERENCE_TIME)

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  int delay;
  int param_a_offset;
  const unsigned short *net;
  const unsigned short *grammar;
  uint8_t channels = 1;
  uint32_t sample_rate_hz = (1000 * BRICK_SIZE_SAMPLES) / BRICK_SIZE_MS;

  print_banner(sample_rate_hz);

  // Initialize Sensory library.
  delay        = 0;
  param_a_offset = 0;
  net          = dnn_wakeword_netLabel;
  grammar      = gs_wakeword_grammarLabel;
  errors_t result = SensoryInitialize((u32)net, (u32)grammar, param_a_offset, delay);
  if (result == ERR_OK) {
    // Start listening on microphone.
    sl_sleeptimer_init();
    sl_mic_init(sample_rate_hz, channels);
    sl_mic_start_streaming(mic_buffer, BRICK_SIZE_SAMPLES, on_mic_data);
  } else {
    printf("SensoryInitialize returned error 0x%x\n", result);
  }
}

/***************************************************************************//**
 * Startup banner
 ******************************************************************************/
void print_banner(uint32_t sample_rate_hz)
{
#if defined(MODEL_NAME)
  const char* model_name = MODEL_NAME;
#else
  const char* model_name = "Unknown";
#endif // defined(MODEL_NAME)

  printf("\n\n");
  printf("#==============================================#\n");
  printf("#   Sensory TrulyHandsfree Wake Word Example   #\n");
  printf("#==============================================#\n");
  printf("#   Core Frequency: %21ld [Hz] #\n", CMU_ClockFreqGet(cmuClock_CORE));
  printf("#   Sample rate:    %21lu [Hz] #\n", sample_rate_hz);
  printf("#   Model Name: %30s #\n", model_name);
  printf("#==============================================#\n");
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  while (1) {
    // Program logic is triggered by the `on_mic_data` callback.
  }
}

/***************************************************************************//**
 * This function is called by mic_driver each time BRICK_SIZE_SAMPLES from the
 * microphone are ready.
 * Call Sensory recognition processing function to look for wake up word.
 ******************************************************************************/
static void on_mic_data(const void *buffer, uint32_t frames)
{
  (void)frames;

  uint32_t tick_start = sl_sleeptimer_get_tick_count();
  // Feed mic samples into Sensory library.
  SensoryProcessBrick((void*)buffer, process_brick_callback);

#if defined(DISPLAY_INFERENCE_TIME)
  uint32_t tick_end = sl_sleeptimer_get_tick_count();
  uint32_t ticks = tick_end - tick_start;

  avg_processing_ticks =
    (ticks + (processing_step - 1) * avg_processing_ticks) / processing_step;
  processing_step++;
  uint32_t fifteen_milliseconds_ticks = sl_sleeptimer_ms_to_tick(15);
  uint32_t percentage_time_spent_on_inference = (100 * avg_processing_ticks) / fifteen_milliseconds_ticks;
  printf("Average Inference Time [%% of 15ms]: %lu  \r", percentage_time_spent_on_inference);
#else
  (void)tick_start;
#endif // defined(DISPLAY_INFERENCE_TIME)
}

/***************************************************************************//**
 * Timer callback which turns off LEDs.
 ******************************************************************************/
static void turn_off_leds(sl_sleeptimer_timer_handle_t *handle, void* data)
{
  // Avoid warnings from unused args.
  (void)handle;
  (void)data;

  sl_led_turn_off(&GREEN_LED);
  sl_led_turn_off(&RED_LED);
}

/***************************************************************************//**
 * Restarts the timer that turns off LEDs.
 ******************************************************************************/
static void schedule_timer_to_turn_off_leds(uint32_t timeout_ms)
{
  uint32_t timeout_ticks = sl_sleeptimer_ms_to_tick(timeout_ms);
  sl_sleeptimer_restart_timer(&timer, timeout_ticks, turn_off_leds, NULL, 0, 0);
}

/***************************************************************************//**
 * Get the string label associated with the given model output index.
 ******************************************************************************/
static const char* wakeword_label(uint16_t word_id)
{
  if (word_id < (uint16_t)WAKEWORD_PHRASE_COUNT) {
    return wakeword_phrases[word_id];
  }
  return "Unknown";
}

/***************************************************************************//**
 * Check whether the given wakeword should enable the red LED.
 ******************************************************************************/
static bool wakeword_should_enable_red_led(uint16_t word_id)
{
  return word_id == 1 || word_id == 3;
}

/***************************************************************************//**
 * Check whether the given wakeword should enable the green LED.
 ******************************************************************************/
static bool wakeword_should_enable_green_led(uint16_t word_id)
{
  return word_id == 1 || word_id == 4;
}

/***************************************************************************//**
 * Check whether the LED lights should time out automatically.
 * This is only necessary if the model does not have an 'off' command,
 * i.e. it's trained to detect only one phrase.
 ******************************************************************************/
static bool should_have_timeout_for_leds()
{
  // There are two wakeword phrases that are always present: "SILENCE" and "nota".
  // so WAKEWORD_PHRASE_COUNT being 3 indicates there is only one actively spoken
  // phrase the model is trained to detect
  return WAKEWORD_PHRASE_COUNT == 3;
}

/***************************************************************************//**
 * Recognition callback, invoked whenever we run inference.
 ******************************************************************************/
static void process_brick_callback(recognition_context_t* context)
{
  if (context->error == ERR_NOT_FINISHED) {
    return;
  } else if (context->error != ERR_OK) {
    printf("Error: %d\n", context->error);
    return;
  }

  const char* label = wakeword_label(context->wordID);
  bool red_led_on = wakeword_should_enable_red_led(context->wordID);
  bool green_led_on = wakeword_should_enable_green_led(context->wordID);

  int8_t confidence_pct = (int8_t)(0.0030517578125 * context->nnpqScore); // (100 * (1/32768))
  printf("[k=%6lu] Recognized %-13s (id=%d) with confidence %d%%\n", (long unsigned int)context->timestep, label, context->wordID, confidence_pct);
  // The LED timer timeout for the model must be specified in model/<name>/command.h
  if (should_have_timeout_for_leds()) {
    schedule_timer_to_turn_off_leds(1500);
  }

  if (red_led_on) {
    sl_led_turn_on(&RED_LED);
  } else {
    sl_led_turn_off(&RED_LED);
  }

  if (green_led_on) {
    sl_led_turn_on(&GREEN_LED);
  } else {
    sl_led_turn_off(&GREEN_LED);
  }
}
