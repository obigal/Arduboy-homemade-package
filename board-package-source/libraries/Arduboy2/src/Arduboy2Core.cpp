/**
 * @file Arduboy2Core.cpp
 * \brief
 * The Arduboy2Core class for Arduboy hardware initilization and control.
 */

#include "Arduboy2Core.h"

const uint8_t PROGMEM lcdBootProgram[] = {
  // boot defaults are commented out but left here in case they
  // might prove useful for reference
  //
  // Further reading: https://www.adafruit.com/datasheets/SSD1306.pdf

#ifdef OLED_SH1106
  0x8D, 0x14,                   // Charge Pump Setting v = enable (0x14)
  0xA1,                         // Set Segment Re-map
  0xC8,                         // Set COM Output Scan Direction
  0x81, 0xCF,                   // Set Contrast v = 0xCF
  0xD9, 0xF1,                   // Set Precharge = 0xF1
  OLED_SET_COLUMN_ADDRESS_LO,   //Set column address for left most pixel 
  0xAF                          // Display On
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
 #if defined(OLED_96X96) || defined(OLED_128X64_ON_96X96)
  0x15, 0x10, 0x3f, //left most 32 pixels are invisible
 #elif defined(OLED_96X96_ON_128X128)
  0x15, 0x08, 0x37, //center 96 pixels horizontally
 #elif defined(OLED_64X128_ON_128X128)
  0x15, 0x10, 0x2f, //center 64 pixels horizontally
 #else
  0x15, 0x00, 0x3f, //Set column start and end address
 #endif
 #if defined (OLED_96X96) 
  0x75, 0x20, 0x7f, //Set row start and end address
 #elif defined (OLED_128X64_ON_96X96) 
  0x75, 0x30, 0x6f, //Set row start and end address
 #elif defined (OLED_128X96)
  0x75, 0x00, 0x5f, //Set row start and end address
 #elif defined(OLED_128X64_ON_128X96)
  0x75, 0x10, 0x4f, //Set row start and end address
 #elif defined(OLED_96X96_ON_128X128) || defined(OLED_128X96_ON_128X128)
  0x75, 0x10, 0x6f, //Set row start and end address to centered 96 lines
 #elif defined(OLED_128X64_ON_128X128)
  0x75, 0x20, 0x5f, //Set row start and end address to centered 64 lines
 #else
  0x75, 0x00, 0x7F, //Set row start and end address to use all 128 lines
 #endif
 #if defined(OLED_64X128_ON_128X128)
  0xA0, 0x51,       //set re-map: split odd-even COM signals|COM remap|column address remap
 #else
  0xA0, 0x55,       //set re-map: split odd-even COM signals|COM remap|vertical address increment|column address remap
 #endif
  0xA1, 0x00,       //set display start line
  0xA2, 0x00,       //set display offset
  //0xA4,           //Normal display
  0xA8, 0x7F,       //Set MUX ratio 128MUX
  //0xB2, 0x23,
  //0xB3, 0xF0,     //set devider clock | oscillator frequency
  0x81, 0xCF,       //Set contrast
  //0xBC, 0x1F,     //set precharge voltage
  //0x82, 0xFE,     //set second Precharge speed
  0xB1, 0x21,       //reset and 1st precharge phase length  phase 2:2 DCLKs, Phase 1: 1 DCLKs
  //0xBB, 0x0F,     //set 2nd precharge period: 15 DCLKs
  //0xbe, 0x1F,     //output level high voltage com signal
  //0xB8, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, //set gray scale table
  0xAF              //Display on
#else
  // for SSD1306 and SSD1309 displays
  //
  // Display Off
  // 0xAE,

  // Set Display Clock Divisor v = 0xF0
  // default is 0x80
  0xD5, 0xF0,

  // Set Multiplex Ratio v = 0x3F
  // 0xA8, 0x3F,

  // Set Display Offset v = 0
  // 0xD3, 0x00,

  // Set Start Line (0)
  // 0x40,
 #if defined OLED_SSD1309
  //Charge Pump command not supported, use two NOPs instead to keep same size and easy patchability
  0xE3, 0xE3,
 #else  
  // Charge Pump Setting v = enable (0x14)
  // default is disabled
  0x8D, 0x14,
 #endif

  // Set Segment Re-map (A0) | (b0001)
  // default is (b0000)
  0xA1,

  // Set COM Output Scan Direction
  0xC8,

  // Set COM Pins v
  // 0xDA, 0x12,

  // Set Contrast v = 0xCF
  0x81, 0xCF,

  // Set Precharge = 0xF1
  0xD9, 0xF1,

  // Set VCom Detect
  // 0xDB, 0x40,

  // Entire Display ON
  // 0xA4,

  // Set normal/inverse display
  // 0xA6,

  // Display On
  0xAF,

  // set display mode = horizontal addressing mode (0x00)
  0x20, 0x00,

  // set col address range
  // 0x21, 0x00, COLUMN_ADDRESS_END,

  // set page address range
  // 0x22, 0x00, PAGE_ADDRESS_END
#endif
};

