/*
 * menu.c
 *
 *  Created on: Aug 22, 2021
 *      Author: Mahdi
 */


#include "menu.h"
//Status_t OSC_Status;
Settings_t Settings;

void Settings_Init() {
	Settings.trigger.edge = rising_edge;
	Settings.trigger.mode = auto_mode;
	Settings.timebase.samp = timebase_x1;
	Settings.runmode = Running;
	Settings.timebase.offset = 0;
	Settings.timebase.div = 0;

	//OSC_Status.Running_Stat = Running;
}
