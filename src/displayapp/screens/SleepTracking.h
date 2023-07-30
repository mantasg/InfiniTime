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
            SleepTracking(Pinetime::Controllers::SleepTrackingService& sleepTrackingService, 
                          DisplayApp* displayApp);
            ~SleepTracking() override;
            void Refresh() override;
        private:
            Pinetime::Controllers::SleepTrackingService& sleepTrackingService;
            DisplayApp* displayApp;

            lv_obj_t* content_label;
            lv_task_t* taskRefresh;
        };
    }
}
