#pragma once

#include <cstdint>
#include <string>
#include "displayapp/screens/Screen.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Controllers {
    class WebCallService;
  }

  namespace Applications {
    namespace Screens {
      class WebCall : public Screen {
        public:
          WebCall(Pinetime::Controllers::WebCallService& webCallService);
          ~WebCall() override;
  
          void Refresh() override;
          void OnButtonEvent(lv_obj_t* object, lv_event_t event);
  
        private:
          Pinetime::Controllers::WebCallService& webCallService;
          
          lv_obj_t* percent;
          lv_obj_t* btn_startStop;
          lv_obj_t* label_startStop;
          
          lv_task_t* taskRefresh;
          
          std::string response {"NA"};
      };
    }
  }
}
