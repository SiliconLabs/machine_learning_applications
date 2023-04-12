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
#include "sl_htm_sdr.h"
#include "fort.h"

bool sl_htm_sdr_get_bit(sl_htm_sdr_t *sdr, uint16_t index)
{
  if (index >= sdr->width * sdr->height) {
    printf("Index out of bounds: %d\n", index);
    fflush(stdout);
    while (1);
  }
  return sdr->bits[index];
}

void sl_htm_sdr_set_bit(sl_htm_sdr_t *sdr, uint16_t index, bool value)
{
  if (index >= sdr->width * sdr->height) {
    printf("Index out of bounds: %d\n", index);
    while (1);
  }
  // If the bit is flipped from 0 to 1, increment the active count, and vice versa.
  if (value && !sdr->bits[index]) {
    sdr->num_active_bits++;
  } else if (!value && sdr->bits[index]) {
    sdr->num_active_bits--;
  }
  sdr->bits[index] = value;
}
/**
 * @brief Initialize an SDR.
 *
 * @param sdr
 * @param width
 * @param height
 * @param depth
 */
void sl_htm_sdr_init(sl_htm_sdr_t *sdr, uint8_t width, uint8_t height)
{
  sdr->width = width;
  sdr->height = height;
  sdr->num_active_bits = 0;
  sdr->bits = calloc(width * height, sizeof(bool));
  if (sdr->bits == NULL) {
    printf("Error [%s:%d]: Could not allocate memory for SDR.\n", __FILE__, __LINE__);
    while (1);
  }
}
void sl_htm_sdr_clear(sl_htm_sdr_t *sdr)
{
  memset(sdr->bits, 0, sdr->width * sdr->height * sizeof(bool));
  sdr->num_active_bits = 0;
}
void sl_htm_sdr_shuffle(sl_htm_sdr_t *sdr)
{
  for (uint16_t i = 0; i < sdr->width * sdr->height; i++) {
    uint16_t index1 = rand() % (sdr->width * sdr->height);
    uint16_t index2 = rand() % (sdr->width * sdr->height);
    bool temp = sdr->bits[index1];
    sdr->bits[index1] = sdr->bits[index2];
    sdr->bits[index2] = temp;
  }
}
/**
 * @brief Randomize the SDR to the given sparsity.
 *
 * @param sdr
 * @param sparsity
 */
void sl_htm_sdr_randomize(sl_htm_sdr_t *sdr, float sparsity)
{
  // Clear the SDR
  sl_htm_sdr_clear(sdr);
  // Set N bits to 1
  uint16_t target_active_bits = sparsity * sdr->width * sdr->height;
  for (uint16_t i = 0; i < target_active_bits; i++) {
    sl_htm_sdr_set_bit(sdr, i, true);
  }
  // Shuffle the bit array
  sl_htm_sdr_shuffle(sdr);
}

void sl_htm_sdr_insert(sl_htm_sdr_t* target_sdr, sl_htm_sdr_t* source_sdr, uint8_t x, uint8_t y)
{
  uint16_t target_index = sl_htm_utils_xy_to_index(x, y, target_sdr->width, target_sdr->height);
  for (uint16_t src_index = 0; src_index < source_sdr->width * source_sdr->height; src_index++) {
    bool source_bit = sl_htm_sdr_get_bit(source_sdr, src_index);
    sl_htm_sdr_set_bit(target_sdr, target_index + src_index, source_bit);
  }
}
/**
 * @brief Print the SDR.
 *
 * @param sdr
 */
void sl_htm_sdr_print(sl_htm_sdr_t* sdr)
{
  ft_table_t *table = ft_create_table();
  if (table == NULL) {
    printf("Error [%s:%d]: Failed to create table.\n", __FILE__, __LINE__);
  }
  for (uint8_t y = 0; y < sdr->height; y++) {
    for (uint8_t x = 0; x < sdr->width; x++) {
      ft_printf(table, "%d", sl_htm_sdr_get_bit(sdr, sl_htm_utils_xy_to_index(x, y, sdr->width, sdr->height)));
    }
    ft_ln(table);
  }
  printf("%s", ft_to_string(table));
  ft_destroy_table(table);
}
size_t sl_htm_sdr_memory_size(sl_htm_sdr_t *sdr)
{
  return sizeof(sl_htm_sdr_t) + sdr->width * sdr->height * sizeof(bool);
}