Arduboy2Core::Arduboy2Core() { }

void Arduboy2Core::boot()
{
  #ifdef ARDUBOY_SET_CPU_8MHZ
  // ARDUBOY_SET_CPU_8MHZ will be set by the IDE using boards.txt
  setCPUSpeed8MHz();
  #endif

  // Select the ADC input here so a delay isn't required in initRandomSeed()
  ADMUX = RAND_SEED_IN_ADMUX;

  bootPins();
  bootSPI();
  bootOLED();
  bootPowerSaving();
}

#ifdef ARDUBOY_SET_CPU_8MHZ
// If we're compiling for 8MHz we need to slow the CPU down because the
// hardware clock on the Arduboy is 16MHz.
// We also need to readjust the PLL prescaler because the Arduino USB code
// likely will have incorrectly set it for an 8MHz hardware clock.
void Arduboy2Core::setCPUSpeed8MHz()
{
  uint8_t oldSREG = SREG;
  cli();                // suspend interrupts
  PLLCSR = _BV(PINDIV); // dissable the PLL and set prescale for 16MHz)
  CLKPR = _BV(CLKPCE);  // allow reprogramming clock
  CLKPR = 1;            // set clock divisor to 2 (0b0001)
  PLLCSR = _BV(PLLE) | _BV(PINDIV); // enable the PLL (with 16MHz prescale)
  SREG = oldSREG;       // restore interrupts
}
#endif

// Pins are set to the proper modes and levels for the specific hardware.
// This routine must be modified if any pins are moved to a different port
void Arduboy2Core::bootPins()
{
#ifdef ARDUBOY_10
  // Port B INPUT_PULLUP or HIGH
  PORTB = (_BV(RED_LED_BIT) | _BV(BLUE_LED_BIT) | //RGB LED off
         #ifndef AB_ALTERNATE_WIRING
          _BV(GREEN_LED_BIT) | 
         #endif
         #ifndef ARDUINO_AVR_MICRO
          _BV(RX_LED_BIT) | //RX LED off for Arduboy and non Micro based Arduino
         #endif          
          _BV(B_BUTTON_BIT)) &
  // Port B INPUT or LOW                    
          ~(_BV(SPI_MISO_BIT) | _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT));

  // Port B outputs
  DDRB = (_BV(RED_LED_BIT)   | _BV(BLUE_LED_BIT) |
        #ifndef AB_ALTERNATE_WIRING
         _BV(GREEN_LED_BIT) | 
        #endif
         _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT)  | _BV(RX_LED_BIT)) &
  // Port B inputs
         ~(_BV(B_BUTTON_BIT) | _BV(SPI_MISO_BIT));

  // Port C
  // Speaker: Not set here. Controlled by audio class

  // Port D INPUT_PULLUP or HIGH
  PORTD = (
         #ifdef AB_ALTERNATE_WIRING
          _BV(GREEN_LED_BIT) |
         #endif
         #ifndef ARDUINO_AVR_MICRO
          _BV(TX_LED_BIT) | //TX LED off for Arduboy and non Micro based Arduino
         #endif          
          _BV(CART_BIT) | _BV(DC_BIT)) & //flash cart inactive, LCD data mode
  // Port D INPUT or LOW          
          ~(_BV(CS_BIT) | _BV(RST_BIT)  //oled chip enabled, reset active
         #ifdef AB_ALTERNATE_WIRING
          | _BV(SPEAKER_2_BIT)
         #endif
          );
  
  // Port D outputs
  DDRD = _BV(RST_BIT) | _BV(CS_BIT) | _BV(DC_BIT) | 
        #ifdef AB_ALTERNATE_WIRING
         _BV(GREEN_LED_BIT) |
        #endif
         _BV(CART_BIT) | _BV(TX_LED_BIT);
  // Port D inputs (none)

  // Port E INPUT_PULLUP or HIGH
  PORTE |= _BV(A_BUTTON_BIT);
  // Port E INPUT or LOW (none)
  // Port E inputs
  DDRE &= ~(_BV(A_BUTTON_BIT));
  // Port E outputs (none)

  // Port F INPUT_PULLUP or HIGH
  PORTF = (_BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) |
          _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT)) &
  // Port F INPUT or LOW
          ~(_BV(RAND_SEED_IN_BIT));
  
  // Port F outputs (none)
  DDRF = 0 &
  // Port F inputs
         ~(_BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) |
         _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
         _BV(RAND_SEED_IN_BIT));

