#include "timers.h"
#include "headers.h"
#include "powermgm.h"
#include "eventmgm.h"
#include "config.h"
#include "gui.h"

int ctr_pressing = 0;
int ctr_pressed_repeat = 0;
int secret_mode = 0;

void event_cb( lv_obj_t * obj, lv_event_t event ) {
    // Serial.println("event_cb");
    switch(event) {
        case LV_EVENT_PRESSED:
            // Serial.println("LV_EVENT_PRESSED");
            clockFaceTimer = 0;
            restartDimmerTimer();
            if (powermode != FULLPOWER) {
                powerMode(FULLPOWER);
            } 
            if (retapTimer != 0) {
                retapCounter++;
                if (retapCounter == 1) {
                    Serial.printf("Date screen.\n");
                    screen = DATE;
                }
                else if (retapCounter == 2) {
                    Serial.printf("Battery screen.\n");
                    screen = BATTERY;
                }
                else if (retapCounter == 3 || retapCounter == 6) {
                    Serial.printf("Blank screen.\n");
                    screen = DASHES;
                }
                else if (retapCounter == 4) {
                    Serial.printf("Toggle debug.\n");
                    screen = D3B9;
                }
                else if (retapCounter == 5) {
                    Serial.printf("Long-term battery monitor screen.\n");
                    screen = B477;
                }
                else if (retapCounter == 10) {
                    Serial.printf("Boob. Teehee.\n");
                    screen = IAMSUCHABOYCHILD;
                }
                // Serial.printf("retapCounter: %d\n", retapCounter);
                // Serial.printf("screen: %d\n", screen);
            }
            else if (screen == BLANK) {
                screen = TIME;
            }
            processDisplay();
            break;
        case LV_EVENT_PRESSING:
            // Serial.printf("LV_EVENT_PRESSING: %d\n", ctr_pressing);
            ctr_pressing++;
            if (ctr_pressing > 60 && screen == TIME) {
                // Serial.printf("LV_EVENT_PRESSING: %d\n", ctr_pressing);
                screen = SECONDS;
            }
            else if (ctr_pressing > 0 && (screen == BLANK || (mode == BATTERY_MONITOR && retapTimer == 0))) {
                // Serial.printf("LV_EVENT_PRESSING: %d\n", ctr_pressing);
                // screen = TIME;
            }
            if (blLevel) {
                restartDimmerTimer();
                restartRetapTimer();
            }
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
            // Serial.println("LV_EVENT_RELEASED");
            if (screen == TIME) {
                resetDisplay();
            }
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
            restartRetapTimer();
            Serial.printf("Gesture Direction: %d\n", (int)lv_indev_get_gesture_dir(lv_indev_get_act()));
            lv_gesture_dir_t direction = (int)lv_indev_get_gesture_dir(lv_indev_get_act());

            detectSecretMode(direction);
            if (secret_mode > 1) {
                return;
            }
            if ((direction) == LV_GESTURE_DIR_BOTTOM) {
                if (mode == SECRET_MODE) {
                    clockMode(250);
                }
                Serial.println("AUTHENTIC_TIME_MODE");
                mode = AUTHENTIC_TIME_MODE;
            }
            else if ((direction) == LV_GESTURE_DIR_TOP) {
                if (mode == SECRET_MODE) {
                    clockMode(250);
                }
                Serial.println("ALWAYS_ON_TIME_MODE");
                mode = ALWAYS_ON_TIME_MODE;
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

void resetDisplay() {
                ctr_pressing = ctr_pressed_repeat = 0;
                if (mode == ALWAYS_ON_TIME_MODE) {
                    screen = TIME;
                    // Serial.printf("Screen: time\n");
                }
                else if (mode == BATTERY_MONITOR) {
                    screen = BATTERY;
                    // Serial.printf("Screen: battery\n");
                }
                else if (mode != BATTERY_MONITOR) {
                    screen = BLANK;
                    // Serial.printf("Screen: BLANK\n");
                }
                processDisplay();
}

void detectSecretMode(lv_gesture_dir_t dir) {
    if (dir == LV_GESTURE_DIR_TOP && secret_mode == 0) {
        secret_mode = 1;
    }
    else if (dir == LV_GESTURE_DIR_TOP && secret_mode == 1) {
        secret_mode = 2;
    }
    else if (dir == LV_GESTURE_DIR_BOTTOM && secret_mode == 2) {
        secret_mode = 3;
    }
    else if (dir == LV_GESTURE_DIR_BOTTOM && secret_mode == 3) {
        secret_mode = 4;
    }
    else if (dir == LV_GESTURE_DIR_LEFT && secret_mode == 4) {
        secret_mode = 5;
    }
    else if (dir == LV_GESTURE_DIR_RIGHT && secret_mode == 5) {
        secret_mode = 6;
    }
    else if (dir == LV_GESTURE_DIR_LEFT && secret_mode == 6) {
        secret_mode = 7;
    }
    else if (dir == LV_GESTURE_DIR_RIGHT && secret_mode == 7) {
        secret_mode = 8;
    }
    else {
        secret_mode = 0;
    }
    if (secret_mode == 8) {
        secret_mode = 0;
        watch->shake();
        hiddenMode();
    }
    Serial.printf("secret: %d\n", secret_mode);
}