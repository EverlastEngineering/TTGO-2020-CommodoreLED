#ifndef _EVENTMGM_H
	#define _EVENTMGM_H

	#include "config.h"
	void event_cb( lv_obj_t * obj, lv_event_t event );
	void detectSecretMode(lv_gesture_dir_t dir);

	extern int secret_mode;

#endif // _EVENTMGM_H

