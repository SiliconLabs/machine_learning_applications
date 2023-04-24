#define KEY_MEDIA_VOLUMEUP 1
#define KEY_MEDIA_VOLUMEDOWN 1 << 1
#define KEY_MEDIA_MUTE 1 << 2
#define KEY_MEDIA_PLAYPAUSE 1 << 3

#define KEY_ARROW_UP 0x52
#define KEY_ARROW_DOWN 0x51

#define REPORT_PAGE_KEYBOARD 0x01
#define REPORT_PAGE_MEDIA 0x02

#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"

extern uint8_t current_report_page;
extern uint8_t current_key;
extern uint8_t current_modifier_key;
#ifdef __cplusplus
extern "C" {
#endif
void reset_report();
#ifdef __cplusplus
}
#endif
