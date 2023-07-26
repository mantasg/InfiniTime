#include "displayapp/screens/WatchFaceDigital.h"

#include <lvgl/lvgl.h>
#include <cstdio>
#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/settings/Settings.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

void btnHandler(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_LONG_PRESSED) {
        // TODO: might need some more conditions here
        auto* screen = static_cast<WatchFaceDigital*>(obj->user_data);
        screen->displayApp->PushMessage(Pinetime::Applications::Display::Messages::OpenNagios);
    }
}

WatchFaceDigital::WatchFaceDigital(Controllers::DateTime& dateTimeController,
                                   const Controllers::Battery& batteryController,
                                   const Controllers::Ble& bleController,
                                   Controllers::NotificationManager& notificationManager,
                                   Controllers::Settings& settingsController,
                                   Controllers::HeartRateController& heartRateController,
                                   Controllers::MotionController& motionController,
                                   DisplayApp* displayApp,
                                   Pinetime::Controllers::WebCallService& webCallService)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    statusIcons(batteryController, bleController),
    displayApp{displayApp},
    webCallService{webCallService} {

  statusIcons.Create();

  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_LIME);
  lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(false));
  lv_obj_align(notificationIcon, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  label_date = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 100);
  lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x999999));

  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);

  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 35);

  label_time_ampm = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_time_ampm, "");
  lv_obj_align(label_time_ampm, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, -30, -55);

  heartbeatIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
  lv_label_set_text_static(heartbeatValue, "");
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
  lv_label_set_text_static(stepValue, "0");
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);
  
  // Nagios status
  
  nagios_status_btn = lv_btn_create(lv_scr_act(), nullptr);
  lv_obj_set_height(nagios_status_btn, 30);
  lv_obj_set_width(nagios_status_btn, 240);
  lv_obj_align(nagios_status_btn, nullptr, LV_ALIGN_IN_TOP_MID, 0, 140);
  nagios_status_btn->user_data = this;
  lv_obj_set_event_cb(nagios_status_btn, btnHandler);
  
  label_nagios_status_text = lv_label_create(nagios_status_btn, nullptr);
  lv_label_set_text_static(label_nagios_status_text, "???");
  lv_obj_set_style_local_text_color(label_nagios_status_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));

  // On call primary

  on_call_primary_btn = lv_btn_create(lv_scr_act(), nullptr);
  lv_obj_set_width(on_call_primary_btn, 118);
  lv_obj_align(on_call_primary_btn, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 175);
  lv_obj_set_style_local_bg_color(on_call_primary_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x505050));
  on_call_primary_btn->user_data = this;
  lv_obj_set_event_cb(on_call_primary_btn, btnHandler);
  
  on_call_primary_text = lv_label_create(on_call_primary_btn, nullptr);
  lv_label_set_text_static(on_call_primary_text, "???");
  lv_obj_set_style_local_text_color(on_call_primary_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
  
  // On call secondary
  
  on_call_secondary_btn = lv_btn_create(lv_scr_act(), nullptr);
  lv_obj_set_width(on_call_secondary_btn, 118);
  lv_obj_align(on_call_secondary_btn, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 175);
  lv_obj_set_style_local_bg_color(on_call_secondary_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x505050));
  on_call_secondary_btn->user_data = this;
  lv_obj_set_event_cb(on_call_secondary_btn, btnHandler);
  
  on_call_secondary_text = lv_label_create(on_call_secondary_btn, nullptr);
  lv_label_set_text_static(on_call_secondary_text, "???");
  lv_obj_set_style_local_text_color(on_call_secondary_text, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));      
  
  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
  
  webCallService.Subscribe(this);
  webCallService.MakeWebCall("nagios_status");
}

