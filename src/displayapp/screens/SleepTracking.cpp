#include "displayapp/screens/SleepTracking.h"
#include "displayapp/InfiniTimeTheme.h"
#include <components/ble/SleepTrackingService.h>

using namespace Pinetime::Applications::Screens;

SleepTracking::SleepTracking() {
  lv_obj_t* content_label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(content_label, true);
  lv_label_set_long_mode(content_label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(content_label, 240);
  lv_label_set_text_static(content_label, "Sleep Tracking");
  lv_obj_align(content_label, nullptr, LV_ALIGN_IN_LEFT_MID, 0, -20);
}

SleepTracking::~SleepTracking() {
  lv_obj_clean(lv_scr_act());
}