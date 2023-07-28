#include "components/ble/SleepTrackingService.h"
#include "components/motion/MotionController.h"
#include "components/ble/NimbleController.h"

using namespace Pinetime::Controllers;

SleepTrackingService::SleepTrackingService(Pinetime::Controllers::NimbleController& nimble,
                                           Pinetime::Controllers::MotionController& motionController)
  : nimble{nimble},
    motionController{motionController} {
}

void SleepTrackingService::Init() {
}