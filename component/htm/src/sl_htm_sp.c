/***************************************************************************//**
 * @file
 * @brief HTM Implementation
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
#include "sl_htm_sp.h"
#include "sl_htm_utils.h"
#include "fort.h"
/**
 * @brief Check if a connection is active. A connection is active if it is connected to an active bit in the input SDR.
 *
 * @param connection
 * @param input_sdr
 */
bool sl_htm_sp_connection_active(sl_htm_sp_connection_t* connection, sl_htm_sdr_t* input_sdr)
{
  uint16_t input_index = sl_htm_utils_xy_to_index(connection->sdr_x, connection->sdr_y, input_sdr->width, input_sdr->height);
  return input_sdr->bits[input_index];
}
/**
 * @brief Check if a connection is connected. A connection is connected if its permanence is greater than or equal to the permanence threshold.
 *
 * @param connection
 * @param sp
 */
bool sl_htm_sp_connection_connected(sl_htm_sp_connection_t* connection, sl_htm_sp_t* sp)
{
  return connection->permanence >= sp->parameters.permanence_threshold;
}
void sl_htm_sp_init_connection(sl_htm_sp_t* sp, uint8_t column_x, uint8_t column_y, uint8_t input_x, uint8_t input_y, uint16_t connection_idx)
{
  uint16_t column_index = column_x + column_y * sp->width;
  // Set up the connection
  sp->columns[column_index].connections[connection_idx].sdr_x = input_x;
  sp->columns[column_index].connections[connection_idx].sdr_y = input_y;
  sp->columns[column_index].connections[connection_idx].sp_x = column_x;
  sp->columns[column_index].connections[connection_idx].sp_y = column_y;

  sp->columns[column_index].connections[connection_idx].permanence = rand() % UINT8_MAX;
}
void sl_htm_sp_init_column(sl_htm_sp_t* sp, uint8_t column_x, uint8_t column_y, uint8_t input_width, uint8_t input_height)
{
  if (input_width < sp->width || input_height < sp->height) {
    printf("Error [%s:%d]: Input SDR must be larger than or equal to the spatial pooler.\n", __FILE__, __LINE__);
    while (1);
  }

  uint16_t column_index = column_x + column_y * sp->width;
  sp->columns[column_index].num_connections = sp->parameters.potential_radius * sp->parameters.potential_radius * 2 * sp->parameters.potential_pct;
  sp->columns[column_index].connections = malloc(sizeof(sl_htm_sp_connection_t) * sp->columns[column_index].num_connections);
  if (sp->columns[column_index].connections == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for column connections.\n", __FILE__, __LINE__);
    while (1);
  }
  sp->columns[column_index].overlap_score = 0;
  sp->columns[column_index].column_x = column_x;
  sp->columns[column_index].column_y = column_y;

  // Visually, the columns are centered in the input SDR, so we need to calculate the offset.
  uint8_t input_x_offset = (input_width - sp->width) / 2;
  uint8_t input_y_offset = (input_height - sp->height) / 2;
  // Set up connections for current column
  for (uint16_t connection_idx = 0; connection_idx < sp->columns[column_index].num_connections; connection_idx++) {
    bool connection_found = false;
    uint8_t tries = 0;
    uint8_t max_tries = 50;
    uint8_t input_x = 0;
    uint8_t input_y = 0;
    while (!connection_found) {
      // If we have tried 50 times to find a unique connection, then we have a problem
      if (tries >= max_tries) {
        printf("Error [%s:%d]: Could not find a unique connection for column (%d, %d).\n", __FILE__, __LINE__, column_x, column_y);
        while (1);
      }
      tries++;
      // Pick a random x and y in the input that is within the potential radius relative to the current column
      input_x = rand() % (sp->parameters.potential_radius * 2 + 1) + column_x - sp->parameters.potential_radius + input_x_offset;
      input_y = rand() % (sp->parameters.potential_radius * 2 + 1) + column_y - sp->parameters.potential_radius + input_y_offset;
      // If the random x and y are outside the input, wrap around
      // Cannot be less than 0 due to data type
      if (input_x >= input_width) {
        input_x -= input_width;
      }
      if (input_y >= input_height) {
        input_y -= input_height;
      }
      bool unique_connection = true;
      // Check if any other connection in the column points to the same input bit
      for (uint16_t other_connection_idx = 0; other_connection_idx < connection_idx; other_connection_idx++) {
        sl_htm_sp_connection_t* other_connection = &sp->columns[column_index].connections[other_connection_idx];
        // Check if the other connection has been set up
        if (other_connection->sdr_x == 0 && other_connection->sdr_y == 0) {
          // If not set up, check the next connection
          continue;
        }

        // Compare the input x and y of the other connection to the current input x and y
        if (other_connection->sdr_x == input_x && other_connection->sdr_y == input_y) {
          // If equal, then the connection is not unique and we need to try again
          unique_connection = false;
          break;
        }
      }
      // If the connection is unique, then we can use it
      if (unique_connection) {
        connection_found = true;
      }
    }

    // Initialize the connection
    sl_htm_sp_init_connection(sp, column_x, column_y, input_x, input_y, connection_idx);
  }
}
static sl_htm_sp_column_t** top_columns = NULL;
void sl_htm_sp_init(sl_htm_sp_t* sp, uint8_t input_width, uint8_t input_height, uint8_t output_width, uint8_t output_height)
{
  sp->width = output_width;
  sp->height = output_height;
  sp->columns = malloc(sizeof(sl_htm_sp_column_t) * sp->width * sp->height);
  if (sp->columns == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for spatial pooler columns.\n", __FILE__, __LINE__);
    while (1);
  }
  // Initialize the columns
  for (uint8_t column_x = 0; column_x < sp->width; column_x++) {
    for (uint8_t column_y = 0; column_y < sp->height; column_y++) {
      sl_htm_sp_init_column(sp, column_x, column_y, input_width, input_height);
    }
  }
  top_columns = malloc(sizeof(sl_htm_sp_column_t*) * sp->width * sp->height * sp->parameters.sparsity);
  if (top_columns == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for top columns\n", __FILE__, __LINE__);
    while (1);
  }
}
/**
 * @brief Initialize the spatial pooler parameters to some default values
 *
 */
