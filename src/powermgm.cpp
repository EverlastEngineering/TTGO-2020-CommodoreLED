#include "powermgm.h"
#include "headers.h"
#include "config.h"

powermodes powermode = FULLPOWER;
powermodes targetpowermode = MEDIUMPOWER;
int blLevel = 10;

void powerMode(powermodes desiredPowerMode) {
    powermode = desiredPowerMode;
    switch (powermode) {
        case LOWPOWER:
			Serial.println("LOWPOWER");
            fade = FADINGOUT;
            // setCpuFrequencyMhz(10);
            break;
        case MEDIUMPOWER:
			Serial.println("MEDIUMPOWER");
            fade = FADINGOUT;
            // setCpuFrequencyMhz(10);
            break;
        case FULLPOWER:
			Serial.println("FULLPOWER");
            fade = FADINGIN;
            setCpuFrequencyMhz(40); 
            break;
    }
}