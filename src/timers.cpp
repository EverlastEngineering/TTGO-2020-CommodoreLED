#include "config.h"
#include "timers.h"
#include "headers.h"
#include "powermgm.h"

int retapTimer = 0;
int dimmerTimer = 0;
bool dimmerTimerActive = true;
int retapCounter = 0;

void restartRetapTimer() {
    retapTimer = RETAPWINDOW;
}

void restartDimmerTimer() {
    // Serial.printf("dimmerTimer: %d\n",dimmerTimer);
    if (!dimmerTimer) {
        powerMode(FULL);
        screen = BLANK;
    }
    dimmerTimer = DIMMERWINDOW;
}


void processTimers() {
    if (dimmerTimer > 0 && dimmerTimerActive) {
        Serial.printf("dimmerTimer: %d\n", dimmerTimer);
        dimmerTimer = dimmerTimer - MAINTHREADCYCLERATE;
        if (dimmerTimer <= 0) {
            dimmerTimer = 0;
            powerMode(MEDIUM);
        }
    }
    if (retapTimer > 0) {
        Serial.printf("retapTimer: %d\n", dimmerTimer);
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