/***************************************************************************//**
 * @file model/go_silabs/command.h
 * @brief Model command definitions for the `go_silabs` wakeup word model
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
  GO_SILABS = 1,
} Command_t;

char* model_command_label(Command_t command) {
    if (command == GO_SILABS) return "Go Silabs";
    return "Unknown";
}

bool model_command_should_enable_red_led(Command_t command) {
    return command == GO_SILABS;
}

bool model_command_should_enable_green_led(Command_t command) {
    return command == GO_SILABS;
}


const unsigned int model_command_timeout_ms = 2000;