#elif defined(AB_DEVKIT)

  // Port B INPUT_PULLUP or HIGH
  PORTB |= _BV(LEFT_BUTTON_BIT) | _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
           _BV(BLUE_LED_BIT);
  // Port B INPUT or LOW (none)
  // Port B inputs
  DDRB &= ~(_BV(LEFT_BUTTON_BIT) | _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
            _BV(SPI_MISO_BIT));
  // Port B outputs
  DDRB |= _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT) | _BV(BLUE_LED_BIT);

  // Port C INPUT_PULLUP or HIGH
  PORTC |= _BV(RIGHT_BUTTON_BIT);
  // Port C INPUT or LOW (none)
  // Port C inputs
  DDRC &= ~(_BV(RIGHT_BUTTON_BIT));
  // Port C outputs (none)

  // Port D INPUT_PULLUP or HIGH
  PORTD |= _BV(CS_BIT);
  // Port D INPUT or LOW
  PORTD &= ~(_BV(RST_BIT));
  // Port D inputs (none)
  // Port D outputs
  DDRD |= _BV(RST_BIT) | _BV(CS_BIT) | _BV(DC_BIT);

  // Port E (none)

  // Port F INPUT_PULLUP or HIGH
  PORTF |= _BV(A_BUTTON_BIT) | _BV(B_BUTTON_BIT);
  // Port F INPUT or LOW
  PORTF &= ~(_BV(RAND_SEED_IN_BIT));
  // Port F inputs
  DDRF &= ~(_BV(A_BUTTON_BIT) | _BV(B_BUTTON_BIT) | _BV(RAND_SEED_IN_BIT));
  // Port F outputs (none)
  // Speaker: Not set here. Controlled by audio class

#endif
}

void Arduboy2Core::bootOLED()
{
    // reset the display
    uint8_t cmd;
    const void* ptr = lcdBootProgram;
    asm volatile(
    "1:                               \n\t" //assembly loop for 2nd delayShort(5)
    );
    delayShort(5);                          //for a short active low reset pulse
    asm volatile(
    "    sbic %[rst_port], %[rst_bit] \n\t" //continue if reset is active
    "    rjmp 2f                      \n\t" //else break
    "    sbi  %[rst_port], %[rst_bit] \n\t" //deactivate reset
    "    rjmp 1b                      \n\t" //loop for a recover from reset delay
    "2:                              \n\t"
    :
    : [rst_port] "I" (_SFR_IO_ADDR(RST_PORT)),
      [rst_bit]  "I" (RST_BIT)
    :
  );
 #if defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  for (uint16_t i = 0; i < 8192; i++) SPItransfer(0); //make sure all display ram is cleared
 #endif
  //bitClear(CS_PORT, CS_BIT);               // select the display as default SPI device, already cleared by boot pins)
  LCDCommandMode();
  asm volatile(
    "    ldi  r25, %[size]           \n\t" // for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++) 
    "3:                              \n\t" // {
    "    lpm  %[cmd], Z+             \n\t" //   cmd = pgm_read_byte(lcdBootProgram + i));
    : [ptr] "+z" (ptr),
      [cmd] "=r" (cmd)
    : [size] "I" (sizeof(lcdBootProgram))
    : "r25"
  );    
  SPItransfer(cmd);                        //   transfer display command
  asm volatile(
    "    dec  r25                    \n\t" // }
    "    brne 3b                     \n\t"
    :
    :
    : "r25"
  );
  LCDDataMode();
}

void Arduboy2Core::LCDDataMode()
{
  bitSet(DC_PORT, DC_BIT);
}

void Arduboy2Core::LCDCommandMode()
{
  bitClear(DC_PORT, DC_BIT);
}

// Initialize the SPI interface for the display
void Arduboy2Core::bootSPI()
{
// master, mode 0, MSB first, CPU clock / 2 (8MHz)
  SPCR = _BV(SPE) | _BV(MSTR);
  SPSR = _BV(SPI2X);
}

