/**
 * Commodore LED Watch recreation for the TTGO Watch 2020 edition.
 * Written by Everlast Engineering
 * */
#include "config.h"
#include "timers.h"
#include "headers.h"
#include "powermgm.h"
#include "eventmgm.h"
#include "gui.h"
#include "qrcode.h"

TTGOClass *watch = nullptr;
PCF8563_Class *rtc;
BMA *sensor;




bool irq = false;
int digit[4] = {0,0,0,0};
int displayed_digit[4] = {0,0,0,0};

typedef enum {TIME_MODE} modes;
static modes mode = TIME_MODE;

screens screen = BLANK;
fades fade = NONE;

void setup()
{
    Serial.begin(115200);
    watch = TTGOClass::getWatch();
    watch->begin();
    watch->lvgl_begin();
    QRCode qrcode;
    uint8_t qrcodeBytes[qrcode_getBufferSize(6)];
    qrcode_initText(&qrcode, qrcodeBytes, 6, ECC_LOW, "HELLO WORLD");
    for (uint y = 0; y < qrcode.size; y++) {
        for (uint x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                Serial.print("██");
            } else {
                Serial.print("  ");
            }
        }
        Serial.print("\n");
    }
    sensor = watch->bma;
    rtc = watch->rtc;

    // Use compile time
    rtc->check();

    watch->openBL();

    //Lower the brightness
    watch->bl->adjust(10);

    guiSetup();

    

    

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
        // hiddenMode();
    }, MAINTHREADCYCLERATE, LV_TASK_PRIO_MID, nullptr);

    
}

void hiddenMode() {
    //c64 stuff
    Serial.printf("Hidden Mode!\n");
    lv_obj_t * screen_c64 = lv_obj_create(NULL, NULL);
    lv_obj_t *C64BootupScreen = lv_img_create(screen_c64, NULL);
    lv_img_set_src(C64BootupScreen, &BootupOn1702);
    lv_obj_align(C64BootupScreen, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_scr_load_anim(screen_c64, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 250, 0, false);
    lv_obj_set_event_cb(screen_c64, event_cb);
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