void sl_htm_sp_init_default_params(sl_htm_sp_parameters_t* params)
{
  params->sparsity = 0.2f;

  params->potential_radius = 8;
  params->potential_pct = 0.2f;

  params->overlap_score_threshold = 2 * params->potential_radius * params->potential_radius * params->potential_pct * 0.1f;

  params->permanence_threshold = 128;
  params->permanence_increment = 15;
  params->permanence_decrement = 15;
}
void sl_htm_sp_get_top_columns(sl_htm_sp_column_t** top_columns, uint16_t num_active_columns, sl_htm_sp_t* sp)
{
  // Clear the top columns
  memset(top_columns, 0, num_active_columns * sizeof(sl_htm_sp_column_t*));
  // Go through each column
  for (uint16_t column_idx = 0; column_idx < sp->width * sp->height; column_idx++) {
    // Go through each of the already found top columns
    for (uint16_t top_column_idx = 0; top_column_idx < num_active_columns; top_column_idx++) {
      bool empty = top_columns[top_column_idx] == NULL;
      // If the current top column is empty, insert the current column
      if (empty) {
        top_columns[top_column_idx] = &sp->columns[column_idx];
        break;
      }
      // If the current column has a higher overlap score than the current top column
      else if (sp->columns[column_idx].overlap_score > top_columns[top_column_idx]->overlap_score) {
        // Shift all the columns after the current top column down one
        for (uint16_t shift_idx = num_active_columns; shift_idx > top_column_idx; shift_idx--) {
          top_columns[shift_idx] = top_columns[shift_idx - 1];
        }
        // Insert the current column into the top columns
        top_columns[top_column_idx] = &sp->columns[column_idx];
        break;
      }
    }
  }
}
/**
 * @brief Compute the overlap score for each column in the spatial pooler
 *
 * @param sp
 * @param sdr
 */
void sl_htm_sp_execute_overlap(sl_htm_sp_t* sp, sl_htm_sdr_t* input_sdr)
{
  for (uint16_t column_x = 0; column_x < sp->width; column_x++) {
    for (uint16_t column_y = 0; column_y < sp->height; column_y++) {
      uint16_t column_index = column_x + column_y * sp->width;
      sp->columns[column_index].overlap_score = 0;

      for (uint16_t connection_idx = 0; connection_idx < sp->columns[column_index].num_connections; connection_idx++) {
        bool connected = sl_htm_sp_connection_connected(&sp->columns[column_index].connections[connection_idx], sp);

        bool active = sl_htm_sp_connection_active(&sp->columns[column_index].connections[connection_idx], input_sdr);

        if (connected && active) {
          sp->columns[column_index].overlap_score++;
        }
      }
    }
  }
}
/**
 * @brief Activate the top columns in the spatial pooler based on the overlap score so that the number of active columns is equal to the desired sparsity
 *
 * @param sp
 */
