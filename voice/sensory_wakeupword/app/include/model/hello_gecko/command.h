/***************************************************************************//**
 * @file model/hello_gecko/command.h
 * @brief Model command definitions for the `hello_gecko` wakeup word model
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
  HELLO_GECKO = 1,
  BYE_BYE_GECKO = 2,
  GECKO_GREEN = 3,
  GECKO_RED = 4,
} Command_t;

char* model_command_label(Command_t command) {
    if (command == HELLO_GECKO) return "Hello Gecko";
    if (command == GECKO_GREEN) return "Gecko Green";
    if (command == GECKO_RED) return "Gecko Red";
    if (command == BYE_BYE_GECKO) return "Bye Bye Gecko";
    return "Unknown";
}

bool model_command_should_enable_red_led(Command_t command) {
    return command == HELLO_GECKO || command == GECKO_RED;
}

bool model_command_should_enable_green_led(Command_t command) {
    return command == HELLO_GECKO || command == GECKO_GREEN;
}


const unsigned int model_command_timeout_ms = __UINT32_MAX__;