WatchFaceDigital::~WatchFaceDigital() {
  webCallService.Unsubscribe(this);
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceDigital::OnReceiveWebCall(std::string response)  {
  std::string ok = "???";
  std::string warn = "???";
  std::string crit = "???";
  std::string on_call_prim = "???";
  std::string on_call_sec = "???";

  int counter = 0;
  const std::string del = "\n";
  int start = 0;
  int end = -1 * del.size();
  do {
    start = end + del.size();
    end = response.find(del, start);
    std::string item = response.substr(start, end - start);

    if (counter == 0) ok = item;
    else if (counter == 1) warn = item;
    else if (counter == 2) crit = item;
    else if (counter == 3) on_call_prim = item;
    else if (counter == 4) on_call_sec = item;
    counter++;
  } while (end != -1);

  lv_label_set_text_fmt(label_nagios_status_text, "O:%s W:%s C:%s", ok.c_str(), warn.c_str(), crit.c_str());
  if (crit != "0") {
    lv_obj_set_style_local_bg_color(nagios_status_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x009933));
  }
  else if (warn != "0") {
    lv_obj_set_style_local_bg_color(nagios_status_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff9900));
  }
  else {
    lv_obj_set_style_local_bg_color(nagios_status_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xcc0000));
  }

  lv_label_set_text_fmt(on_call_primary_text, "%s", on_call_prim.c_str());
  if (on_call_prim.find("MG") != std::string::npos) {
    lv_obj_set_style_local_bg_color(on_call_primary_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
  }
  else {
    lv_obj_set_style_local_bg_color(on_call_primary_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x505050));
  }

  lv_label_set_text_fmt(on_call_secondary_text, "%s", on_call_sec.c_str());
  if (on_call_sec.find("MG") != std::string::npos) {
    lv_obj_set_style_local_bg_color(on_call_secondary_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
  }
  else {
    lv_obj_set_style_local_bg_color(on_call_secondary_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x505050));
  }
}

void WatchFaceDigital::Refresh() {
  statusIcons.Update();
  
  notificationState = notificationManager.AreNewNotificationsAvailable();
  if (notificationState.IsUpdated()) {
    lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::minutes>(dateTimeController.CurrentDateTime());

  if (currentDateTime.IsUpdated()) {
    uint8_t hour = dateTimeController.Hours();
    uint8_t minute = dateTimeController.Minutes();

    if (minute == 0 || minute % 5 == 0) {
      webCallService.MakeWebCall("nagios_status");
    }
    
    // Modifiy to fiddle with time position
    if (settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[3] = "AM";
      if (hour == 0) {
        hour = 12;
      } else if (hour == 12) {
        ampmChar[0] = 'P';
      } else if (hour > 12) {
        hour = hour - 12;
        ampmChar[0] = 'P';
      }
      lv_label_set_text(label_time_ampm, ampmChar);
      lv_label_set_text_fmt(label_time, "%2d:%02d", hour, minute);
      lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, 0, 0);
    } else {
      lv_label_set_text_fmt(label_time, "%02d:%02d", hour, minute);
      lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 35);
    }

    currentDate = std::chrono::time_point_cast<days>(currentDateTime.Get());
    if (currentDate.IsUpdated()) {
      uint16_t year = dateTimeController.Year();
      uint8_t day = dateTimeController.Day();
      if (settingsController.GetClockType() == Controllers::Settings::ClockType::H24) {
        lv_label_set_text_fmt(label_date,
                              "%s %d %s %d",
                              dateTimeController.DayOfWeekShortToString(),
                              day,
                              dateTimeController.MonthShortToString(),
                              year);
      } else {
        lv_label_set_text_fmt(label_date,
                              "%s %s %d %d",
                              dateTimeController.DayOfWeekShortToString(),
                              dateTimeController.MonthShortToString(),
                              day,
                              year);
      }
      lv_obj_realign(label_date);
    }
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1B1B1B));
      lv_label_set_text_static(heartbeatValue, "");
    }

    lv_obj_realign(heartbeatIcon);
    lv_obj_realign(heartbeatValue);
  }

  stepCount = motionController.NbSteps();
  if (stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
    lv_obj_realign(stepValue);
    lv_obj_realign(stepIcon);
  }
}
