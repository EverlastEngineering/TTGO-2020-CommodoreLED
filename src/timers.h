#ifndef _TIMERS_H
    #define _TIMERS_H
	#define RETAPWINDOW 500 //milliseconds between multi tap event
	#define DIMMERWINDOW 10000 //milliseconds before dimming screen and setting CPU to slow
	#define CLOCKFACEWINDOW 3000 //milliseconds to show the clock if the clock is asked to show via a method other than screen tap (like tilt or button press)

	extern int retapTimer;
	extern int dimmerTimer;
	extern int clockFaceTimer;
	extern bool dimmerTimerActive;
	extern int retapCounter;

	void restartRetapTimer();
	void restartDimmerTimer();
	void restartClockFaceTimer();
	void processTimers();

#endif // _TIMERS_H