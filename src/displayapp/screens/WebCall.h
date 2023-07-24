#pragma once

#include <cstdint>
#include <string>
#include "displayapp/screens/Screen.h"
#include "systemtask/SystemTask.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Controllers {
    class WebCallService;
  }

    namespace Applications::Screens {
        class WebCall : public Screen {
        public:
            WebCall(Pinetime::Controllers::WebCallService& webCallService, System::SystemTask& systemTask);
            ~WebCall() override;
            
            void Refresh() override;
            void OnButtonEvent(lv_obj_t* acc, lv_event_t event);
            bool OnTouchEvent(TouchEvents event) override;
        private:
            Pinetime::Controllers::WebCallService& webCallService;
            System::SystemTask& systemTask;
    
            lv_obj_t* content_label;
            lv_obj_t* page_label;
    
            lv_task_t* taskRefresh;
            
            uint8_t maxItems = 45;
            uint8_t maxItemLength = 20;
            uint8_t itemsPerPage = 9;
            uint8_t pageCount = 0;
            uint8_t currentPage = 0;
            uint8_t itemCount = 0;
            std::string values[45] {};
            
/*            uint8_t itemsPerPage = 7;
            uint8_t pageCount = 3;
            uint8_t currentPage = 0;
            uint8_t itemCount = 18;
            std::string values[21] {
                "zero",
                "one",
                "two",
                "three",
                "four",
                "five",
                "six",
                "seven",
                "eight",
                "nine",
                "ten",
                "eleven",
                "twelve",
                "thirteen",
                "fourteen",
                "fifteen",
                "sixteen",
                "seventeen",
            };
*/
            
            void DrawItems();
            void MakeWebCall();
        };
    }
}
