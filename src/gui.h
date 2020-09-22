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

	static str_datetime_t g_data;

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

	static const lv_img_dsc_t *number[] = {
		&Number0,
		&Number1,
		&Number2,
		&Number3,
		&Number4,
		&Number5,
		&Number6,
		&Number7,
		&Number8,
		&Number9,
		&Colon, //10
		&Dash, //11
    &Empty //12
	};
	
	void guiSetup();

#endif // _GUI_H

