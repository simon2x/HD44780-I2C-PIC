 
# HD44780 LCD display with I2C backpack for PIC micros

Refer to the official data sheet for help

- https://www.sparkfun.com/datasheets/LCD/HD44780.pdf


# Notes

- Does not support reading the busy flag and reading in general (yet)
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