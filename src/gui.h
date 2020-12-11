#ifndef _GUI_H
	#define _GUI_H
	#include "config.h"

	
	typedef struct {
		lv_obj_t *hour1;
		lv_obj_t *hour2;
		lv_obj_t *minute1;
		lv_obj_t *minute2;
		lv_obj_t *colon;
	} str_datetime_t;

	extern str_datetime_t g_data;

	LV_IMG_DECLARE(WatchFacePCB);
	LV_IMG_DECLARE(Number0);
	LV_IMG_DECLARE(Number1);
	LV_IMG_DECLARE(Number2);
	LV_IMG_DECLARE(Number3);
	LV_IMG_DECLARE(Number4);
	LV_IMG_DECLARE(Number5);
	LV_IMG_DECLARE(Number6);
	LV_IMG_DECLARE(Number7);
	LV_IMG_DECLARE(Number8);
	LV_IMG_DECLARE(Number9);
	LV_IMG_DECLARE(Colon);
	LV_IMG_DECLARE(Empty);
	LV_IMG_DECLARE(Dash);

	//hidden mode
	LV_IMG_DECLARE(BootupOn1702);

	extern const lv_img_dsc_t *number[];
	
	void guiSetup(lv_obj_t * _screen) ;
	void hiddenMode();
	void clockMode(int animationTime);
	void clockMode();
	void displayNumerals();
	void processDisplay();

#endif // _GUI_H