// Write to the SPI bus (MOSI pin)
uint8_t Arduboy2Core::SPItransfer(uint8_t data)
{
  SPDR = data;
  /*
   * The following NOP introduces a small delay that can prevent the wait
   * loop form iterating when running at the maximum speed. This gives
   * about 10% more speed, even if it seems counter-intuitive. At lower
   * speeds it is unnoticed.
   */
  asm volatile("nop");
  while (!(SPSR & _BV(SPIF))) { } // wait
  return SPDR;
}

void Arduboy2Core::safeMode()
{
  if (buttonsState() == UP_BUTTON)
  {
    digitalWriteRGB(RED_LED, RGB_ON);

#ifndef ARDUBOY_CORE // for Arduboy core timer 0 should remain enabled
    // prevent the bootloader magic number from being overwritten by timer 0
    // when a timer variable overlaps the magic number location
    power_timer0_disable();
#endif

    while (true) { }
  }
}


/* Power Management */

void Arduboy2Core::idle()
{
  SMCR = _BV(SE); // select idle mode and enable sleeping
  sleep_cpu();
  SMCR = 0; // disable sleeping
}

void Arduboy2Core::bootPowerSaving()
{
  // disable Two Wire Interface (I2C) and the ADC
  // All other bits will be written with 0 so will be enabled
  PRR0 = _BV(PRTWI) | _BV(PRADC);
  // disable USART1
  PRR1 = _BV(PRUSART1);
  // All other bits will be written with 0 so will be enabled
}

// Shut down the display
void Arduboy2Core::displayOff()
{
  LCDCommandMode();
  SPItransfer(0xAE); // display off
  SPItransfer(0x8D); // charge pump:
  SPItransfer(0x10); //   disable
  delayShort(250);
  bitClear(RST_PORT, RST_BIT); // set display reset pin low (reset state)
}

// Restart the display after a displayOff()
void Arduboy2Core::displayOn()
{
  bootOLED();
}

uint8_t Arduboy2Core::width() { return WIDTH; }

uint8_t Arduboy2Core::height() { return HEIGHT; }


/* Drawing */

void Arduboy2Core::paint8Pixels(uint8_t pixels)
{
  SPItransfer(pixels);
}

void Arduboy2Core::paintScreen(const uint8_t *image)
{
#ifdef OLED_SH1106 
  for (uint8_t i = 0; i < HEIGHT / 8; i++)
  {
    LCDCommandMode();
    SPDR = (OLED_SET_PAGE_ADDRESS + i);
    while (!(SPSR & _BV(SPIF)));
    SPDR = (OLED_SET_COLUMN_ADDRESS_HI); // only reset hi nibble to zero
    while (!(SPSR & _BV(SPIF)));
    LCDDataMode();
    for (uint8_t j = WIDTH; j > 0; j--)
      {
        SPDR = pgm_read_byte(image++);
        while (!(SPSR & _BV(SPIF)));
      }
  }
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128) || defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128)
 #if defined(OLED_128X64_ON_96X96)
  uint16_t i = 16;
  for (uint8_t col = 0; col < 96 / 2; col++)
 #else     
  uint16_t i = 0;
  for (uint8_t col = 0; col < WIDTH / 2; col++)
 #endif     
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b1 = pgm_read_byte(image + i);
      uint8_t b2 = pgm_read_byte(image + i + 1);
      for (uint8_t shift = 0; shift < 8; shift++)
      {
        uint8_t c = 0xFF;
        if ((b1 & 1) == 0) c &= 0x0F;
        if ((b2 & 1) == 0) c &= 0xF0;
        SPDR = c;
        b1 = b1 >> 1;
        b2 = b2 >> 1;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH - 2;
  }
#elif defined(OLED_64X128_ON_128X128)
  uint16_t i = WIDTH-1;
  for (uint8_t col = 0; col < WIDTH ; col++)
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b = pgm_read_byte(image + i);
      if (clear) *(image + i) = 0;
      for (uint8_t shift = 0; shift < 4; shift++)
      {
        uint8_t c = 0xFF;
        if ((b & _BV(0)) == 0) c &= 0x0F;
        if ((b & _BV(1)) == 0) c &= 0xF0;
        SPDR = c;
        b = b >> 2;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH  + 1;
  }
#else 
  //OLED SSD1306 and compatibles
  for (int i = 0; i < (HEIGHT*WIDTH)/8; i++)
  {
    SPItransfer(pgm_read_byte(image + i));
  }
