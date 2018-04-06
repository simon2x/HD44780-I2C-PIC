/*

File:   hd44780_i2c.c

Interface with the HD44780 16x02 LCD display via a i2c backpack which 
requires the display to operate in 4-bit mode
 
*/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)
    #include <stdint.h>         /* For uint8_t definition */
#endif

#include "hd44780_i2c.h"
#include "i2c.h"
#include "system.h"
#include "uart.h"

#define LCDADD 0x7E
#define LCDADDR LCDADD+1
#define RETURNHOME 0x02
#define CLEARDISPLAY 0x01

void lcd_send_instruction();

struct {
       unsigned int instruction;
       unsigned int backlight : 1;
       unsigned int readWrite : 1;
       unsigned int registerSelect : 1;   
       unsigned int cursorOn : 1;         
       unsigned int cursorBlink : 1;   
} LCDSTRUCT = {0x00,1,0,0,1,1};

static uint8_t get_control_nibble(uint8_t enable) 
{
        uint8_t c = 0x00;
        c |= LCDSTRUCT.backlight;
        c = c << 1;
        c |= enable;
        c = c << 1;
        c |= LCDSTRUCT.readWrite;
        c = c << 1;
        c |= LCDSTRUCT.registerSelect;
        return c;        
}

void lcd_backlight_off(void)
{
        LCDSTRUCT.backlight = 0;
}

void lcd_backlight_on(void)
{
        LCDSTRUCT.backlight = 1;
}

void lcd_backlight_toggle(void)
{
        LCDSTRUCT.backlight = ~LCDSTRUCT.backlight;
}

void lcd_clear_display()
{
        LCDSTRUCT.instruction = CLEARDISPLAY;
        lcd_send_instruction();
}

/* 
 * pg29 - Set DDRAM address sets the DDRAM address 
 * binary AAAAAAA into the address counter.

 * Data is then written to or read from the MPU for DDRAM.

 * For 2-line display:
 * First line: AAAAAAA can be 00H - 27H 
 * Second line: AAAAAAA can be 40H - 67H
*/             
void lcd_cursor(uint8_t row, uint8_t col) 
{    
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 0;
        LCDSTRUCT.instruction = 0x10;
        if (row == 1) {
                LCDSTRUCT.instruction |= 0x04; // 2nd line
        }    
        LCDSTRUCT.instruction &= 0x10;
        
        lcd_send_instruction();
        
        if (col == 0) {
                return;
        }
        lcd_cursor_shift_right(col);
}

void lcd_cursor_blink_off()
{    
        LCDSTRUCT.cursorBlink = 0;
}

void lcd_cursor_blink_on()
{    
        LCDSTRUCT.cursorBlink = 1;
}

void lcd_cursor_off()
{
        LCDSTRUCT.cursorOn = 0; 
}

void lcd_cursor_on()
{
        LCDSTRUCT.cursorOn = 1;
}

/*
 * Sets DDRAM address 0 in address counter. Also
 * returns display from being shifted to original
 * position. DDRAM contents remain unchanged.
 */
void lcd_cursor_return_home()
{
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 0;
        LCDSTRUCT.instruction = 0x02;
        lcd_send_instruction();
}

void lcd_cursor_shift_left(int n)
{
        // SC = 0, set cursor shift
        // RL = 0 (left), RL = 1 (right)
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 0;
        LCDSTRUCT.instruction = 0x10;        
        for (int i = 0; i < n; i++) {
            lcd_send_instruction();
        }    
}

void lcd_cursor_shift_right(int n)
{
        // SC = 0, set cursor shift
        // RL = 0 (left), RL = 1 (right)
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 0;
        LCDSTRUCT.instruction = 0x14;
        
        for (int i = 0; i < n; i++) {
            lcd_send_instruction();
        }    
}

void lcd_display_off(void)
{
        uint8_t c = 0x00;
        c |= LCDSTRUCT.cursorOn;
        c = c << 1;
        c |= LCDSTRUCT.cursorBlink;
        c |= 0x08;
        
        LCDSTRUCT.instruction = c;
        lcd_send_instruction();
}

void lcd_display_on(void)
{
        uint8_t c = 0x00;
        c |= LCDSTRUCT.cursorOn;
        c = c << 1;
        c |= LCDSTRUCT.cursorBlink;
        c |= 0x0C;
        
        LCDSTRUCT.instruction = c;
        lcd_send_instruction();
}

void lcd_display_shift_left(int n)
{
        // SC = 1, set display shift
        // RL = 0 (left), RL = 1 (right)
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 0;
        LCDSTRUCT.instruction = 0x18;
        for (int i = 0; i < n; i++) {
                lcd_send_instruction();
        }    
}

void lcd_display_shift_right(int n)
{
        // SC = 1, set display shift
        // RL = 0 (left), RL = 1 (right)
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 0;
        LCDSTRUCT.instruction = 0x1C;
        for (int i = 0; i < n; i++) {
                lcd_send_instruction();
        }    
}

/* 
 * refer to figure 24 in datasheet for instructions on 4bit initialisation
 * DL = 0 : 4 bits
 */
static void lcd_function_set_4bit()
{        
        lcd_wait();
        
        // 0 0 1 DL N F - -    
        // 0 0 , 0 0 1 DL=0, N=1 F=0 — —
        
        // we can't actually set N, F until in 4bit mode
        i2c_master_start();
        i2c_master_write(LCDADD);
        i2c_master_write(0x28);   // E = 0  
        i2c_master_write(0x2C);   // E = 1  
        i2c_master_write(0x28);   // E = 0
        i2c_master_stop();
        
        lcd_wait();
   
}   
 
void lcd_print(const char *mystring) 
{
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 1;

        uint8_t i = 0;
        while (mystring[i] != '\0') {            
                LCDSTRUCT.instruction = mystring[i];
                lcd_send_instruction();
                i++;         
        }
}   

/*
 * See page 33 of datasheet
 * In 4-bit operation, the high nibble is sent first, 
 * then the low nibble with corresponding LCDSTRUCT bits
 */
void lcd_send_instruction() 
{        
        lcd_wait();        
        i2c_master_start();
        i2c_master_write(LCDADD);
                
        uint8_t conEnabled = get_control_nibble(1);
        uint8_t conDisabled = get_control_nibble(0);
        uint8_t data;
        
        // Send high nibble of instruction first
        data = (LCDSTRUCT.instruction & 0xF0) | conEnabled; 
        i2c_master_write(data);
        i2c_master_write(conDisabled); 
        
        // Now send low nibble of instruction        
        data = ((LCDSTRUCT.instruction << 4) & 0xF0) | conEnabled; 
        i2c_master_write(data);
        i2c_master_write(conDisabled);        
                
        i2c_master_stop();
        lcd_wait();
}    

void lcd_set_lines(int lines) 
{     
        // once DL has been set, it cannot be changed
        // so we don't need to worry about DL value
        
        // N = 1, 2 lines
        // F = 0, 5x8 dots when two lines are needed
        LCDSTRUCT.readWrite = 0;
        LCDSTRUCT.registerSelect = 0;
        LCDSTRUCT.instruction = 0x20;
        if (lines == 2) {
                LCDSTRUCT.instruction |= 0x08; 
        }         
        lcd_send_instruction();
} 

void lcd_wait()
{    
        unsigned char i,j;
        for(i=0; i<50; i++) // A simple for loop for delay
                for(j=0; j<255; j++);
}

void lcd_init() 
{
        lcd_function_set_4bit();
        lcd_set_lines(2);    
        lcd_display_on();
        lcd_clear_display();
        lcd_backlight_on();
}    
