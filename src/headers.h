
#ifndef _HEADERS_H
	#define _HEADERS_H
	
	typedef enum {TIME, DATE, SECONDS, BATTERY, BLANK, OFF, IAMSUCHABOYCHILD, DASHES, D3B9, B477, SETCLOCK, SET537} screens; 

	extern screens screen;

	typedef enum {NONE, FADINGIN, FADINGOUT} fades;
	extern fades fade;

	typedef enum {AUTHENTIC_TIME_MODE, ALWAYS_ON_TIME_MODE, BATTERY_MONITOR, CYCLE_TIME_MODE, SECRET_MODE} modes;
	extern modes mode;

	typedef enum {UP, DOWN, RIGHT, LEFT} directions;
	extern directions direction;

	#define MAINTHREADCYCLERATE 50 //milliseconds that the main thread refreshes after

	extern bool irq;
	extern int digit[];
	extern int displayed_digit[];
	extern bool debug;

#endif //_HEADERS_H