#include "components/ble/WebCallService.h"
#include "components/ble/NimbleController.h"
#include <nrf_log.h>

using namespace Pinetime::Controllers;

int WebCallServiceCallback(uint16_t /*conn_handle*/, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  return static_cast<Pinetime::Controllers::WebCallService*>(arg)->Callback(ctxt);
}

// 0005yyxx-78fc-48fe-8e23-433b3a1942d0
constexpr ble_uuid128_t CharUuid(uint8_t x, uint8_t y) {
 return ble_uuid128_t {
    .u = { .type = BLE_UUID_TYPE_128 },
    .value = { 0xd0, 0x42, 0x19, 0x3a, 0x3b, 0x43, 0x23, 0x8e, 0xfe, 0x48, 0xfc, 0x78, x, y, 0x50, 0x00 }
  };
}

// 00050000-78fc-48fe-8e23-433b3a1942d0
constexpr ble_uuid128_t BaseUuid() {
  return CharUuid(0x00, 0x00);
}

constexpr ble_uuid128_t msUuid {BaseUuid()};
constexpr ble_uuid128_t msEventCharUuid {CharUuid(0x01, 0x00)};

WebCallService::WebCallService(Pinetime::Controllers::NimbleController& nimble) : nimble(nimble) {
    characteristicDefinition[0] = {
      .uuid = &msEventCharUuid.u,
      .access_cb = WebCallServiceCallback,
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
}

void WebCallService::Init() {
  int res = 0;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

int WebCallService::Callback(struct ble_gatt_access_ctxt* ctxt) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    size_t bufferSize = OS_MBUF_PKTLEN(ctxt->om);
    char data[bufferSize + 1];
    os_mbuf_copydata(ctxt->om, 0, bufferSize, data);
    
    response = data;
    responseReceived = true;
  }
    
  return 0;
}

int WebCallService::MakeWebCall() {
  responseReceived = false;
    
  uint16_t connectionHandle = nimble.connHandle();
  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE)  return 1;

  std::string str = "hello";
  unsigned char bytes[str.length()];
  std::copy(str.begin(), str.end(), bytes);
  auto* om = ble_hs_mbuf_from_flat(&bytes, 5);
  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
  
  return 0;
}

std::string WebCallService::getResponse() const {
  return response;
}

bool WebCallService::getResponseReceived() const {
    return responseReceived;
}