void sl_htm_sp_execute_columns(sl_htm_sp_t* sp, sl_htm_sdr_t* output_sdr, sl_htm_sp_column_t** top_columns, uint16_t num_active_columns)
{
  // Clear the output SDR
  sl_htm_sdr_clear(output_sdr);
  // Activate the top columns
  for (uint16_t top_column_idx = 0; top_column_idx < num_active_columns; top_column_idx++) {
    sl_htm_sp_column_t* column = top_columns[top_column_idx];
    sl_htm_sdr_set_bit(output_sdr, sl_htm_utils_xy_to_index(column->column_x, column->column_y, sp->width, sp->height), true);
  }
}
void sl_htm_sp_execute_learning(sl_htm_sp_t *sp, sl_htm_sdr_t *input_sdr, bool learn, sl_htm_sp_column_t** top_columns, uint16_t num_active_columns)
{
  if (learn) {
    // For each active column
    for (uint16_t column_idx = 0; column_idx < num_active_columns; column_idx++) {
      sl_htm_sp_column_t* column = top_columns[column_idx];
      // For each connection
      for (uint16_t connection_idx = 0; connection_idx < column->num_connections; connection_idx++) {
        // If the connection is active
        if (sl_htm_sp_connection_active(&column->connections[connection_idx], input_sdr)) {
          // Increase the permanence
          if (column->connections[connection_idx].permanence > UINT8_MAX - sp->parameters.permanence_increment) {
            column->connections[connection_idx].permanence = UINT8_MAX;
          } else {
            column->connections[connection_idx].permanence += sp->parameters.permanence_increment;
          }
        } else {
          // Decrease the permanence
          if (column->connections[connection_idx].permanence > sp->parameters.permanence_threshold) {
            column->connections[connection_idx].permanence -= sp->parameters.permanence_decrement;
          } else {
            column->connections[connection_idx].permanence = 0;
          }
        }
      }
    }
  }
}

void sl_htm_sp_execute(sl_htm_sp_t* sp, sl_htm_sdr_t* input_sdr, sl_htm_sdr_t* output_sdr, bool learn)
{
  // Compute the overlap score for each column
  sl_htm_sp_execute_overlap(sp, input_sdr);

  // Activate the top columns
  uint16_t num_active_columns = sp->width * sp->height * sp->parameters.sparsity;

  sl_htm_sp_get_top_columns(top_columns, num_active_columns, sp);
  sl_htm_sp_execute_columns(sp, output_sdr, top_columns, num_active_columns);
  // Perform learning
  sl_htm_sp_execute_learning(sp, input_sdr, learn, top_columns, num_active_columns);
}

void sl_htm_sp_print(sl_htm_sp_t* sp)
{
  uint16_t num_active_columns = sp->width * sp->height * sp->parameters.sparsity;
  sl_htm_sp_get_top_columns(top_columns, num_active_columns, sp);
  ft_table_t* table = ft_create_table();
  if (table == NULL) {
    printf("Error [%s:%d]: Failed to create table.\n", __FILE__, __LINE__);
  }
  for (uint16_t column_y = 0; column_y < sp->height; column_y++) {
    for (uint16_t column_x = 0; column_x < sp->width; column_x++) {
      uint16_t overlap_score = sp->columns[sl_htm_utils_xy_to_index(column_x, column_y, sp->width, sp->height)].overlap_score;
      bool active = false;
      for (uint16_t active_column_idx = 0; active_column_idx < num_active_columns; active_column_idx++) {
        if (top_columns[active_column_idx]->column_x == column_x && top_columns[active_column_idx]->column_y == column_y) {
          active = true;
          break;
        }
      }
      ft_printf(table, "%d (%hu)", active, overlap_score);
    }
    ft_ln(table);
  }
  printf("%s", ft_to_string(table));
  ft_destroy_table(table);
}
size_t sl_htm_sp_memory_size(sl_htm_sp_t* sp)
{
  // This is the dynamic version of the memory size calculation
  // size_t sp_memory = sizeof(sl_htm_sp_t);
  // uint16_t num_columns = sp->width * sp->height;
  // size_t column_memory = num_columns * sizeof(sl_htm_sp_column_t);
  // size_t connection_memory = 0;
  // for (uint16_t i = 0; i < num_columns; i++) {
  //   connection_memory += sp->columns[i].num_connections * sizeof(sl_htm_sp_connection_t);
  // }

  // return column_memory + connection_memory + sp_memory;

  // This is the static version of the memory size calculation
  size_t size = 0;
  size_t num_columns = sp->width * sp->height;
  size_t num_connections_per_column = sp->parameters.potential_radius * sp->parameters.potential_radius * 2 * sp->parameters.potential_pct;
  size += sizeof(sl_htm_sp_t);
  size += num_columns * sizeof(sl_htm_sp_column_t);
  size += num_columns * num_connections_per_column * sizeof(sl_htm_sp_connection_t);
  return size;
}
