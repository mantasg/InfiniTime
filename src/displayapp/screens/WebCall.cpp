#include "displayapp/screens/WebCall.h"
#include "displayapp/InfiniTimeTheme.h"
#include <components/ble/WebCallService.h>

using namespace Pinetime::Applications::Screens;

namespace {
  void btnStartStopEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<WebCall*>(obj->user_data);
    screen->OnButtonEvent(obj, event);
  }
}

WebCall::WebCall(Pinetime::Controllers::WebCallService& webCallService) : webCallService {webCallService} {
  percent = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(percent, true);
  lv_label_set_long_mode(percent, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(percent, 240);
  
  lv_label_set_text_static(percent, "");
  //lv_label_set_align(percent, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(percent, nullptr, LV_ALIGN_IN_TOP_MID, 0, 0);

  btn_startStop = lv_btn_create(lv_scr_act(), nullptr);
  btn_startStop->user_data = this;
  lv_obj_set_height(btn_startStop, 50);
  lv_obj_set_width(btn_startStop, 240);
  lv_obj_set_event_cb(btn_startStop, btnStartStopEventHandler);
  lv_obj_align(btn_startStop, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  label_startStop = lv_label_create(btn_startStop, nullptr);
  lv_label_set_text_static(label_startStop, "Make Web Call");

  taskRefresh = lv_task_create(RefreshTaskCallback, 1000, LV_TASK_PRIO_MID, this);
  Refresh();
}

WebCall::~WebCall() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WebCall::OnButtonEvent(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
        lv_label_set_text_static(percent, "Making web call");
        int const result = webCallService.MakeWebCall();
        if (result > 0) {
            lv_label_set_text_static(percent, "Web call failed");            
        }
    }
}

void WebCall::Refresh() {
    if (webCallService.getResponseReceived()) {
        response = webCallService.getResponse();
        lv_label_set_text_static(percent, response.data());
    }
}
