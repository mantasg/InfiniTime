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
            void OnReceiveWebCall(std::string resp) override;
            bool OnTouchEvent(TouchEvents event) override;
        private:
            Pinetime::Controllers::WebCallService& webCallService;
            System::SystemTask& systemTask;
    
            lv_obj_t* content_label;
            lv_obj_t* page_label;
            
            uint8_t maxItems = 50;
            uint8_t itemsPerPage = 1;
            uint8_t pageCount = 0;
            uint8_t currentPage = 0;
            uint8_t itemCount = 0;
            std::string values[50] {};
            
            void DrawItems();
            void MakeWebCall();
        };
    }
}
