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
                                           Pinetime::Controllers::MotionController& motionController)
  : nimble{nimble},
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
  curr = (curr + 1) % max;
  curr_centre = (curr_centre + 1) % max;

  p = 0.0
      + es[(curr_centre - 9) % max] * exp(-0.25)
      + es[(curr_centre - 8) % max] * exp(-0.5)
      + es[(curr_centre - 7) % max] * exp(-1.0)
      + es[(curr_centre - 6) % max] * exp(-0.25)
      + es[(curr_centre - 5) % max] * exp(-0.5)
      + es[(curr_centre - 4) % max] * exp(-1.0)
      + es[(curr_centre - 3) % max] * exp(-0.25)
      + es[(curr_centre - 2) % max] * exp(-0.5)
      + es[(curr_centre - 1) % max] * exp(-1.0)
      + es[(curr_centre + 0) % max] * exp(0.0)
      + es[(curr_centre + 1) % max] * exp(-1.0)
      + es[(curr_centre + 2) % max] * exp(-0.5)
      + es[(curr_centre + 3) % max] * exp(-0.25)
      + es[(curr_centre + 4) % max] * exp(-1.0)
      + es[(curr_centre + 5) % max] * exp(-0.5)
      + es[(curr_centre + 6) % max] * exp(-0.25)
      + es[(curr_centre + 7) % max] * exp(-1.0)
      + es[(curr_centre + 8) % max] * exp(-0.5)
      + es[(curr_centre + 9) % max] * exp(-0.25);

  e_count = 0;
  e = 0;

  uint16_t connectionHandle = nimble.connHandle();
  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE)  return;

  uint32_t buffer = p;
  auto* om = ble_hs_mbuf_from_flat(&buffer, 4);
  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}