#endif
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void Arduboy2Core::paintScreen(uint8_t image[], bool clear)
{
#ifdef OLED_SH1106 
  //Assembly optimized page mode display code with clear support.
  //Each byte transfer takes 18 cycles
  asm volatile (
    "     ldi  r19, %[page_cmd]                     \n\t"
    "1:                                             \n\t"
    "     ldi  r18, %[col_cmd]        ;1            \n\t"
    "     ldi  r20, 6                 ;1            \n\t"
    "     cbi  %[dc_port], %[dc_bit]  ;2 cmd mode   \n\t"         
    "                                               \n\t"
    "     out  %[spdr], r19           ;1            \n\t"         
    "2:   dec  r20                    ;6*3-1 : 17   \n\t"         
    "     brne 2b                                   \n\t"         
    "     out  %[spdr], r18           ;1            \n\t"        
    
    "     ldi  r18, %[width]          ;1            \n\t"         
    "     inc  r18                    ;1            \n\t"              
    "     rjmp 5f                     ;2            \n\t"              
    "4:                                             \n\t"
    "     lpm  r20, Z                 ;3 delay      \n\t"
    "     ld   r20, Z                 ;2            \n\t"
    "     sbi  %[dc_port], %[dc_bit]  ;2 data mode  \n\t"
    "     out  %[spdr], r20           ;1            \n\t" 
    "     cpse %[clear], __zero_reg__ ;1/2          \n\t" 
    "     mov  r20, __zero_reg__      ;1            \n\t" 
    "     st   Z+, r20                ;2            \n\t"
    "5:                                             \n\t"
    "     lpm  r20, Z                 ;3 delay      \n\t"
    "     dec  r18                    ;1            \n\t"
    "     brne 4b                     ;1/2          \n\t"
    "     inc  r19                    ;1            \n\t"
    "     cpi  r19,%[page_end]        ;1            \n\t"
    "     brne 1b                     ;1/2          \n\t"
    "     in    __tmp_reg__, %[spsr]                \n\t" //read SPSR to clear SPIF
    : [ptr]      "+&z" (image)
    : 
      [page_cmd] "M" (OLED_SET_PAGE_ADDRESS),
      [page_end] "M" (OLED_SET_PAGE_ADDRESS + (HEIGHT / 8)),
      [dc_port]  "I" (_SFR_IO_ADDR(DC_PORT)),
      [dc_bit]   "I" (DC_BIT),
      [spdr]     "I" (_SFR_IO_ADDR(SPDR)),
      [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
      [col_cmd]  "M" (OLED_SET_COLUMN_ADDRESS_HI),
      [width]    "M" (WIDTH),
      [clear]    "r" (clear)
    : "r18", "r19", "r20"
  );
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128)|| defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128)
  // 1 bit to 4-bit expander display code with clear support.
  // Each transfer takes 18 cycles with additional 4 cycles for a column change.
  asm volatile(
   #if defined(OLED_128X64_ON_96X96)
    "  adiw   r30, 16                           \n\t"          
   #endif
    "  ldi   r25, %[col]                        \n\t"          
    ".lcolumn:                                  \n\t"         
    "   ldi  r24, %[row]            ;1          \n\t"
    ".lrow:                                     \n\t"
    "   ldi  r21, 7                 ;1          \n\t"
    "   ld   r22, z                 ;2          \n\t"
    "   ldd  r23, z+1               ;2          \n\t"
    ".lshiftstart:                              \n\t"
    "   ldi  r20, 0xFF              ;1          \n\t"
    "   sbrs r22, 0                 ;1          \n\t"
    "   andi r20, 0x0f              ;1          \n\t"
    "   sbrs r23, 0                 ;1          \n\t"
    "   andi r20,0xf0               ;1          \n\t"
    "   out  %[spdr], r20           ;1          \n\t"
    "                                           \n\t"
    "   cp   %[clear], __zero_reg__ ;1          \n\t"
    "   brne .lclear1               ;1/2        \n\t"
    ".lshiftothers:                             \n\t"
    "   movw r18, %A[ptr]           ;1          \n\t"
    "   rjmp .+0                    ;2          \n\t"
    "   rjmp .lshiftnext            ;2          \n\t"
    ".lclear1:                                  \n\t"
    "   st   z, __zero_reg__        ;2          \n\t" 
    "   std  z+1, __zero_reg__      ;2          \n\t"
    ".lshiftnext:                               \n\t"
    "                                           \n\t"
    "   lsr  r22                    ;1          \n\t"
    "   lsr  r23                    ;1          \n\t"
    "                                           \n\t"
    "   ldi  r20, 0xFF              ;1          \n\t"
    "   sbrs r22, 0                 ;1/2        \n\t"
    "   andi r20, 0x0f              ;1          \n\t"
    "   sbrs r23, 0                 ;1/2        \n\t"
    "   andi r20,0xf0               ;1          \n\t"
    "                                           \n\t"
    "   subi r18, %[top_lsb]        ;1          \n\t" //image - (WIDTH * ((HEIGHT / 8) - 1) - 2)
    "   sbci r19, %[top_msb]        ;1          \n\t"
    "   subi r21, 1                 ;1          \n\t"
    "   out  %[spdr], r20           ;1          \n\t"
    "   brne .lshiftothers          ;1/2        \n\t"
    "                                           \n\t"
    "   nop                         ;1          \n\t"
    "   subi %A[ptr], %[width]      ;1          \n\t" //image + width (negated addition)
    "   sbci %B[ptr], -1            ;1          \n\t"
    "   subi r24, 1                 ;1          \n\t"
    "   brne .lrow                  ;1/2        \n\t"
    "                                           \n\t"
    "   movw %A[ptr], r18           ;1          \n\t"
    "   subi r25, 1                 ;1          \n\t"
    "   brne .lcolumn               ;1/2        \n\t"
    "   in    __tmp_reg__, %[spsr]              \n\t" //read SPSR to clear SPIF
    : [ptr]     "+&z" (image)
    : [spdr]    "I" (_SFR_IO_ADDR(SPDR)),
      [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
      [row]     "M" (HEIGHT / 8),
     #if defined(OLED_128X64_ON_96X96)
      [col]     "M" (96 / 2),
     #else
      [col]     "M" (WIDTH / 2),
     #endif
      [width]   "M" (256 - WIDTH),
      [top_lsb] "M" ((WIDTH * ((HEIGHT / 8) - 1) - 2) & 0xFF),
      [top_msb] "M" ((WIDTH * ((HEIGHT / 8) - 1) - 2) >> 8),
      [clear]   "r" (clear)
    : "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25"
  );
#elif defined(OLED_64X128_ON_128X128)
  uint16_t i = WIDTH-1;
  for (uint8_t col = 0; col < WIDTH ; col++)
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b = *(image + i);
      if (clear) *(image + i) = 0;
      for (uint8_t shift = 0; shift < 4; shift++)
      {
        uint8_t c = 0xFF;
        if ((b & _BV(0)) == 0) c &= 0x0F;
        if ((b & _BV(1)) == 0) c &= 0xF0;
        SPDR = c;
        b = b >> 2;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH  + 1;
  }
#else
  //OLED SSD1306 and compatibles
  //data only transfer with clear support at 18 cycles per transfer
  uint16_t count;

  asm volatile (
    "   ldi   %A[count], %[len_lsb]               \n\t" //for (len = WIDTH * HEIGHT / 8)
    "   ldi   %B[count], %[len_msb]               \n\t"
    "1: ld    __tmp_reg__, %a[ptr]      ;2        \n\t" //tmp = *(image)
    "   out   %[spdr], __tmp_reg__      ;1        \n\t" //SPDR = tmp
    "   cpse  %[clear], __zero_reg__    ;1/2      \n\t" //if (clear) tmp = 0;
    "   mov   __tmp_reg__, __zero_reg__ ;1        \n\t"
    "2: sbiw  %A[count], 1              ;2        \n\t" //len --
    "   sbrc  %A[count], 0              ;1/2      \n\t" //loop twice for cheap delay
    "   rjmp  2b                        ;2        \n\t"
    "   st    %a[ptr]+, __tmp_reg__     ;2        \n\t" //*(image++) = tmp
    "   brne  1b                        ;1/2 :18  \n\t" //len > 0
    "   in    __tmp_reg__, %[spsr]                \n\t" //read SPSR to clear SPIF
    : [ptr]     "+&e" (image),
      [count]   "=&w" (count)
    : [spdr]    "I"   (_SFR_IO_ADDR(SPDR)),
      [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
      [len_msb] "M"   (WIDTH * (HEIGHT / 8 * 2) >> 8),   // 8: pixels per byte
      [len_lsb] "M"   (WIDTH * (HEIGHT / 8 * 2) & 0xFF), // 2: for delay loop multiplier
      [clear]   "r"   (clear)
  );
  #endif  
}

void Arduboy2Core::blank()
{
#ifdef OLED_SH1106 
  for (int i = 0; i < (HEIGHT * 132) / 8; i++)
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128)|| defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  for (int i = 0; i < (HEIGHT * WIDTH) / 2; i++)
#else //OLED SSD1306 and compatibles
  for (int i = 0; i < (HEIGHT * WIDTH) / 8; i++)
#endif
    SPItransfer(0x00);
}

