#pragma once

#include <cstdint>
#include <string>

#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min

namespace Pinetime {
  namespace System {
    class SystemTask;
  }

  namespace Controllers {
    class NimbleController;

    class WebCallService {
    public:
      WebCallService(Pinetime::Controllers::NimbleController& nimble);
      void Init();
      int MakeWebCall(std::string label);
      int Callback(struct ble_gatt_access_ctxt* ctxt);
      std::string getResponse() const;
      bool getResponseReceived() const;
      void reset();
      
    private:
      Pinetime::Controllers::NimbleController& nimble;
      uint16_t eventHandle {};
      
      bool responseReceived = false;
      std::string response {"NA"};
      
      struct ble_gatt_chr_def characteristicDefinition[3];
      struct ble_gatt_svc_def serviceDefinition[2];
    };
  }
}
