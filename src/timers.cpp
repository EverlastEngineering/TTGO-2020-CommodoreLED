#include "config.h"
#include "timers.h"
#include "headers.h"
#include "powermgm.h"
#include "eventmgm.h"

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
        powerMode(FULLPOWER);
    }
    dimmerTimer = DIMMERWINDOW;
}


void processTimers() {
    if (dimmerTimer > 0 && dimmerTimerActive) {
        // Serial.printf("dimmerTimer: %d\n", dimmerTimer);
        dimmerTimer = dimmerTimer - MAINTHREADCYCLERATE;
        if (dimmerTimer <= 0) {
            dimmerTimer = 0;
            powerMode(MEDIUMPOWER);
        }
    }
    if (retapTimer > 0) {
        // Serial.printf("retapTimer: %d\n", dimmerTimer);
        retapTimer = retapTimer - MAINTHREADCYCLERATE;
        if (retapTimer <= 0) {
            // Serial.printf("retap reset\n");
            retapCounter = retapTimer = secret_mode = 0;
        }
    }
}