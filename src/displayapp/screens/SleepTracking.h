#pragma once

#include <cstdint>
#include <string>
#include "displayapp/screens/Screen.h"
#include "systemtask/SystemTask.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
    namespace Controllers {
        class SleepTrackingService;
    }

    namespace Applications::Screens {
        class SleepTracking : public Screen {
        public:
            SleepTracking(Pinetime::Controllers::SleepTrackingService& sleepTrackingService);
            ~SleepTracking() override;
        private:
            Pinetime::Controllers::SleepTrackingService& sleepTrackingService;
            bool todo;
        };
    }
}
