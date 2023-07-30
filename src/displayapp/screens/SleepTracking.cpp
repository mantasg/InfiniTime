#include "displayapp/screens/SleepTracking.h"
#include "displayapp/InfiniTimeTheme.h"
#include <components/ble/SleepTrackingService.h>

#define CONFIG_LV_SPRINTF_DISABLE_FLOAT 0
#define LV_SPRINTF_DISABLE_FLOAT 0
#define LV_SPRINTF_CUSTOM 0

using namespace Pinetime::Applications::Screens;

SleepTracking::SleepTracking(Pinetime::Controllers::SleepTrackingService& sleepTrackingService, 
                             DisplayApp* displayApp)
  : sleepTrackingService{sleepTrackingService}, 
    displayApp{displayApp} {
  content_label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(content_label, true);
  lv_label_set_long_mode(content_label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(content_label, 240);
  lv_label_set_text_static(content_label, "Sleep Tracking");
  lv_obj_align(content_label, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  taskRefresh = lv_task_create(RefreshTaskCallback, 300, LV_TASK_PRIO_MID, this);
}

SleepTracking::~SleepTracking() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void SleepTracking::Refresh() {
  displayApp->PushMessage(Display::Messages::GoToRunning);
  
  lv_label_set_text_fmt(content_label, 
                        "prev_X = %0.3f\nprev_Y = %0.3f\nprev_Z = %0.3f\nX = %0.3f\nY = %0.3f\nZ = %0.3f\nE = %0.3f\nE_count = %d\nP = %0.3f", 
                        sleepTrackingService.prev_x, 
                        sleepTrackingService.prev_y, 
                        sleepTrackingService.prev_z, 
                        sleepTrackingService.x, 
                        sleepTrackingService.y, 
                        sleepTrackingService.z,
                        sleepTrackingService.e,
                        sleepTrackingService.e_count,
                        sleepTrackingService.p);
}