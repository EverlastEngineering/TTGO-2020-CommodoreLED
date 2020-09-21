#ifndef _TIMERS_H
    #define _TIMERS_H
	#define RETAPWINDOW 500 //milliseconds between multi tap event
	#define DIMMERWINDOW 10000 //milliseconds before dimming screen and setting CPU to slow

	extern int retapTimer;
	extern int dimmerTimer;
	extern bool dimmerTimerActive;
	extern int retapCounter;

	void restartRetapTimer();
	void restartDimmerTimer();
	void processTimers();

#endif // _TIMERS_H