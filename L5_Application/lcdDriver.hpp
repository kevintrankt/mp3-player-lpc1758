#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include <stdio.h>
#include "io.hpp"
#include "LPC17xx.h"     // LPC_UART2_BASE
#include "sys_config.h"  // sys_get_cpu_clock()
#include <string.h>
#include <stdio.h>
#include "FreeRTOSConfig.h"
#include "tasks.hpp"
#include "utilities.h" //delay_ms
#include "io.hpp"
#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "lpc_isr.h"
#include "LabUart.hpp"
#include "lcdDriver.hpp"
#include <string.h>
#include "utilities.h" //delay_ms

//16 x 2 Parallax Serial LCD #27977 (Backlit with Piezospeaker)

class lcdDriver
{
	public:
		static int line; //0 or 1
		static int position; //0-15 <->
		//static int size; //size of filename to be displayed
		char lcd[2][25]; //25 for some longer file names

		lcdDriver(); //Constructor
		void initLCD(); //Initialization of LCD screen
		void clear(); //Clears screen and repositions cursor in upper-left corner
		void setCursor(int col, int row); //Re-positions cursor to set location
		void write(char data); //Writes char to the LCD screen
		void writeL1(char data); //writes to Line 1 of LCD screen
		void writeL2(char data); //writes to Line 2 of LCD screen
		void displayTimeElapsed(int elapsed);
		//void scrollDisplayLeft(); //scroll contents of display (text & cursor) continuously left
};

/* Sample LCD display
__________________
|songname.mp3    |
|     01:15      |
------------------
     min:secs
*/

#endif