void Arduboy2Core::sendLCDCommand(uint8_t command)
{
  LCDCommandMode();
  SPItransfer(command);
  LCDDataMode();
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void Arduboy2Core::invert(bool inverse)
{
  sendLCDCommand(inverse ? OLED_PIXELS_INVERTED : OLED_PIXELS_NORMAL);
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void Arduboy2Core::allPixelsOn(bool on)
{
  sendLCDCommand(on ? OLED_ALL_PIXELS_ON : OLED_PIXELS_FROM_RAM);
}

// flip the display vertically or set to normal
void Arduboy2Core::flipVertical(bool flipped)
{
  sendLCDCommand(flipped ? OLED_VERTICAL_FLIPPED : OLED_VERTICAL_NORMAL);
}

// flip the display horizontally or set to normal
void Arduboy2Core::flipHorizontal(bool flipped)
{
  sendLCDCommand(flipped ? OLED_HORIZ_FLIPPED : OLED_HORIZ_NORMAL);
}

/* RGB LED */

void Arduboy2Core::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{ 
#ifdef ARDUBOY_10 // RGB, all the pretty colors
  uint8_t pwmstate = TCCR0A;
 #ifndef AB_ALTERNATE_WIRING
  pwmstate &= ~_BV(COM0A1); //default to digital pin for min and max values
 #else  
  pwmstate &= ~_BV(COM0B1);
 #endif
  if (green == 0)
    bitSet(GREEN_LED_PORT, GREEN_LED_BIT); 
  else if (green == 255)
    bitClear(GREEN_LED_PORT, GREEN_LED_BIT); 
  else 
  {
   #ifndef AB_ALTERNATE_WIRING
    pwmstate |= _BV(COM0A1); //configure pin as pwm pin
    OCR0A = 255 - green;     //set pwm duty
   #else
	pwmstate |= _BV(COM0B1);
	OCR0B = 255 - green;            
   #endif
  }
  TCCR0A = pwmstate;
  pwmstate = TCCR1A & ~(_BV(COM1B1) | _BV(COM1A1)); //default to digital pins for min and max values
  if (red == 0) bitSet(RED_LED_PORT, RED_LED_BIT);
  else if (red == 255) bitClear(RED_LED_PORT, RED_LED_BIT);
  else
  {
    pwmstate |= _BV(COM1B1); //configure pin as pwm pin
    OCR1BH = 0;
    OCR1BL = 255 - red;      //set pwm duty
  } 
  if (blue == 0) bitSet(BLUE_LED_PORT, BLUE_LED_BIT);
  else if (blue == 255) bitClear(BLUE_LED_PORT, BLUE_LED_BIT);
  else
  {
    pwmstate |= _BV(COM1A1); //configure pin as pwm pin
    OCR1AH = 0;
    OCR1AL = 255 - blue;     //set pwm duty
  } 
  TCCR1A = pwmstate;
#elif defined(AB_DEVKIT)
  // only blue on DevKit, which is not PWM capable
  (void)red;    // parameter unused
  (void)green;  // parameter unused
  bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, blue ? RGB_ON : RGB_OFF);
#endif
}

void Arduboy2Core::setRGBled(uint8_t color, uint8_t val)
{
#ifdef ARDUBOY_10
  if (color == RED_LED)
  {
    OCR1BL = val;
  }
  else if (color == GREEN_LED)
  {
   #ifndef AB_ALTERNATE_WIRING
    OCR0A = 255 - val;
   #else
	OCR0B = 255 - val;            
   #endif
  }
  else if (color == BLUE_LED)
  {
    OCR1AL = val;
  }
#elif defined(AB_DEVKIT)
  // only blue on DevKit, which is not PWM capable
  if (color == BLUE_LED)
  {
    bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, val ? RGB_ON : RGB_OFF);
  }
#endif
}

