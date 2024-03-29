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
#include <WiFi.h>


// to do: on wakeup, don't show the last time until its been updated. perhaps just blank for one cycle?
// more power tweaks!
// https://diyprojects.io/t-watch-sleep-and-wake-up-with-bma423-accelerometer-or-esp32-button/#.YTbLCZ5KjG8

TTGOClass *watch = nullptr;



bool irq = false;
int digit[4] = {0,0,0,0};
int displayed_digit[4] = {255,255,255,255};

bool debug = false;

modes mode = AUTHENTIC_TIME_MODE;
screens screen = BLANK;
fades fade = FADINGIN;

void setup()
{
    setCpuFrequencyMhz(40); 
    Serial.begin(115200);
    WiFi.mode(WIFI_OFF);

    watch = TTGOClass::getWatch();
    watch->begin();
    watch->lvgl_begin();
    watch->motor_begin();
    // QRCode qrcode;
    // uint8_t qrcodeBytes[qrcode_getBufferSize(6)];
    // qrcode_initText(&qrcode, qrcodeBytes, 6, ECC_LOW, "HELLO WORLD");
    // for (uint y = 0; y < qrcode.size; y++) {
    //     for (uint x = 0; x < qrcode.size; x++) {
    //         if (qrcode_getModule(&qrcode, x, y)) {
    //             Serial.print("██");
    //         } else {
    //             Serial.print("  ");
    //         }
    //     }
    //     Serial.print("\n");
    // }

    // Use compile time
    watch->rtc->check();

    // watch->rtc->setDateTime(2020, 12, 11, 0, 22, 00);
    watch->openBL();

    //Lower the brightness
    watch->bl->adjust(10);

    clockMode();

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
    watch->bma->accelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    watch->bma->enableAccel();

    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, [] {
        // Set interrupt to set irq value to 1
        irq = 1;
    }, RISING); //It must be a rising edge

    // Enable BMA423 isStepCounter feature
    // sensor->enableFeature(BMA423_STEP_CNTR, true);
    // Enable BMA423 isTilt feature
    watch->bma->enableFeature(BMA423_TILT, true);
    // Enable BMA423 isDoubleClick feature
    // sensor->enableFeature(BMA423_WAKEUP, true);

    // Reset steps
    // sensor->resetStepCounter();

    // Turn on feature interrupt
    // sensor->enableStepCountInterrupt();
    watch->bma->enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    // sensor->enableWakeupInterrupt();
    
    lv_task_create([](lv_task_t *t2) {
        if (irq) {
            irq = 0;
            bool  rlst;
            do {
                // Read the BMA423 interrupt status,
                // need to wait for it to return to true before continuing
                rlst =  watch->bma->readInterrupt();
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
            if (watch->bma->isTilt()) {
                if (powermode == LOWPOWER) {
                    restartDimmerTimer();
                    restartClockFaceTimer();
                    Serial.println("isTilt");
                }
                // tft->setTextColor(random(0xFFFF), TFT_BLACK);
                // tft->setCursor(xoffset, 160);
                // tft->print("isTilt:");
                // tft->print(++tiltCount);
            }
            // Double-click interrupt
            // if (sensor->isDoubleClick()) {
            // }
        }
        watch->power->readIRQ();
        if (watch->power->isPEKShortPressIRQ())
        {
            watch->power->clearIRQ();
            Serial.println("Side Button Press");
            if (powermode == FULLPOWER || powermode == MEDIUMPOWER) {
                powerMode(LOWPOWER);
                targetpowermode = LOWPOWER;
                screen = TIME; // put the screen back to TIME to 'reset' the watch if you get it all effed up lol
            }
            else if (powermode == LOWPOWER) {
                // watch->openBL();     
                // watch->displayWakeup();
                // watch->startLvglTick();
                powerMode(FULLPOWER);
                targetpowermode = MEDIUMPOWER;
                restartDimmerTimer();
                restartClockFaceTimer();
            }
        }
        else if (watch->power->isPEKLongtPressIRQ()) {
            watch->power->clearIRQ();
            Serial.println("Side Button Long Press");
        }
        watch->power->clearIRQ();
        
        
        if (debug && screen == SETCLOCK) {
            // Serial.printf("debugging setclock\n");
           
        }
        else if (debug) {
            Serial.printf("isChargeing: %d\n", watch->power->isChargeing());
            Serial.printf("isBatteryConnect: %d\n", watch->power->isBatteryConnect());
            Serial.printf("isChargeingEnable: %d\n", watch->power->isChargeingEnable());
            Serial.printf("getBattVoltage: %f\n", round(watch->power->getBattVoltage())/1000);
            Serial.printf("getBattDischargeCurrent: %f\n", watch->power->getBattDischargeCurrent());
            char buf[128];
            watch->tft->setTextColor(0x444, TFT_BLACK);
            snprintf(buf, sizeof(buf), "Battery: %.2f", round(watch->power->getBattVoltage())/1000);
            watch->tft->drawString(buf, 10, 10, 2);
            snprintf(buf, sizeof(buf), "Battery %%: %.0f", round(watch->power->getBattPercentage()));
            watch->tft->drawString(buf, 10, 30, 2);
        }
        
    }, 250, LV_TASK_PRIO_MID, nullptr);
    
    lv_task_create([](lv_task_t *t) {
        // Serial.println("lv_task@50ms");
        if (powermode == LOWPOWER && fade == FADINGOUT) {
            blLevel = blLevel - 63; 
            if (blLevel <= 0) {
                fade = NONE;
                blLevel = retapCounter = retapTimer = dimmerTimer = 0;
                // watch->closeBL();        // switch off backlight
                // watch->displaySleep();   // switch off screen 
                // watch->stopLvglTick();   // Pause LVGL handler
                // watch->powerOff();
                setCpuFrequencyMhz(10);
                gpio_wakeup_enable ((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);  
                gpio_wakeup_enable ((gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL);  
                esp_sleep_enable_gpio_wakeup();
                esp_light_sleep_start();
                // watch->powerOff();
                // esp_deep_sleep_start();
                // esp_light_sleep_start();
            }
            watch->bl->adjust(blLevel);
        }
        else if (fade == FADINGOUT && blLevel != BLDIMMED) {
            blLevel = blLevel - 10;
            if (blLevel <= BLDIMMED) {
                blLevel = BLDIMMED;
                fade = NONE;
                setCpuFrequencyMhz(10);
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

        if (powermode == LOWPOWER || (powermode != FULLPOWER && mode != BATTERY_MONITOR && mode != ALWAYS_ON_TIME_MODE)) {
            return;
        }
        
        processDisplay();
    }, MAINTHREADCYCLERATE, LV_TASK_PRIO_MID, nullptr);

    restartDimmerTimer();
    restartClockFaceTimer();
}



void loop()
{
    lv_task_handler();
}