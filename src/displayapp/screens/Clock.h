#pragma once

#include <lvgl/src/lv_core/lv_obj.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include <components/heartrate/HeartRateController.h>
#include "displayapp/screens/Screen.h"
#include "components/datetime/DateTimeController.h"
#include "components/ble/weather/WeatherService.h"

namespace Pinetime {
  namespace Controllers {
    class Settings;
    class Battery;
    class Ble;
    class NotificationManager;
    class MotionController;
  }

  namespace Applications {
    namespace Screens {
      class Clock : public Screen {
      public:
        Clock(Controllers::DateTime& dateTimeController,
              const Controllers::Battery& batteryController,
              const Controllers::Ble& bleController,
              Controllers::NotificationManager& notificationManager,
              Controllers::Settings& settingsController,
              Controllers::HeartRateController& heartRateController,
              Controllers::MotionController& motionController,
              Controllers::FS& filesystem,
              DisplayApp* displayApp);
        ~Clock() override;

        bool OnTouchEvent(TouchEvents event) override;
        bool OnButtonPushed() override;

      private:
        Controllers::DateTime& dateTimeController;
        const Controllers::Battery& batteryController;
        const Controllers::Ble& bleController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;
        Controllers::HeartRateController& heartRateController;
        Controllers::MotionController& motionController;
        Controllers::FS& filesystem;
        DisplayApp* displayApp;

        std::unique_ptr<Screen> screen;
        std::unique_ptr<Screen> WatchFaceDigitalScreen();
      };
    }
  }
}
