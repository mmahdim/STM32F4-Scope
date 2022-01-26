/*
 * menu.h
 *
 *  Created on: Aug 22, 2021
 *      Author: Mahdi
 */

#ifndef APPLICATION_USER_MENU_H_
#define APPLICATION_USER_MENU_H_

#include "main.h"


typedef enum {
	rising_edge = 0,
	falling_edge
}Trigger_edge;

typedef enum {
	auto_mode = 0,
	normal_mode,
	single_mode
}Trigger_mode;

typedef struct {
  Trigger_edge edge;      // Trigger edge
  Trigger_mode mode;      // Mode (Normal, Auto, Single)
}Trigger_t;

typedef enum {
	timebase_x1 = 3150,		//7.2Msps, DIV2
	timebase_x2 = 3150*2,	//3.6Msps, DIV4
	timebase_x4 = 3150*4	//1.2Msps, DIV8
}Timebase_samp;

typedef struct {
	Timebase_samp samp;			// Timebase Hardware Samplerate
	int8_t div;				// Timebase Software Division (on the graph)
	int offset;					// Timebase Offset
}Timebase_t;

typedef struct {
  uint32_t mode;  // aktueller Mode (Off, CH1, Time)
  uint16_t p1;    // data Cursor-A (bei CH1 oder CH2)
  uint16_t p2;    // data Cursor-B (bei CH1 oder CH2)
  uint16_t t1;    // data Cursor-A (bei Time)
  uint16_t t2;    // data Cursor-B (bei Time)
}Cursor_t;

typedef enum {
	Running = 0,
	Stopped = 1
}Runmode_t;

typedef struct {
  Timebase_t timebase;		// Timebase
  Trigger_t trigger;		// Trigger
  Cursor_t cursor;			// Cursor
  Runmode_t runmode;		// running mode (Running/Stopped)
}Settings_t;



//typedef enum {
//	Running = 0,
//	Stopped = 1
//}run_stat;
//
//typedef struct {
//	run_stat Running_Stat;
//}Status_t;



void Settings_Init();

#endif /* APPLICATION_USER_MENU_H_ */
