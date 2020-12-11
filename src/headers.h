
#ifndef _HEADERS_H
	#define _HEADERS_H
	
	typedef enum {TIME, DATE, SECONDS, BATTERY, BLANK, OFF, IAMSUCHABOYCHILD, DASHES} screens; 
	extern screens screen;

	typedef enum {NONE, FADINGIN, FADINGOUT} fades;
	extern fades fade;

	typedef enum {AUTHENTIC_TIME_MODE, ALWAYS_ON_TIME_MODE, BATTERY_MONITOR, CYCLE_TIME_MODE, SECRET_MODE} modes;
	extern modes mode;

	#define MAINTHREADCYCLERATE 50 //milliseconds that the main thread refreshes after

	extern bool irq;
	extern int digit[];
	extern int displayed_digit[];

#endif //_HEADERS_H