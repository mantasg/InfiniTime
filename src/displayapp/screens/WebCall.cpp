#include "displayapp/screens/WebCall.h"
#include "displayapp/InfiniTimeTheme.h"
#include <components/ble/WebCallService.h>
#include <numeric>
#include <sstream>
#include <cmath>

using namespace Pinetime::Applications::Screens;

namespace {
  void btnStartStopEventHandler(lv_obj_t* obj, lv_event_t event) {
    auto* screen = static_cast<WebCall*>(obj->user_data);
    screen->OnButtonEvent(obj, event);
  }
}

WebCall::WebCall(Pinetime::Controllers::WebCallService& webCallService) : webCallService {webCallService} {
    lv_obj_t* act = lv_scr_act();
    act->user_data = this;
    lv_obj_set_event_cb(act, btnStartStopEventHandler);
    
    content_label = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_recolor(content_label, true);
    lv_label_set_long_mode(content_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(content_label, 240);
    lv_label_set_text_static(content_label, "");
    lv_obj_align(content_label, nullptr, LV_ALIGN_IN_TOP_MID, 0, 0);
    
    page_label = lv_label_create(lv_scr_act(), nullptr);
    lv_obj_set_width(page_label, 240);
    lv_label_set_text_static(page_label, "   ");
    lv_obj_align(page_label, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
      
    taskRefresh = lv_task_create(RefreshTaskCallback, 1000, LV_TASK_PRIO_MID, this);
    MakeWebCall();
}

WebCall::~WebCall() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WebCall::OnButtonEvent(lv_obj_t* obj, lv_event_t event) {}

void WebCall::MakeWebCall() {
    lv_label_set_text_static(content_label, "Making web call");
    int const result = webCallService.MakeWebCall();
    if (result > 0) {
        lv_label_set_text_static(content_label, "Web call failed");
    }
}

void WebCall::DrawItems() {
    if (itemCount == 0) {
        lv_label_set_text_static(content_label, "");
        lv_label_set_text_static(page_label, "");
        return;
    }
    
    const uint8_t begin = currentPage * itemsPerPage;
    uint8_t end = currentPage * itemsPerPage + itemsPerPage;
    if (end > itemCount) end = itemCount;

    const std::basic_string<char> &basicString = std::accumulate(
            std::begin(values) + begin,
            std::begin(values) + end,
            std::string(),
            [](std::string &acc, std::string &str) { return acc.empty() ? str : acc + "\n" + str; });

    lv_label_set_text_fmt(content_label, "%s", basicString.c_str());
    lv_label_set_text_fmt(page_label, "%d/%d", currentPage + 1, pageCount);
}

void WebCall::Refresh() {
    if (webCallService.getResponseReceived()) {
        const std::string response = webCallService.getResponse();
        webCallService.reset();

        int counter = 0;
        const std::string del = "\n";
        int start = 0;
        int end = -1 * del.size();
        do {
            if (counter == maxItems) break;
            
            start = end + del.size();
            end = response.find(del, start);
            std::string item = response.substr(start, end - start);
            if (item.length() > maxItemLength) item = item.substr(0, maxItemLength);
            
            if (item.rfind("[W]") == 0) {
                char buffer[item.length() + 9];
                snprintf(buffer, sizeof(buffer), "#FFFF00 %s", item.c_str());
                item = buffer;
            }

            if (item.rfind("[C]") == 0) {
                char buffer[item.length() + 9];
                snprintf(buffer, sizeof(buffer), "#FF0000 %s", item.c_str());
                item = buffer;
            }
            
            values[counter++] = item;
        } while (end != -1);

        currentPage = 0;
        itemCount = counter;
        pageCount = itemCount == 0 ? 0 : ceil((float) itemCount / (float) itemsPerPage); 
        
        DrawItems();
    }
}

bool WebCall::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
    if (event == TouchEvents::SwipeLeft) {
        if (currentPage < pageCount - 1) {
            currentPage++;
            DrawItems();
        }
    } 
    else if (event == TouchEvents::SwipeRight) {
        if (currentPage > 0) {
            currentPage--;
            DrawItems();
        }
    }
    else if (event == TouchEvents::LongTap) {
        MakeWebCall();
    }
    
    return true;
}