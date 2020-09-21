/**
 * Commodore LED Watch recreation for the TTGO Watch 2020 edition.
 * Written by Everlast Engineering
 * */
#include "config.h"

typedef struct {
    lv_obj_t *hour1;
    lv_obj_t *hour2;
    lv_obj_t *minute1;
    lv_obj_t *minute2;
    lv_obj_t *colon;
} str_datetime_t;

static str_datetime_t g_data;

TTGOClass *watch = nullptr;
PCF8563_Class *rtc;
BMA *sensor;

LV_IMG_DECLARE(WatchFacePCB);
LV_IMG_DECLARE(BootupOn1702);
LV_IMG_DECLARE(Number0);
LV_IMG_DECLARE(Number1);
LV_IMG_DECLARE(Number2);
LV_IMG_DECLARE(Number3);
LV_IMG_DECLARE(Number4);
LV_IMG_DECLARE(Number5);
LV_IMG_DECLARE(Number6);
LV_IMG_DECLARE(Number7);
LV_IMG_DECLARE(Number8);
LV_IMG_DECLARE(Number9);
LV_IMG_DECLARE(Colon);
LV_IMG_DECLARE(Empty);
LV_IMG_DECLARE(Dash);

static const lv_img_dsc_t *number[] = {
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

#define SHOWBATTERYTIMER 0
#define DIMMERWINDOW 10000 //milliseconds before dimming screen and setting CPU to slow
#define RETAPWINDOW 500 //milliseconds between multi tap event
#define MAINTHREADCYCLERATE 50 //milliseconds that the main thread refreshes after
bool irq = false;
int digit[4] = {0,0,0,0};
int displayed_digit[4] = {0,0,0,0};

static int dimmerTimer = 0;
static bool dimmerTimerActive = true;
static int retapTimer = 0;
static int retapCounter = 0;

static int blLevel = 255;

#define BLDIMMED 10
#define BLBRIGHT 255

typedef enum {NONE, FADINGIN, FADINGOUT} fades;
static fades fade = NONE;

typedef enum {MEDIUM, FULL} powermodes;
static powermodes powermode = FULL;

typedef enum {TIME, DATE, SECONDS, BATTERY, BLANK, OFF, BATTERY_MONITOR_MEDIUM, IAMSUCHABOYCHILD, DASHES} screens; 
static screens screen = BLANK;

typedef enum {TIME_MODE} modes;
static modes mode = TIME_MODE;


static int ctr_pressing = 0;
static int ctr_pressed_repeat = 0;

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
void setup()
{
    Serial.begin(115200);
    watch = TTGOClass::getWatch();
    watch->begin();
    watch->lvgl_begin();
    sensor = watch->bma;
    rtc = watch->rtc;

    // Use compile time
    rtc->check();

    watch->openBL();

    //Lower the brightness
    watch->bl->adjust(10);

    lv_obj_set_event_cb(lv_scr_act(), event_cb);

    lv_obj_t *img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &WatchFacePCB);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);

    g_data.minute2 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.minute2, &Dash);
    lv_obj_align(g_data.minute2, NULL, LV_ALIGN_CENTER, 64, 3);

    g_data.minute1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.minute1, &Dash);
    lv_obj_align(g_data.minute1, NULL, LV_ALIGN_CENTER, 26, 3);

    g_data.colon = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.colon, &Colon);
    lv_obj_align(g_data.colon, NULL, LV_ALIGN_CENTER, 0, 3);

    g_data.hour2 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.hour2, &Dash);
    lv_obj_align(g_data.hour2, NULL, LV_ALIGN_CENTER, -29, 3);

    g_data.hour1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.hour1, &Dash);
    lv_obj_align(g_data.hour1, NULL, LV_ALIGN_CENTER, -67, 3);

    lv_img_set_src(g_data.hour1, &Empty);
    lv_img_set_src(g_data.hour2, &Empty);
    lv_img_set_src(g_data.minute1, &Empty);
    lv_img_set_src(g_data.minute2, &Empty);
    lv_img_set_src(g_data.colon, &Empty);

    sensor = watch->bma;
     // Accel parameter structure
    Acfg cfg;
    /*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    cfg.range = BMA4_ACCEL_RANGE_2G;
    /*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    cfg.perf_mode = BMA4_CONTINUOUS_MODE; //BMA4_CIC_AVG_MODE?

    // Configure the BMA423 accelerometer
    sensor->accelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    sensor->enableAccel();

    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, [] {
        // Set interrupt to set irq value to 1
        irq = 1;
    }, RISING); //It must be a rising edge

    // Enable BMA423 isStepCounter feature
    // sensor->enableFeature(BMA423_STEP_CNTR, true);
    // Enable BMA423 isTilt feature
    sensor->enableFeature(BMA423_TILT, true);
    // Enable BMA423 isDoubleClick feature
    // sensor->enableFeature(BMA423_WAKEUP, true);

    // Reset steps
    // sensor->resetStepCounter();

    // Turn on feature interrupt
    // sensor->enableStepCountInterrupt();
    sensor->enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    // sensor->enableWakeupInterrupt();
    
    lv_task_create([](lv_task_t *t2) {
        if (irq) {
            irq = 0;
            bool  rlst;
            do {
                // Read the BMA423 interrupt status,
                // need to wait for it to return to true before continuing
                rlst =  sensor->readInterrupt();
            } while (!rlst);

            // Check if it is a step interrupt
            // if (sensor->isStepCounter()) {
            //     // Get step data from register
            //     // stepCount = sensor->getCounter();
            //     // tft->setTextColor(random(0xFFFF), TFT_BLACK);
            //     // tft->setCursor(xoffset, 118);
            //     // tft->print("StepCount:");
            //     // tft->print(stepCount);
            // }
            // The wrist must be worn correctly, otherwise the data will not come out
            if (sensor->isTilt()) {
                restartDimmerTimer();
                Serial.println("isTilt");
                // tft->setTextColor(random(0xFFFF), TFT_BLACK);
                // tft->setCursor(xoffset, 160);
                // tft->print("isTilt:");
                // tft->print(++tiltCount);
            }
            // Double-click interrupt
            // if (sensor->isDoubleClick()) {
            // }
        }
        
    }, 250, LV_TASK_PRIO_MID, nullptr);
    
    dimmerTimer = 1;
    
    lv_task_create([](lv_task_t *t) {
        if (fade == FADINGOUT && blLevel != BLDIMMED) {
            blLevel = blLevel - 10;
            if (blLevel <= BLDIMMED) {
                blLevel = BLDIMMED;
                fade = NONE;
            }
            watch->bl->adjust(blLevel);
        }
        else if (fade == FADINGIN && blLevel != BLBRIGHT) {
            blLevel = blLevel + 40;
            if (blLevel >= BLBRIGHT) {
                blLevel = BLBRIGHT;
                fade = NONE;
            }
            watch->bl->adjust(blLevel);
        }

        if (powermode != FULL && screen != BATTERY_MONITOR_MEDIUM) {
            return;
        }
        
        RTC_Date curr_datetime = rtc->getDateTime();
        int colonVisible = 0;
 
        if (screen == TIME) {
            digit[0] =  (curr_datetime.hour/10) % 10;
            digit[1] =  curr_datetime.hour % 10;
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
        else if (screen == BATTERY || BATTERY_MONITOR_MEDIUM) {
            int level = watch->power->getBattPercentage();
            // Serial.printf("Battery: %d%%\n", level);
            digit[0] = 8;
            digit[1] = (level/100) % 10;
            if (digit[1] == 0) {
                digit[1] = 11;
            }
            digit[2] = (level/10) % 10;
            digit[3] = (level/1) % 10;
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
    }, MAINTHREADCYCLERATE, LV_TASK_PRIO_MID, nullptr);
}

void restartDimmerTimer() {
    if (!dimmerTimer) {
        powerMode(FULL);
        screen = BLANK;
    }
    dimmerTimer = DIMMERWINDOW;
}

void restartRetapTimer() {
    retapTimer = RETAPWINDOW;
}

void processTimers() {
    if (dimmerTimer > 0 && dimmerTimerActive) {
        // Serial.printf("dimmer: %d\n", dimmerTimer);
        dimmerTimer = dimmerTimer - MAINTHREADCYCLERATE;
        if (dimmerTimer <= 0) {
            dimmerTimer = 0;
            powerMode(MEDIUM);
        }
    }
    if (retapTimer > 0) {
        retapTimer = retapTimer - MAINTHREADCYCLERATE;
        if (retapTimer <= 0) {
            Serial.printf("retap reset\n");
            retapCounter = retapTimer = 0;
            if (screen != BATTERY_MONITOR_MEDIUM) {
                screen = BLANK;
            }
        }
    }
}

void powerMode(powermodes desiredPowerMode) {
    powermode = desiredPowerMode;
    switch (powermode) {
        case MEDIUM:
            fade = FADINGOUT;
            setCpuFrequencyMhz(10);
            break;
        case FULL:
            fade = FADINGIN;
            setCpuFrequencyMhz(40);
            break;
    }
}

void displayNumerals() {
    if (displayed_digit[0] != digit[0]) {
        if (digit[0]==0) {
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

void loop()
{
    lv_task_handler();
}