#ifndef _HEADERS_H
	#define _HEADERS_H
	
	typedef enum {TIME, DATE, SECONDS, BATTERY, BLANK, OFF, BATTERY_MONITOR_MEDIUM, IAMSUCHABOYCHILD, DASHES} screens; 
	extern screens screen;

	typedef enum {NONE, FADINGIN, FADINGOUT} fades;
	extern fades fade;

#endif //_HEADERS_H