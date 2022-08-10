/***************************************************************************//**
 * @file model/voice_genie/command.h
 * @brief Model command definitions for the `voice_genie` wakeup word model
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


typedef enum Command {
  VOICE_GENIE = 1,
} Command_t;

char* model_command_label(Command_t command) {
    if (command == VOICE_GENIE) return "Voice Genie";
    return "Unknown";
}

bool model_command_should_enable_red_led(Command_t command) {
    return command == VOICE_GENIE;
}

bool model_command_should_enable_green_led(Command_t command) {
    return command == VOICE_GENIE;
}


const unsigned int model_command_timeout_ms = 2000;