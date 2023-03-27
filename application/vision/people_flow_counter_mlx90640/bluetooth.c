#include "bluetooth.h"
#include <stdio.h>
static uint8_t notification_enabled = 0;
static uint8_t connection_handle = 0;

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;
void assert_status(sl_status_t sc)
{
  if (sc != SL_STATUS_OK) {
    app_log("Error occured: 0x%04X\n", sc);
  }
}
void start_advertising()
{
  sl_status_t sc;
  // Generate data for advertising
  sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                             sl_bt_advertiser_general_discoverable);
  assert_status(sc);
  // Start advertising and enable connections.

  sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                     sl_bt_advertiser_connectable_scannable);
  assert_status(sc);
  app_log_info("Started advertising\n");
}
/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d\n",
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);
      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      assert_status(sc);

      // Pad and reverse unique ID to get System ID.
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      assert_status(sc);

      app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                   address_type ? "static random" : "public device",
                   address.addr[5],
                   address.addr[4],
                   address.addr[3],
                   address.addr[2],
                   address.addr[1],
                   address.addr[0]);

      // Disable bonding for easy of use.
      sc = sl_bt_sm_set_bondable_mode(0);
      assert_status(sc);
      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      assert_status(sc);

      start_advertising();
      break;
    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      notification_enabled = 0;
      connection_handle = evt->data.evt_connection_opened.connection;
      sc = sl_bt_sm_increase_security(evt->data.evt_connection_opened.connection);
      assert_status(sc);
      app_log_info("Connection opened.\n");
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      notification_enabled = 0;
      connection_handle = 0xff;
      app_log_info("Connection closed.\n");
      start_advertising();
      break;
    case sl_bt_evt_sm_bonding_failed_id:
      app_log("Bonding failed: 0x%04X\n", evt->data.evt_sm_bonding_failed.reason);
      break;
    case sl_bt_evt_gatt_server_characteristic_status_id:
      app_log_info("Characteristic was attempted changed: %d\n", evt->data.evt_gatt_server_characteristic_status.characteristic);
      if (evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_data_capture_data) {
        // A local Client Characteristic Configuration descriptor was changed in
        // the gattdb_report characteristic.
        if (evt->data.evt_gatt_server_characteristic_status.client_config_flags
            & sl_bt_gatt_notification) {
          // The client just enabled the notification.
          notification_enabled = 1;
          app_log_info("Notification enabled.\n");
        } else {
          notification_enabled = 0;
          app_log_info("Notification disabled.\n");
        }
      }
      break;
    default:
      app_log("0x%08X\n", SL_BT_MSG_ID(evt->header));
      break;
  }
}
#define PACKET_SIZE  242
// The packet buffer is used to store the data to be sent.
static uint8_t packet[PACKET_SIZE];
// The remaining_packet variable is used to keep track of how much capacity is left in the packet.
static size_t remaining_packet = PACKET_SIZE;
/**
 * @brief Adds data to the packet and sends it when the packet is full.
 *
 * @param buf_ptr
 * @param len
 */
void data_notify(void* buf_ptr, size_t len)
{
  if (connection_handle == 0xff || notification_enabled == 0) {
    return;
  }
  sl_status_t sc;
  size_t remaining_len = len;
  while (remaining_len > 0) {
    printf("remaining_len: %d", remaining_len);
    size_t len_to_send = remaining_packet;
    if (remaining_len <= len_to_send) {
      len_to_send = remaining_len;
    }
    memcpy(packet + (PACKET_SIZE - remaining_packet), buf_ptr + (len - remaining_len), len_to_send);
    remaining_packet -= len_to_send;
    remaining_len -= len_to_send;
    if (remaining_packet == 0) {
      sc = sl_bt_gatt_server_send_notification(
        connection_handle,
        gattdb_data_capture_data,
        PACKET_SIZE,
        packet);
      if (sc != SL_STATUS_OK) {
        app_log_error("[E: 0x%04x] Failed to send characteristic notification\n", (int)sc);
      }
      remaining_packet = PACKET_SIZE;
      sl_sleeptimer_delay_millisecond(10);
    }
  }
}