void Arduboy2Core::freeRGBled()
{
#ifdef ARDUBOY_10
  // clear the COM bits to return the pins to normal I/O mode
  TCCR0A = _BV(WGM01) | _BV(WGM00);
  TCCR1A = _BV(WGM10);
#endif
}

void Arduboy2Core::digitalWriteRGB(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10
  bitWrite(RED_LED_PORT, RED_LED_BIT, red);
  bitWrite(GREEN_LED_PORT, GREEN_LED_BIT, green);
  bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, blue);
#elif defined(AB_DEVKIT)
  // only blue on DevKit
  (void)red;    // parameter unused
  (void)green;  // parameter unused
  bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, blue);
#endif
}

void Arduboy2Core::digitalWriteRGB(uint8_t color, uint8_t val)
{
#ifdef ARDUBOY_10
  if (color == RED_LED)
  {
    bitWrite(RED_LED_PORT, RED_LED_BIT, val);
  }
  else if (color == GREEN_LED)
  {
    bitWrite(GREEN_LED_PORT, GREEN_LED_BIT, val);
  }
  else if (color == BLUE_LED)
  {
    bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, val);
  }
#elif defined(AB_DEVKIT)
  // only blue on DevKit
  if (color == BLUE_LED)
  {
    bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, val);
  }
