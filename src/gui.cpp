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
int set_clock_screen = 0;// 0 = clock, 1 = year, 2 = month / day

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
            digit[0] =  11; //dash
            digit[1] =  11; //dash
            digit[2] =  11; //dash
            digit[3] =  11; //dash
            colonVisible = 1;
        }
        else if (screen == IAMSUCHABOYCHILD) {
            digit[0] =  8; //B
            digit[1] =  0; //O
            digit[2] =  0; //O
            digit[3] =  8; //B
            colonVisible = 0;
        }
        else if (screen == D3B9) {
            digit[0] =  0; //D
            digit[1] =  3; //E
            digit[2] =  8; //B
            digit[3] =  9; //G
            colonVisible = 0;
        }
        else if (screen == B477) {
            digit[0] =  8; //B
            digit[1] =  4; //A
            digit[2] =  7; //T
            digit[3] =  7; //T
            colonVisible = 0;
        }
        else if (screen == SET537) {
            digit[0] =  5; //S
            digit[1] =  3; //E
            digit[2] =  7; //T
            digit[3] =  12; //empty
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
        else if (screen == SETCLOCK) {
            if (set_clock_screen == 0) {
                setClock();
                colonVisible = 1;
            }
            else if (set_clock_screen == 1) {
                setYear();
                colonVisible = 0;
            }
            else if (set_clock_screen == 2) {
                setMonthDay();
                colonVisible = 0;
            }
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

// setClock parameters
int settingDigit = 0; // which digit is being set in 0 first digit format
int timeSetCandidateDigit = 0; // the 'candidate' digit 


int proposed_digit[4] = {0,0,0,0};
int settingClock = 0;
int setClockTimer = 0;

int proposedYear = 0;
int proposedMonth = 0;
int proposedDay = 0;
int proposedHour = 0;
int proposedMinute = 0;



void setYear() {
    setClockTimer++; // used to flash the digit very quickly to show which is being set

    // Serial.printf("proposed_digits: %d %d %d %d\n",proposed_digit[0],proposed_digit[1],proposed_digit[2],proposed_digit[3]);
    digit[0] =  proposed_digit[0];
    digit[1] =  proposed_digit[1];
    digit[2] =  proposed_digit[2];
    digit[3] =  proposed_digit[3];
    
    if (setClockTimer % 2) {
        digit[0] = 12; //empty
        digit[2] = 12; //empty
    }
    else {
        digit[1] = 12; //empty
        digit[3] = 12; //empty
    }
}

void setMonthDay() {
    setClockTimer++; // used to flash the digit very quickly to show which is being set

    // Serial.printf("proposed_digits: %d %d %d %d\n",proposed_digit[0],proposed_digit[1],proposed_digit[2],proposed_digit[3]);
    digit[0] =  proposed_digit[0];
    digit[1] =  proposed_digit[1];
    digit[2] =  proposed_digit[2];
    digit[3] =  proposed_digit[3];
    
    if (setClockTimer % 2) {
        digit[settingDigit] = 12; //empty
    }
    else {
        digit[settingDigit+1] = 12; //empty
    }
}

void setClock() {
    restartDimmerTimer();
    setClockTimer++; // used to flash the digit very quickly to show which is being set
    // Serial.printf("setClockTimer %d\n", setClockTimer);
    // int colonVisible = 0;

    if (!settingClock) {
        settingClock = 1;
        RTC_Date curr_datetime = watch->rtc->getDateTime();
        proposedHour = curr_datetime.hour;
        proposedMinute = curr_datetime.minute;
        proposedYear = curr_datetime.year;
        proposedMonth = curr_datetime.month;
        proposedDay = curr_datetime.day;
        // Serial.printf("Setting up clock to hour: %d", proposedHour);
        proposed_digit[0] = (proposedHour/10) % 10;
        proposed_digit[1] = proposedHour % 10;
        proposed_digit[2] = (proposedMinute/10) % 10;
        proposed_digit[3] = proposedMinute % 10;
    }

    // char buf[128];
    // watch->tft->setTextColor(0x444, TFT_BLACK);
    // snprintf(buf, sizeof(buf), "24 hour clock is used to set time");
    // watch->tft->drawString(buf, 10, 10, 2);

    digit[0] =  proposed_digit[0];
    digit[1] =  proposed_digit[1];
    digit[2] =  proposed_digit[2];
    digit[3] =  proposed_digit[3];
    
    if (setClockTimer % 2) {
        digit[settingDigit] = 12; //empty
    }
    else {
        digit[settingDigit+1] = 12; //empty
    }
}


void setTime() {
    if (!settingClock) 
        return;
    screen = TIME;
    settingClock = 0;
    Serial.println("TIME SET");
    watch->rtc->setDateTime(proposedYear, proposedMonth, proposedDay, proposedHour, proposedMinute, 0);
}

void receiveEventForSetClock(directions direction) {
    // RTC_Date curr_datetime = watch->rtc->getDateTime();

    if (direction == UP)
    {
        Serial.println("UP");
        if (set_clock_screen == 0) {
            if (settingDigit==0) {
                proposedHour++;
                if (proposedHour < 0) proposedHour = 0;
                if (proposedHour > 23) proposedHour = 23;
            }
            if (settingDigit==2) {
                proposedMinute++;
                if (proposedMinute < 0) proposedMinute = 59;
                if (proposedMinute > 59) proposedMinute = 0;
            }
        }
        else if (set_clock_screen == 1) {
            proposedYear++;
        }
        else if (set_clock_screen == 2) {
            if (settingDigit==0) {
                proposedMonth++;
            }
            if (settingDigit==2) {
                proposedDay++;
            }
        }
    }
    else if (direction == DOWN)
    {
        Serial.println("DOWN");
        if (set_clock_screen == 0) {
            if (settingDigit==0) {
                proposedHour--;
                if (proposedHour < 0) proposedHour = 0;
                if (proposedHour > 23) proposedHour = 23;
            }
            if (settingDigit==2) {
                proposedMinute--;
                if (proposedMinute < 0) proposedMinute = 59;
                if (proposedMinute > 59) proposedMinute = 0;
            }
        }
        else if (set_clock_screen == 1) {
            proposedYear--;
        }
        else if (set_clock_screen == 2) {
            if (settingDigit==0) {
                proposedMonth--;
            }
            if (settingDigit==2) {
                proposedDay--;
            }
        }
    }
    else if (direction == RIGHT)
    {
        Serial.println("RIGHT");
        if (set_clock_screen == 0) { 
            if (settingDigit == 2)
            {
                set_clock_screen = 1;
            }
            settingDigit = 2;
        }
        else if (set_clock_screen == 1) { 
            set_clock_screen = 2;
            settingDigit = 0;
        }
        else if (set_clock_screen == 2) {
            settingDigit = 2;
        }
    }
    else if (direction == LEFT)
    {
        Serial.println("LEFT");
        if (set_clock_screen == 0) { 
            if (settingDigit == 2)
            {
               settingDigit = 0;
            }
        }
        else if (set_clock_screen == 1) { 
            set_clock_screen = 0;
            settingDigit = 2;
        }
        else if (set_clock_screen == 2) {
            if (settingDigit == 2) {
                settingDigit = 0;
            }
            else {
                set_clock_screen = 1;
                settingDigit = 0;
            }
        }
    }

    proposedMonth = monthLogic(proposedMonth);
    proposedDay = dayLogic(proposedDay,proposedMonth,proposedYear);

    if (set_clock_screen == 0) { 
        proposed_digit[0] = (proposedHour/10) % 10;
        proposed_digit[1] = proposedHour % 10;
        proposed_digit[2] = (proposedMinute/10) % 10;
        proposed_digit[3] = proposedMinute % 10;
    }
    else if (set_clock_screen == 1) {
        proposed_digit[0] = (proposedYear/1000) % 10;
        proposed_digit[1] = (proposedYear/100) % 10;
        proposed_digit[2] = (proposedYear/10) % 10;
        proposed_digit[3] = proposedYear % 10;
    }
    else if (set_clock_screen == 2) {
        proposed_digit[0] = (proposedMonth/10) % 10;
        proposed_digit[1] = proposedMonth % 10;
        proposed_digit[2] = (proposedDay/10) % 10;
        proposed_digit[3] = proposedDay % 10;
    }
}

int dayLogic(int day, int month, int year) {
    int leapYear = !(year % 4);
    int max_days = 31;
    if (month == 2) {
        max_days = 28;
        if (leapYear == 1) {
            max_days = 29;
        }
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11) {
        max_days = 30;
    }

    if (day < 1) {
        day = max_days;
    }
    else if (day > max_days) {
        day = 1;
    }
    return day;
}

int monthLogic(int month) {
    if (month < 1) {
        return 12;
    }
    if (month > 12) {
        return 1;
    } 
    return month;
}