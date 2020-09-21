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
