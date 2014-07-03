#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "usb_keyboard.h"
#include "hid.h"
#include "timer.h"
#include "auxiliary.h"
#include "gfx.h"
#include "time.h"
#include "buttons.h"

#define K_UP	1
#define K_LEFT	2
#define K_DOWN	3
#define K_RIGHT	4
#define K_PROG	5

const uint8_t PROGMEM robot[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x40, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x40, 0x04, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x00, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x40, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x04, 0x44, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x04, 0x44, 0x44, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x40, 0x04, 0x44, 0x04, 0x44, 0x44, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x44, 0x04, 0x44, 0x04, 0x44, 0x44, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x44, 0x00, 0x40, 0x44, 0x44, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x40, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x44, 0x44, 0x04, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x40, 0x44, 0x44, 0x44, 0x44, 0x40, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x04, 0x40, 0x44, 0x44, 0x04, 0x44, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x04, 0x40, 0x04, 0x40, 0x44, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x40, 0x44, 0x44, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x44, 0x44, 0x44, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x44, 0x44, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x44, 0x04, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x40, 0x04, 0x00, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x04, 0x44, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x40, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x44, 0x44, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x40, 0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* whole screen rectangle, for basic text drawing */
const rect_t screen_r = {0, 0, 24, 7};

#define MAX_LEN 10

uint8_t EEMEM ee_strings[4][MAX_LEN+1];
uint8_t EEMEM ee_modes[4][MAX_LEN+1];

int main(void)
{
	clock_prescale_set(clock_div_1);

	sei();
	USB_init();
	HID_commit_state();

	BUTTONS_init();

	/* initialize eeprom */
	for (int i = 0; i < 4; ++i) {
		const char c = eeprom_read_byte(&ee_strings[i][0]);
		eeprom_busy_wait();
		if (!('a' <= c && c <= 'z')) {
			eeprom_write_byte(&ee_strings[i][0], 'a');
			eeprom_write_byte(&ee_strings[i][1], 0);
			eeprom_write_byte(&ee_modes[i][0], 3);
			eeprom_busy_wait();
		}
	}

	//TIMER_init();

	GFX_init();
	char macro[MAX_LEN+1] = "a";
	uint8_t macro_modes[MAX_LEN+1] = {3, 3, 3, 3, 3, 3};
	int macro_len = 1;

	/* program edit mode */
	int prog_mode = 0;
	/* after holding "PROGRAM", waiting for key choice to reprogram */
	bool prog_mode_select = false;

	const uint8_t bright = 4;

	TIME_delay_ms(200);
	for (int i = 0; i < 41; ++i)
	{
		TIME_delay_ms(20);
		GFX_blit_progmem((rect_t){0, 0, 24, 7}, robot, 12, 0, i);
		GFX_swap();
	}
	TIME_delay_ms(150);

	while (true) {
		//Display home text
		if (prog_mode_select) {
			GFX_fill((rect_t){4, 0, 2, 1}, bright);
			GFX_fill((rect_t){18, 0, 2, 1}, bright);
			GFX_putpixel(3, 1, bright);
			GFX_putpixel(6, 1, bright);
			GFX_fill((rect_t){11, 1, 3, 1}, bright);
			GFX_putpixel(17, 1, bright);
			GFX_putpixel(20, 1, bright);
			GFX_putpixel(6, 2, bright);
			GFX_putpixel(11, 2, bright);
			GFX_putpixel(13, 2, bright);
			GFX_putpixel(20, 2, bright);
			GFX_fill((rect_t){4, 3, 2, 1}, bright);
			GFX_fill((rect_t){9, 3, 7, 1}, bright);
			GFX_fill((rect_t){18, 3, 2, 1}, bright);
			GFX_putpixel(4, 4, bright);
			GFX_putpixel(9, 4, bright);
			GFX_putpixel(11, 4, bright);
			GFX_putpixel(13, 4, bright);
			GFX_putpixel(15, 4, bright);
			GFX_putpixel(18, 4, bright);
			GFX_fill((rect_t){9, 5, 7, 1}, bright);
			GFX_putpixel(4, 6, bright);
			GFX_putpixel(18, 6, bright);
			GFX_swap();
		} else if (prog_mode == 0) {
			if (TIME_get() % 7000 < 3500) {
				GFX_fill((struct rect){0, 0, 6, 2}, 2);
				GFX_fill((struct rect){2, 2, 2, 5}, 2);
				GFX_fill((struct rect){7, 0, 5, 2}, 2);
				GFX_fill((struct rect){7, 2, 2, 5}, 2);
				GFX_fill((struct rect){9, 3, 2, 1}, 2);
				GFX_fill((struct rect){9, 5, 3, 2}, 2);
				GFX_fill((struct rect){13, 0, 4, 2}, 2);
				GFX_fill((struct rect){13, 2, 2, 3}, 2);
				GFX_fill((struct rect){13, 5, 4, 2}, 2);
				GFX_fill((struct rect){18, 0, 2, 7}, 2);
				GFX_fill((struct rect){20, 2, 2, 2}, 2);
				GFX_fill((struct rect){22, 0, 2, 7}, 2);
				GFX_swap();
				TIME_delay_ms(30);
			} else {
				GFX_fill((struct rect){0, 0, 2, 7}, 2);
				GFX_fill((struct rect){4, 0, 2, 1}, 2);
				GFX_fill((struct rect){3, 1, 3, 1}, 2);
				GFX_fill((struct rect){2, 2, 3, 1}, 2);
				GFX_fill((struct rect){2, 3, 2, 1}, 2);
				GFX_fill((struct rect){3, 4, 2, 1}, 2);
				GFX_fill((struct rect){4, 5, 2, 2}, 2);
				GFX_fill((struct rect){7, 0, 4, 2}, 2);
				GFX_fill((struct rect){7, 2, 2, 3}, 2);
				GFX_putpixel(9, 3, 2);
				GFX_fill((struct rect){7, 5, 4, 2}, 2);
				GFX_fill((struct rect){12, 0, 2, 2}, 2);
				GFX_fill((struct rect){16, 0, 2, 2}, 2);
				GFX_fill((struct rect){14, 1, 2, 1}, 2);
				GFX_fill((struct rect){13, 2, 4, 1}, 2);
				GFX_fill((struct rect){14, 3, 2, 4}, 2);
				GFX_fill((struct rect){20, 0, 3, 1}, 2);
				GFX_fill((struct rect){19, 1, 5, 1}, 2);
				GFX_fill((struct rect){19, 2, 2, 1}, 2);
				GFX_fill((struct rect){20, 3, 3, 1}, 2);
				GFX_fill((struct rect){22, 4, 2, 1}, 2);
				GFX_fill((struct rect){19, 5, 5, 1}, 2);
				GFX_fill((struct rect){20, 6, 3, 1}, 2);
				GFX_swap();
				TIME_delay_ms(30);
			}
		} else {
			if (macro_len <= 4) {
				// Display first 3 entry letters prior to scrolling
				if (TIME_get() % 300 < 220)
					GFX_put_text(screen_r, 0, 0, macro, macro_len, bright, 0);
				else
					GFX_put_text(screen_r, 0, 0, macro, macro_len-1, bright, 0);
				for (uint8_t i = 0; i < macro_len; ++i) {
					if (macro_modes[i] & 0x01) {
						GFX_putpixel(i*6+5, 6, 2);
						GFX_putpixel(i*6+5, 5, 1);
					}
					if (macro_modes[i] & 0x02) {
						GFX_putpixel(i*6+5, 0, 2);
						GFX_putpixel(i*6+5, 1, 1);
					}
				}
			} else {
				// Display text past 3 characters that is scrolled
				int position = (macro_len - 4) * -6;
				if (TIME_get() % 300 < 220)
					GFX_put_text(screen_r, position, 0, macro, macro_len, bright, 0);
				else
					GFX_put_text(screen_r, position, 0, macro, macro_len-1, bright, 0);
				for (uint8_t i = 0; i < 4; ++i) {
					if (macro_modes[macro_len - 4 + i] & 0x01) {
						GFX_putpixel(i*6+5, 6, 2);
						GFX_putpixel(i*6+5, 5, 1);
					}
					if (macro_modes[macro_len - 4 + i] & 0x02) {
						GFX_putpixel(i*6+5, 0, 2);
						GFX_putpixel(i*6+5, 1, 1);
					}
				}
			}
			GFX_swap();
		}

		//Poll Keys
		int clicked = 0;
		int held = 0;
		int released = 0;
		for (int i = 0; i < 5; ++i) {
			if (BUTTONS_has_been_clicked(i))
				clicked = i+1;
			if (BUTTONS_has_been_held(i))
				held = i+1;
			if (BUTTONS_has_been_released(i))
				released = i+1;
		}
		if (prog_mode > 0) {
			/* check key clicks */
			switch (clicked) {
			case K_UP:
				//THERE HAS TO BE A BETTER WAY...
				if (--macro[macro_len-1] < 'a')
					macro[macro_len-1] = 'a';
				TIME_delay_ms(150);
				break;
			case K_LEFT:
				//TODO SHORTEN TEMP_STRING BY 1
				if (macro_len > 1) {
					macro[macro_len - 1] = 0;
					--macro_len;
				}
				TIME_delay_ms(300);
				break;
			case K_DOWN:
				if (++macro[macro_len-1] > 'z')
					macro[macro_len-1] = 'z';
				TIME_delay_ms(150);
				break;
			case K_RIGHT:
				//ADD LETTER TO TEMP_STRING
				if (macro_len < MAX_LEN) {
					macro[macro_len] = 'a';
					macro_modes[macro_len] = 3;
					++macro_len;
					macro[macro_len] = 0;
				}
				TIME_delay_ms(150);
				break;
			default:
				break;
			}
			/* check key holds */
			if (held == K_PROG) {
				eeprom_write_block(macro, &ee_strings[prog_mode-1], MAX_LEN+1);
				eeprom_write_block(macro_modes, &ee_modes[prog_mode-1], MAX_LEN+1);
				eeprom_busy_wait();
				prog_mode = 0;
				TIME_delay_ms(300);
			} else if (released == K_PROG) {
				if (++macro_modes[macro_len - 1] > 3)
					macro_modes[macro_len - 1] = 1;
			}
		} else if (prog_mode_select) {
			if (1 <= clicked && clicked <= 4) {
				//UP, DOWN, LEFT, RIGHT ARROW
				prog_mode = clicked;
				prog_mode_select = false;
				/* initialize temp_strig */
				eeprom_read_block(macro, &ee_strings[clicked-1], MAX_LEN+1);
				eeprom_read_block(macro_modes, &ee_modes[clicked-1], MAX_LEN+1);
				eeprom_busy_wait();
				macro_len = strlen(macro);
				TIME_delay_ms(300);
			} else if (clicked == K_PROG) {
				prog_mode = 0;
				prog_mode_select = false;
				TIME_delay_ms(300);
			}
		} else { /* regular mode */
			if (held == K_PROG) {
				prog_mode_select = true;
				//Delay to avoid immediate escape from prog mode
				TIME_delay_ms(300);
			} else if (clicked == K_PROG) {
				//TODO
			} else if (clicked > 0) {
				eeprom_read_block(macro, &ee_strings[clicked-1], MAX_LEN+1);
				eeprom_read_block(macro_modes, &ee_modes[clicked-1], MAX_LEN+1);
				macro_len = strlen(macro);
				eeprom_busy_wait();
				for (int i = 0; i < macro_len; ++i) {
					if (macro_modes[i] & 0x01) {
						HID_set_scancode_state(KA + macro[i] - 'a', true);
						HID_commit_state();
						TIME_delay_ms(10);
					}
					if (macro_modes[i] & 0x02) {
						HID_set_scancode_state(KA + macro[i] - 'a', false);
						HID_commit_state();
						TIME_delay_ms(10);
					}
				}
				TIME_delay_ms(300);
			}
		}
	}
}


void MAIN_timer_handler()
{
}

void MAIN_handle_sof()
{
	TIME_update_1ms();
	BUTTONS_task();
}