#endif
}

/* Buttons */

uint8_t Arduboy2Core::buttonsState()
{
#ifndef ARDUBOY_CORE
  uint8_t buttons;
 #ifdef ARDUBOY_10
  // up, right, left, down
  buttons = ((~PINF) &
              (_BV(UP_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) |
               _BV(LEFT_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT)));
  // A
  if (bitRead(A_BUTTON_PORTIN, A_BUTTON_BIT) == 0) { buttons |= A_BUTTON; }
  // B
  if (bitRead(B_BUTTON_PORTIN, B_BUTTON_BIT) == 0) { buttons |= B_BUTTON; }
#elif defined(AB_DEVKIT)
  // down, left, up
  buttons = ((~PINB) &
              (_BV(DOWN_BUTTON_BIT) | _BV(LEFT_BUTTON_BIT) | _BV(UP_BUTTON_BIT)));
  // right
  if (bitRead(RIGHT_BUTTON_PORTIN, RIGHT_BUTTON_BIT) == 0) { buttons |= RIGHT_BUTTON; }
  // A
  if (bitRead(A_BUTTON_PORTIN, A_BUTTON_BIT) == 0) { buttons |= A_BUTTON; }
  // B
  if (bitRead(B_BUTTON_PORTIN, B_BUTTON_BIT) == 0) { buttons |= B_BUTTON; }
 #endif
#else
  register uint8_t buttons asm("r24");
  asm volatile("call scan_buttons\n\t" : "=d" (buttons));
#endif  
  return buttons;
}

// delay in ms with 16 bit duration
void Arduboy2Core::delayShort(uint16_t ms)
{
  #ifndef ARDUBOY_CORE
    delay((unsigned long) ms);
  #else
  ::delayShort(ms);
  #endif
}

void Arduboy2Core::exitToBootloader()
{
#ifndef ARDUBOY_CORE
  cli();
  // set bootloader magic key
  // storing two uint8_t instead of one uint16_t saves an instruction
  //  when high and low bytes of the magic key are the same
  *(uint8_t *)MAGIC_KEY_POS = lowByte(MAGIC_KEY);
  *(uint8_t *)(MAGIC_KEY_POS + 1) = highByte(MAGIC_KEY);
  // enable watchdog timer reset, with 16ms timeout
  wdt_reset();
  WDTCSR = (_BV(WDCE) | _BV(WDE));
  WDTCSR = _BV(WDE);
  while (true) { }
#else
  bootloader_timer = 120; //ms
#endif
}

// Replacement main() that eliminates the USB stack code.
// Used by the ARDUBOY_NO_USB macro. This should not be called
// directly from a sketch.

void Arduboy2Core::mainNoUSB()
{
  // disable USB
  UDCON = _BV(DETACH);
  UDIEN = 0;
  UDINT = 0;
  USBCON = _BV(FRZCLK);
  UHWCON = 0;
  power_usb_disable();

  init();

  // This would normally be done in the USB code that uses the TX and RX LEDs
  //TX_RX_LED_INIT; // configured by bootpins

  // Set the DOWN button pin for INPUT_PULLUP
  bitSet(DOWN_BUTTON_PORT, DOWN_BUTTON_BIT);
  bitClear(DOWN_BUTTON_DDR, DOWN_BUTTON_BIT);

  // Delay to give time for the pin to be pulled high if it was floating
  delayShort(10);

  // if the DOWN button is pressed
  if (bitRead(DOWN_BUTTON_PORTIN, DOWN_BUTTON_BIT) == 0) {
    exitToBootloader();
  }

  // The remainder is a copy of the Arduino main() function with the
  // USB code and other unneeded code commented out.
  // init() was called above.
  // The call to function initVariant() is commented out to fix compiler
  // error: "multiple definition of 'main'".
  // The return statement is removed since this function is type void.

//  init();

//  initVariant();

//#if defined(USBCON)
//  USBDevice.attach();
//#endif

  setup();

  for (;;) {
    loop();
//    if (serialEventRun) serialEventRun();
  }

//  return 0;
}

