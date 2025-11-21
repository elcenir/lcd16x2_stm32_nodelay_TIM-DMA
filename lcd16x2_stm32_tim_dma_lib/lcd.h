#pragma once

#include "main.h"

//***** List of COMMANDS *****//
#define LCD_CLEARDISPLAY 		0x01
#define LCD_RETURNHOME 			0x02
#define LCD_ENTRYMODESET 		0x04
#define LCD_DISPLAYCONTROL   	0x08
#define LCD_CURSORSHIFT 		0x10
#define LCD_FUNCTIONSET 		0x20
#define LCD_SETCGRAMADDR 		0x40
#define LCD_SETDDRAMADDR 		0x80

//***** List of commands Bitfields *****//
//1) Entry mode Bitfields
#define LCD_ENTRY_SH		 				0x01
#define LCD_ENTRY_ID 						0x02
//2) Entry mode Bitfields
#define LCD_ENTRY_SH		 				0x01
#define LCD_ENTRY_ID 						0x02
//3) Display control
#define LCD_DISPLAY_B						0x01
#define LCD_DISPLAY_C						0x02
#define LCD_DISPLAY_D						0x04
//4) Shift control
#define LCD_SHIFT_RL						0x04
#define LCD_SHIFT_SC						0x08
//5) Function set control
#define LCD_FUNCTION_F					0x04
#define LCD_FUNCTION_N					0x08
#define LCD_FUNCTION_DL					0x10


void lcd_init(void);
void lcd_Set_cursor(uint8_t row,uint8_t col);
void lcd_print(uint8_t * string,uint8_t size);
void lcd_command(uint8_t command);



