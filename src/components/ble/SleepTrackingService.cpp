#include "components/ble/SleepTrackingService.h"
#include "components/motion/MotionController.h"
#include "components/ble/NimbleController.h"

using namespace Pinetime::Controllers;

// 0006yyxx-78fc-48fe-8e23-433b3a1942d0
constexpr ble_uuid128_t CharUuid(uint8_t x, uint8_t y) {
  return ble_uuid128_t {
          .u = { .type = BLE_UUID_TYPE_128 },
          .value = { 0xd0, 0x42, 0x19, 0x3a, 0x3b, 0x43, 0x23, 0x8e, 0xfe, 0x48, 0xfc, 0x78, x, y, 0x60, 0x00 }
  };
}

// 00060000-78fc-48fe-8e23-433b3a1942d0
constexpr ble_uuid128_t BaseUuid() {
  return CharUuid(0x00, 0x00);
}

constexpr ble_uuid128_t msUuid {BaseUuid()};
constexpr ble_uuid128_t msEventCharUuid {CharUuid(0x01, 0x00)};

void handleTimer(TimerHandle_t xTimer) {
  auto* sleepTrackingService = static_cast<SleepTrackingService*>(pvTimerGetTimerID(xTimer));
  sleepTrackingService->Update();
}

int SleepTrackingServiceCallback(uint16_t /*conn_handle*/, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  return 0;
}

SleepTrackingService::SleepTrackingService(Pinetime::Controllers::NimbleController& nimble,
                                           Pinetime::Controllers::DateTime& dateTimeController,
                                           Pinetime::Controllers::MotionController& motionController)
  : nimble{nimble},
    dateTimeController{dateTimeController},
    motionController{motionController} {

  characteristicDefinition[0] = {
          .uuid = &msEventCharUuid.u,
          .access_cb = SleepTrackingServiceCallback,
          .arg = this,
          .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
          .val_handle = &eventHandle
  };
  characteristicDefinition[1] = {0};

  serviceDefinition[0] = {
          .type = BLE_GATT_SVC_TYPE_PRIMARY,
          .uuid = &msUuid.u,
          .characteristics = characteristicDefinition
  };
  serviceDefinition[1] = {0};

  timer = xTimerCreate("SleepTrackingService", pdMS_TO_TICKS(500), pdTRUE, this, handleTimer);
  xTimerStart(timer, 0);
}

void SleepTrackingService::Init() {
  int res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

void SleepTrackingService::Update() {
  prev_x = x;
  prev_y = y;
  prev_z = z;

  x = (float) motionController.X() / 100.0;
  y = (float) motionController.Y() / 100.0;
  z = (float) motionController.Z() / 100.0;

  e = e + abs(x - prev_x) + abs(y - prev_y) + abs(z - prev_z);
  e_count++;
  if (e_count < 30) return;

  es[curr] = e;
  e_timestamps[curr] = std::chrono::duration_cast<std::chrono::seconds>(dateTimeController.UTCDateTime().time_since_epoch()).count();
  
  p = 0.0
      + es[(curr_centre + max - 9) % max] * exp(-0.25)
      + es[(curr_centre + max - 8) % max] * exp(-0.5)
      + es[(curr_centre + max - 7) % max] * exp(-1.0)
      + es[(curr_centre + max - 6) % max] * exp(-0.25)
      + es[(curr_centre + max - 5) % max] * exp(-0.5)
      + es[(curr_centre + max - 4) % max] * exp(-1.0)
      + es[(curr_centre + max - 3) % max] * exp(-0.25)
      + es[(curr_centre + max - 2) % max] * exp(-0.5)
      + es[(curr_centre + max - 1) % max] * exp(-1.0)
      + es[(curr_centre       + 0) % max] * exp(0.0)
      + es[(curr_centre       + 1) % max] * exp(-1.0)
      + es[(curr_centre       + 2) % max] * exp(-0.5)
      + es[(curr_centre       + 3) % max] * exp(-0.25)
      + es[(curr_centre       + 4) % max] * exp(-1.0)
      + es[(curr_centre       + 5) % max] * exp(-0.5)
      + es[(curr_centre       + 6) % max] * exp(-0.25)
      + es[(curr_centre       + 7) % max] * exp(-1.0)
      + es[(curr_centre       + 8) % max] * exp(-0.5)
      + es[(curr_centre       + 9) % max] * exp(-0.25);
  
  uint32_t p_value = p;
  uint32_t e_value = es[curr_centre];
  uint64_t timestamp = e_timestamps[curr_centre];
  
  if (pending_items.size() > 1000) {
    pending_items.pop();
  }
  PendingItem item = { timestamp, p_value, e_value };
  pending_items.push(item);
  
  curr = (curr + 1) % max;
  curr_centre = (curr_centre + 1) % max;
  e_count = 0;
  e = 0;

  uint16_t connectionHandle = nimble.connHandle();
  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE)  return;
  
  while (!pending_items.empty()) {
    PendingItem item = pending_items.front();
    unsigned char bytes[16];
    bytes[0] = item.p & 0xFF;
    bytes[1] = (item.p >> 8) & 0xFF;
    bytes[2] = (item.p >> 16) & 0xFF;
    bytes[3] = (item.p >> 24) & 0xFF;
  
    bytes[4] = item.timestamp & 0xFF;
    bytes[5] = (item.timestamp >> 8) & 0xFF;
    bytes[6] = (item.timestamp >> 16) & 0xFF;
    bytes[7] = (item.timestamp >> 24) & 0xFF;
    bytes[8] = (item.timestamp >> 32) & 0xFF;
    bytes[9] = (item.timestamp >> 40) & 0xFF;
    bytes[10] = (item.timestamp >> 48) & 0xFF;
    bytes[11] = (item.timestamp >> 56) & 0xFF;
  
    bytes[12] = item.e & 0xFF;
    bytes[13] = (item.e >> 8) & 0xFF;
    bytes[14] = (item.e >> 16) & 0xFF;
    bytes[15] = (item.e >> 24) & 0xFF;
    
    auto* om = ble_hs_mbuf_from_flat(&bytes, 16);
    int res = ble_gattc_notify_custom(connectionHandle, eventHandle, om);
    if (res != 0) break; // Failed to send. Will attempt to send all pending_items next iteration.
    pending_items.pop();
  }
}