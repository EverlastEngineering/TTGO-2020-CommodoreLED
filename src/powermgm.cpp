#include "powermgm.h"
#include "headers.h"
#include "config.h"

powermodes powermode = FULL;
int blLevel = 10;

void powerMode(powermodes desiredPowerMode) {
    powermode = desiredPowerMode;
    switch (powermode) {
        case MEDIUM:
			Serial.printf("here");
            fade = FADINGOUT;
            setCpuFrequencyMhz(10);
            break;
        case FULL:
			Serial.printf("here2");
            fade = FADINGIN;
            setCpuFrequencyMhz(40);
            break;
    }
}