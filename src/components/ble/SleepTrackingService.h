#pragma once

#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min

#include <FreeRTOS.h>
#include <timers.h>
#include <queue>

namespace Pinetime {
    namespace Controllers {
        class NimbleController;
        class DateTime;
        class MotionController;
        
        struct PendingItem {
          uint64_t timestamp;
          uint32_t p;
          uint32_t e;
        };
        
        class SleepTrackingService {
        public:
            SleepTrackingService(Pinetime::Controllers::NimbleController& nimble,
                                 Pinetime::Controllers::DateTime& dateTimeController,
                                 Pinetime::Controllers::MotionController& motionController);
            void Init();
            void Update();

            float x;
            float y;
            float z;

            float prev_x;
            float prev_y;
            float prev_z;

            float e;
            float p;
            int e_count = 0;

            std::queue<PendingItem> pending_items {};
            
            int max = 19;
            float es[19] {};
            int64_t e_timestamps[19] {};
            int16_t curr = 0;
            int16_t curr_centre = 9;
        private:
            Pinetime::Controllers::NimbleController& nimble;
            Pinetime::Controllers::DateTime& dateTimeController;
            Pinetime::Controllers::MotionController& motionController;
            TimerHandle_t timer;

            uint16_t eventHandle {};
            struct ble_gatt_chr_def characteristicDefinition[3];
            struct ble_gatt_svc_def serviceDefinition[2];
        };
    }
}