#include "timers.h"
#include "headers.h"
#include "powermgm.h"
#include "eventmgm.h"
#include "config.h"


int ctr_pressing = 0;
int ctr_pressed_repeat = 0;

void event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch(event) {
        case LV_EVENT_PRESSED:
            // Serial.println("LV_EVENT_PRESSED");
            if (retapTimer != 0) {
                retapCounter++;
                if (screen == TIME && retapCounter == 1) {
                    Serial.printf("Date screen.\n");
                    screen = DATE;
                }
                if (retapCounter == 2) {
                    Serial.printf("Battery screen.\n");
                    screen = BATTERY;
                }
                if (retapCounter == 3 || retapCounter == 6) {
                    Serial.printf("Blank screen.\n");
                    screen = DASHES;
                }
                if (retapCounter == 5) {
                    Serial.printf("Long-term battery monitor screen.\n");
                    screen = BATTERY_MONITOR_MEDIUM;
                }
                if (retapCounter == 10) {
                    Serial.printf("Boob.\n");
                    screen = IAMSUCHABOYCHILD;
                }
                Serial.printf("retapCounter: %d\n", retapCounter);
            }
            break;
        case LV_EVENT_PRESSING:
            ctr_pressing++;
            if (ctr_pressing > 60 && screen == TIME) {
                // Serial.printf("LV_EVENT_PRESSING: %d\n", ctr_pressing);
                screen = SECONDS;
            }
            else if (ctr_pressing > 0 && (screen == BLANK || (screen == BATTERY_MONITOR_MEDIUM && retapTimer == 0))) {
                Serial.printf("LV_EVENT_PRESSING: %d\n", ctr_pressing);
                screen = TIME;
            }
            restartDimmerTimer();
            restartRetapTimer();
			break;
        // case LV_EVENT_PRESS_LOST:
        //     Serial.println("LV_EVENT_PRESS_LOST");
		// 	break;
        case LV_EVENT_SHORT_CLICKED:
            // Serial.println("LV_EVENT_SHORT_CLICKED");
			break;
        // case LV_EVENT_CLICKED:
        //     Serial.println("LV_EVENT_CLICKED");
		// 	break;
        case LV_EVENT_LONG_PRESSED:
            //Serial.println("LV_EVENT_LONG_PRESSED");
			break;
        case LV_EVENT_LONG_PRESSED_REPEAT:
            ctr_pressed_repeat++;
            //Serial.printf("LV_EVENT_LONG_PRESSED_REPEAT: %d\n", ctr_pressed_repeat);
            
			break;
        case LV_EVENT_RELEASED:
            Serial.println("LV_EVENT_RELEASED");
            ctr_pressing = ctr_pressed_repeat = 0;
			break;
        // case LV_EVENT_DRAG_BEGIN:
        //     Serial.println("LV_EVENT_DRAG_BEGIN");
		// 	break;
        // case LV_EVENT_DRAG_END:
        //     Serial.println("LV_EVENT_DRAG_END");
		// 	break;
        // case LV_EVENT_DRAG_THROW_BEGIN:
        //     Serial.println("LV_EVENT_DRAG_THROW_BEGIN");
		// 	break;
        case LV_EVENT_GESTURE:
            Serial.printf("Gesture Direction: %d\n", (int)lv_indev_get_gesture_dir(lv_indev_get_act()));
            if ((int)lv_indev_get_gesture_dir(lv_indev_get_act()) == 0) {
                // hiddenMode();
            }
            
			// Serial.println("LV_EVENT_GESTURE");
			break;
        // case LV_EVENT_KEY:
        //     Serial.println("LV_EVENT_KEY");
		// 	break;
        // case LV_EVENT_FOCUSED:
        //     Serial.println("LV_EVENT_FOCUSED");
		// 	break;
        // case LV_EVENT_DEFOCUSED:
        //     Serial.println("LV_EVENT_DEFOCUSED");
		// 	break;
        // case LV_EVENT_LEAVE:
        //     Serial.println("LV_EVENT_LEAVE");
		// 	break;
        // case LV_EVENT_VALUE_CHANGED:
        //     Serial.println("LV_EVENT_VALUE_CHANGED");
		// 	break;
        // case LV_EVENT_INSERT:
        //     Serial.println("LV_EVENT_INSERT");
		// 	break;
        // case LV_EVENT_REFRESH:
        //     Serial.println("LV_EVENT_REFRESH");
		// 	break;
        // case LV_EVENT_APPLY:
        //     Serial.println("LV_EVENT_APPLY");
		// 	break;
        // case LV_EVENT_CANCEL:
        //     Serial.println("LV_EVENT_CANCEL");
		// 	break;
        // case LV_EVENT_DELETE:
        //     Serial.println("LV_EVENT_DELETE");
		// 	break;
        // default:
        //     Serial.println("Unknown event");
		// 	break;
        }
}