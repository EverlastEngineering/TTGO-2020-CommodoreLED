#include "config.h"
#include "timers.h"
#include "headers.h"
#include "eventmgm.h"
#include "gui.h"

const lv_img_dsc_t *number[] = {
		&Number0,
		&Number1,
		&Number2,
		&Number3,
		&Number4,
		&Number5,
		&Number6,
		&Number7,
		&Number8,
		&Number9,
		&Colon, //10
		&Dash, //11
    &Empty //12
	};

str_datetime_t g_data;

void guiSetup(lv_obj_t * _screen) {

	lv_obj_t *img1 = lv_img_create(_screen, NULL);
    lv_img_set_src(img1, &WatchFacePCB);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);

    g_data.minute2 = lv_img_create(_screen, NULL);
   	lv_img_set_src(g_data.minute2, &Dash);
    lv_obj_align(g_data.minute2, NULL, LV_ALIGN_CENTER, 64, 3);

    g_data.minute1 = lv_img_create(_screen, NULL);
    lv_img_set_src(g_data.minute1, &Dash);
    lv_obj_align(g_data.minute1, NULL, LV_ALIGN_CENTER, 26, 3);

    g_data.colon = lv_img_create(_screen, NULL);
    lv_img_set_src(g_data.colon, &Colon);
    lv_obj_align(g_data.colon, NULL, LV_ALIGN_CENTER, 0, 3);

    g_data.hour2 = lv_img_create(_screen, NULL);
    lv_img_set_src(g_data.hour2, &Dash);
    lv_obj_align(g_data.hour2, NULL, LV_ALIGN_CENTER, -29, 3);

    g_data.hour1 = lv_img_create(_screen, NULL);
    lv_img_set_src(g_data.hour1, &Dash);
    lv_obj_align(g_data.hour1, NULL, LV_ALIGN_CENTER, -67, 3);

    lv_img_set_src(g_data.hour1, &Empty);
    lv_img_set_src(g_data.hour2, &Empty);
    lv_img_set_src(g_data.minute1, &Empty);
    lv_img_set_src(g_data.minute2, &Empty);
    lv_img_set_src(g_data.colon, &Empty);
}


void hiddenMode() {
    //c64 stuff
	mode = SECRET_MODE;
    Serial.printf("Hidden Mode!\n");
    lv_obj_t * screen_c64 = lv_obj_create(NULL, NULL);
    lv_obj_t *C64BootupScreen = lv_img_create(screen_c64, NULL);
    lv_img_set_src(C64BootupScreen, &BootupOn1702);
    lv_obj_align(C64BootupScreen, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_scr_load_anim(screen_c64, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 250, 250, false);
    lv_obj_set_event_cb(screen_c64, event_cb);
}

void clockMode(int animationTime) {
	mode = AUTHENTIC_TIME_MODE;
    Serial.printf("Clock Mode!\n");
    lv_obj_t * _screen = lv_obj_create(NULL, NULL);
    guiSetup(_screen);
	lv_scr_load_anim(_screen, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, animationTime, animationTime, false);
    lv_obj_set_event_cb(_screen, event_cb);
}

void clockMode() {
    clockMode(0);
}

void displayNumerals() {
    if (displayed_digit[0] != digit[0]) {
        if (digit[0]==0 && screen == TIME) {
            lv_img_set_src(g_data.hour1, &Empty);
        }
        else {
            lv_img_set_src(g_data.hour1, number[digit[0]]);
        }
        displayed_digit[0] = digit[0];
    }
    if (displayed_digit[1] != digit[1]) {
        lv_img_set_src(g_data.hour2, number[digit[1]]);
        displayed_digit[1] = digit[1];
    }
    if (displayed_digit[2] != digit[2]) {
        lv_img_set_src(g_data.minute1, number[digit[2]]);
        displayed_digit[2] = digit[2];
    }
    if (displayed_digit[3] != digit[3]) {
        lv_img_set_src(g_data.minute2, number[digit[3]]);
        displayed_digit[3] = digit[3];
    }
}

void processDisplay() {
        int colonVisible = 0;
        RTC_Date curr_datetime = watch->rtc->getDateTime();
        int hour = curr_datetime.hour;
        if (hour == 0) hour = 12;
        if (hour > 12) hour = hour-12;
        if (screen == TIME) {
            digit[0] =  (hour/10) % 10;
            digit[1] =  hour % 10;
            digit[2] =  (curr_datetime.minute/10) % 10;
            digit[3] =  curr_datetime.minute % 10;
            colonVisible = curr_datetime.second % 2;
        }
        else if (screen == SECONDS) {
            digit[0] =  12; //empty
            digit[1] =  12; //empty
            digit[2] =  (curr_datetime.second/10) % 10;
            digit[3] =  curr_datetime.second % 10;
            colonVisible = 0;
        }
        else if (screen == DATE) {
            digit[0] =  (curr_datetime.month/10) % 10;
            digit[1] =  curr_datetime.month % 10;
            digit[2] =  (curr_datetime.day/10) % 10;
            digit[3] =  curr_datetime.day % 10;
            colonVisible = 0;
        }
        else if (screen == BLANK) {
            digit[0] =  12; //empty
            digit[1] =  12; //empty
            digit[2] =  12; //empty
            digit[3] =  12; //empty
            colonVisible = 0;
        }
        else if (screen == DASHES) {
            digit[0] =  11; //empty
            digit[1] =  11; //empty
            digit[2] =  11; //empty
            digit[3] =  11; //empty
            colonVisible = 1;
        }
        else if (screen == IAMSUCHABOYCHILD) {
            digit[0] =  8; //empty
            digit[1] =  0; //empty
            digit[2] =  0; //empty
            digit[3] =  8; //empty
            colonVisible = 0;
        }
        else if (screen == D3B9) {
            digit[0] =  0; //empty
            digit[1] =  3; //empty
            digit[2] =  8; //empty
            digit[3] =  9; //empty
            colonVisible = 0;
        }
        else if (screen == B477) {
            digit[0] =  8; //empty
            digit[1] =  4; //empty
            digit[2] =  7; //empty
            digit[3] =  7; //empty
            colonVisible = 0;
        }
        else if (screen == BATTERY || mode == BATTERY_MONITOR) {
            
            float batteryVoltage = watch->power->getBattVoltage() / 1000;
            // int batteryLevel = watch->power->getBattVoltage(); 
            int batteryLevel = (batteryVoltage - 3.5) * 2 * 100;
            // Serial.printf("Battery: %d%%\n", batteryLevel);
            if (batteryLevel > 100) batteryLevel = 100;
            if (batteryLevel < 0) batteryLevel = 0;
            digit[0] = 8;
            digit[1] = (batteryLevel/100) % 10;
            if (digit[1] == 0) {
                digit[1] = 11;
            }
            digit[2] = (batteryLevel/10) % 10;
            digit[3] = (batteryLevel/1) % 10;
            colonVisible = 0;
        }
        if (colonVisible == 1) 
        {
            lv_img_set_src(g_data.colon, &Colon);
        }
        else {
            lv_img_set_src(g_data.colon, &Empty);
        }
        processTimers();
        displayNumerals();    
        // hiddenMode();
}