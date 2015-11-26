#ifndef INTERFACE_H
#define INTERFACE_H

#define LAMP1    PBout(8)
#define LAMP2    PBout(9) 

#include "lcd.h"
#include "touch.h"
#include "led.h"
#include "beep.h"
#include "adc.h"
#include "delay.h"

extern void (*current_handler)(u16, u16);
extern u8 music_on;
void set_handler(void (*handler)(u16, u16));
void draw_main_menu(void);
void draw_entertainment_menu(void);
void draw_appliance_menu(void);
void main_menu_handler(u16 x, u16 y);
void entertainment_menu_handler(u16 x, u16 y);
void appliance_menu_handler(u16 x, u16 y);
u8 file_explorer(char *path);
void file_explorer_handler(u16 x, u16 y);
void snake_game(void);
void music_handler(void);
void file_exploror_menu(void);
int audio_play(void);
int picture_display(void);

void Device_Init(void);
extern u8 music_on;
extern u8 is_web_music_player;
#endif