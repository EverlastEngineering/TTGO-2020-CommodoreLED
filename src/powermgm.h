#ifndef _POWERMGM_H
	#define _POWERMGM_H
	#define BLDIMMED 10
	#define BLBRIGHT 255

	typedef enum {MEDIUM, FULL} powermodes;
	extern powermodes powermode;
	extern int blLevel;

	void powerMode(powermodes desiredPowerMode);

#endif // _POWERMGM_H

