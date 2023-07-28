#pragma once

#include <cstdint>
#include <string>
#include <set>

#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#undef max
#undef min

namespace Pinetime {
    namespace Controllers {
        class NimbleController;
        class MotionController;
        
        class SleepTrackingService {
        public:
            SleepTrackingService(Pinetime::Controllers::NimbleController& nimble,
                                 Pinetime::Controllers::MotionController& motionController);
            void Init();
        private:
            Pinetime::Controllers::NimbleController& nimble;
            Pinetime::Controllers::MotionController& motionController;
            bool fuck;
        };
    }
}
