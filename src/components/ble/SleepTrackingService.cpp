#include "components/ble/SleepTrackingService.h"
#include "components/motion/MotionController.h"
#include "components/ble/NimbleController.h"

using namespace Pinetime::Controllers;

void handleTimer(TimerHandle_t xTimer) {
  //auto* sleepTrackingService = static_cast<SleepTrackingService*>(pvTimerGetTimerID(xTimer));
}

SleepTrackingService::SleepTrackingService(Pinetime::Controllers::NimbleController& nimble,
                                           Pinetime::Controllers::MotionController& motionController)
  : nimble{nimble},
    motionController{motionController} {
  
  timer = xTimerCreate("SleepTrackingService", pdMS_TO_TICKS(1000), pdTRUE, this, handleTimer);
  xTimerStart(timer, 0);
}

void SleepTrackingService::Init() {
}