#include "config.h"
#include "timers.h"
#include "headers.h"
#include "powermgm.h"
#include "eventmgm.h"

int retapTimer = 0;
int dimmerTimer = 0;
int clockFaceTimer = 0;
bool dimmerTimerActive = true;
int retapCounter = 0;

void restartRetapTimer() {
    retapTimer = RETAPWINDOW;
}

void restartDimmerTimer() {
    // Serial.printf("dimmerTimer: %d\n",dimmerTimer);
    if (!dimmerTimer && powermode != FULLPOWER) {
        powerMode(FULLPOWER);
    }
    dimmerTimer = DIMMERWINDOW;
}

void restartClockFaceTimer() {
    // Serial.printf("clockFaceTimer: %d\n",clockFaceTimer);
    if (mode == AUTHENTIC_TIME_MODE) {
        clockFaceTimer = CLOCKFACEWINDOW;
        if (screen == BLANK) {
            screen = TIME;
        }
    }
}


void processTimers() {
    if (dimmerTimer > 0 && dimmerTimerActive) {
        Serial.printf("dimmerTimer: %d\n", dimmerTimer);
        dimmerTimer = dimmerTimer - MAINTHREADCYCLERATE;
        if (dimmerTimer <= 0) {
            dimmerTimer = 0;
            powerMode(targetpowermode);
        }
    }
    if (retapTimer > 0) {
        // Serial.printf("retapTimer: %d\n", dimmerTimer);
        retapTimer = retapTimer - MAINTHREADCYCLERATE;
        if (retapTimer <= 0) {
            // Serial.printf("retap reset\n");
            if (retapCounter == 4) 
            {
                debug = !debug;
            }
            else if (retapCounter == 5) 
            {
                screen = BATTERY;
                mode = BATTERY_MONITOR;
            }
            retapCounter = retapTimer = secret_mode = 0;
            resetDisplay();
        }
    }
    if (clockFaceTimer > 0) {
        clockFaceTimer = clockFaceTimer - MAINTHREADCYCLERATE;
        if (clockFaceTimer <= 0) {
            clockFaceTimer = 0;
            if (screen == TIME && mode == AUTHENTIC_TIME_MODE) {
                screen = BLANK;
            }
        }
    }
}