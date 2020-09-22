#include "config.h"
#include "timers.h"
#include "headers.h"
#include "eventmgm.h"
#include "gui.h"

const lv_img_dsc_t *number[] = {
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

str_datetime_t g_data;

void guiSetup() {
	lv_obj_set_event_cb(lv_scr_act(), event_cb);

	lv_obj_t *img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &WatchFacePCB);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);

    g_data.minute2 = lv_img_create(lv_scr_act(), NULL);
   	lv_img_set_src(g_data.minute2, &Dash);
    lv_obj_align(g_data.minute2, NULL, LV_ALIGN_CENTER, 64, 3);

    g_data.minute1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.minute1, &Dash);
    lv_obj_align(g_data.minute1, NULL, LV_ALIGN_CENTER, 26, 3);

    g_data.colon = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.colon, &Colon);
    lv_obj_align(g_data.colon, NULL, LV_ALIGN_CENTER, 0, 3);

    g_data.hour2 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.hour2, &Dash);
    lv_obj_align(g_data.hour2, NULL, LV_ALIGN_CENTER, -29, 3);

    g_data.hour1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(g_data.hour1, &Dash);
    lv_obj_align(g_data.hour1, NULL, LV_ALIGN_CENTER, -67, 3);

    lv_img_set_src(g_data.hour1, &Empty);
    lv_img_set_src(g_data.hour2, &Empty);
    lv_img_set_src(g_data.minute1, &Empty);
    lv_img_set_src(g_data.minute2, &Empty);
    lv_img_set_src(g_data.colon, &Empty);
}