 
# HD44780 LCD display with I2C backpack for PIC micros

Refer to the official data sheet for help

- https://www.sparkfun.com/datasheets/LCD/HD44780.pdf


# Notes

- Does not support reading the busy flag and reading in general (yet)
- Therefore wait is implemented by a for loop
- Supports only the 4-bit mode i.e. with I2C backpack
- Default to show cursor, blink cursor and backlight on
- The user must substitute/include their own i2c.c functions. See i2c.h for prototypes
- The user must set the correct oscillator speed _XTAL_FREQ in system.h


# Simple Example
```

    int main(void) 
    {
            i2c_master_init(500000);
            lcd_init();
                    
            lcd_cursor(0, 0);
            lcd_print("Line 1: Test!");
            
            lcd_cursor(1, 0);
            lcd_print("Line 2: Test!");
            
            return 0; 
    }
```

# Functions/Usage

```
    void lcd_backlight_off();

    void lcd_backlight_on();

    void lcd_backlight_toggle();

    void lcd_clear_display();

    void lcd_cursor(uint8_t row, uint8_t col); 

    void lcd_cursor_blink_off();

    void lcd_cursor_blink_on();

    void lcd_cursor_off();

    void lcd_cursor_on();

    void lcd_cursor_shift_left(int n);

    void lcd_cursor_shift_right(int n);

    void lcd_cursor_return_home();

    void lcd_display_shift_left(int n);

    void lcd_display_shift_right(int n);

    void lcd_display_off(void);

    void lcd_display_on(void);

    void lcd_print(const char * mystring);

    void lcd_wait();

    void lcd_init();

```