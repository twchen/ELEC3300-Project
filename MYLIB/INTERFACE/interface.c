#include "interface.h"
#include "ff.h"
#include "malloc.h"
#include "usart.h"
#include "string.h"
#include "key.h"
#include "wavplay.h"
#include "piclib.h"

u8 music_on = 0;
u8 is_web_music_player = 0;
void (*current_handler)(u16, u16);

int ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void get_parent_path(char *parent, char *curr_path)
{
	char *pch = strrchr(curr_path, '/');
	if(pch == 0){
		parent[0] = 0;
		return;
	}
	strncpy(parent, curr_path, pch - curr_path);
	parent[pch - curr_path] = 0;
}

void set_handler(void (*handler)(u16, u16))
{
	current_handler = handler;
}

void draw_main_menu()
{
	LCD_Clear(WHITE);
	ai_load_picfile("0:/systems/main.jpg", 0, 0, 480, 800, 1);
}

void draw_appliance_menu(){
	LCD_Clear(WHITE);
	ai_load_picfile("0:/systems/home.jpg", 0, 0, 480, 800, 1);
}

void draw_entertainment_menu(void)
{
	LCD_Clear(WHITE);
	ai_load_picfile("0:/systems/entertainment.jpg", 0, 0, 480, 800, 1);
	delay_ms(1000);
}

char prev_path[_MAX_LFN + 1] = "";
char curr_path[_MAX_LFN + 1];

u8 file_explorer(char *path)
{
	FRESULT res;
	FILINFO f_info;
	DIR dir;
	char *fn;
	u16 table_size = 0;
	u16 pointing_index = 0;
	u16 temp; // temp index
	u16 index_table[64];
	u8 i = 0;
	LCD_Clear(WHITE);
	res = f_opendir(&dir, path);
	f_info.lfsize = _MAX_LFN * 2 + 1;
	f_info.lfname = mymalloc(SRAMIN, f_info.lfsize);
	if(res == FR_OK && f_info.lfname != NULL)
	{
		u16 cur_index = 0;
		while(1){
			temp = dir.index;
			res = f_readdir(&dir, &f_info);
			if(res != FR_OK || f_info.fname[0] == 0){
				break;
			}
			fn = *f_info.lfname ? f_info.lfname : f_info.fname;
			index_table[cur_index++] = temp;
			if(f_info.fattrib & AM_DIR){
				POINT_COLOR = BLUE;
				LCD_ShowString(30, i++ * 30, 250, 24, 24, fn);
				POINT_COLOR = BLACK;
			}
			else{
				LCD_ShowString(30, i++ * 30, 250, 24, 24, fn);
			}
		}
		table_size = cur_index;
	}
	LCD_Fill(5, pointing_index * 30 + 10, 15, pointing_index * 30 + 20, RED);
	while(1){
		u8 key = KEY_Scan(0);
		if(key == KEY0_PRES){
			if(pointing_index < table_size - 1){
				LCD_Fill(5, pointing_index * 30 + 10, 15, pointing_index * 30 + 20, WHITE);
				pointing_index++;
				LCD_Fill(5, pointing_index * 30 + 10, 15, pointing_index * 30 + 20, RED);
			}
		}
		else if(key == KEY2_PRES){
			if(pointing_index != 0){
				LCD_Fill(5, pointing_index * 30 + 10, 15, pointing_index * 30 + 20, WHITE);
				--pointing_index;
				LCD_Fill(5, pointing_index * 30 + 10, 15, pointing_index * 30 + 20, RED);
			}
		}
		else if(key == WKUP_PRES){
			get_parent_path(prev_path, path);
			if(prev_path[0] == 0){
				/*
				draw_entertainment_menu();
				set_handler(entertainment_menu_handler);
				*/
				draw_main_menu();
				set_handler(main_menu_handler);
				break;
			}
			else{
				strcpy(curr_path, prev_path);
				myfree(SRAMIN, f_info.lfname);
				return 1;
			}
		}
		else if(key == KEY1_PRES){
			//open file
			dir_sdi(&dir, index_table[pointing_index]);
			res = f_readdir(&dir, &f_info);
			if(res == FR_OK && f_info.fname[0] != 0){
				fn = *f_info.lfname ? f_info.lfname : f_info.fname;
				printf("open file: %s\n", fn);
				if(f_info.fattrib & AM_DIR){
					sprintf(curr_path, "%s/%s", path, fn);
					myfree(SRAMIN, f_info.lfname);
					return 1;
				}
				else if(ends_with(fn, ".wav")){
					//play music
					sprintf(curr_path, "%s/%s", path, fn);
					printf("wav file: %s\n", curr_path);
					wav_play_song(curr_path);
				}
				else if(ends_with(fn, ".bmp") || ends_with(fn, ".jpg")){
					// pictures
					
				}
			}
		}
	}
	myfree(SRAMIN, f_info.lfname);
	return 0;
}

void main_menu_handler(u16 x, u16 y)
{
	if(y > 200 && y < 380){
		draw_appliance_menu();
		set_handler(appliance_menu_handler);
	}
	else if(y > 380 && y < 600){
		draw_entertainment_menu();
		set_handler(entertainment_menu_handler);
	}
}

void appliance_menu_handler(u16 x, u16 y)
{
	short temp;
	if(y < 100){
	}
	else if(y < 200){
		if(x < 80){}
		else if(x < 230)
			LED0 = 0;
		else
			LED0 = 1;
	}
	else if(y < 300){
		if(x < 80){}
		else if(x < 230)
			LED1 = 0;
		else
			LED1 = 1;
	}
	else if(y < 400){
		if(x < 80){}
		else if(x < 230)
			BEEP = 1;
		else
			BEEP = 0;
	}
	else if(y < 500){
		temp = Get_Temperature();
		LCD_ShowxNum(325, 440, temp/100, 2, 24, 0);
		//LCD_ShowxNum(80+11*16, 420, temp%100, 2, 24, 0);
	}
	else if(y < 600){
		draw_main_menu();
		set_handler(main_menu_handler);
	}
}

void entertainment_menu_handler(u16 x, u16 y)
{
	if(y < 100){
	}
	else if(y < 200){
		u8 res = file_explorer("0:");
		/*
		if(res == 0){
			tp_dev.scan(0);
			tp_dev.scan(0);
			delay_ms(200);
		}
		*/
		while(res){
			printf("result not 0\n");
			res = file_explorer(curr_path);
		}
	}
	else if(y < 320){
		LCD_Clear(WHITE);
		ai_load_picfile("0:/systems/music.jpg", 0, 0, 480, 800, 1);
		audio_play();
		draw_entertainment_menu();
		set_handler(entertainment_menu_handler);
	}
	else if(y < 430){
		picture_display();
		draw_entertainment_menu();
		set_handler(entertainment_menu_handler);
	}
	else if(y < 550){
		// enter snake game;
		snake_game();
		// finished playing game
		draw_entertainment_menu();
	}
	else if(y < 660){
		draw_main_menu();
		set_handler(main_menu_handler);
	}
}

void music_handler(void)
{
	printf("hello\n");
	if(music_on){
		LCD_Clear(WHITE);
		ai_load_picfile("0:/systems/music.jpg", 0, 0, 480, 800, 1);
		audio_play();
		draw_entertainment_menu();
		set_handler(entertainment_menu_handler);
	}
}
