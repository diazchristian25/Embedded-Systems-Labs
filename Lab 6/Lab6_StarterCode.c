/*******************************
 * Name: Dario Ugalde
 * Student ID#: 1001268068
 * Lab Day: April 17, 2018
 * CSE 3442/5442 - Embedded Systems 1
 * Lab 6: Building a Watch - Timers and Interrupts 
 ********************************/

#include <xc.h> // For the XC8 Compiler (automatically finds/includes the specific PIC18F452.h header file)

#define _XTAL_FREQ  10000000     // Running at 10MHz (external oscillator/crystal), REQUIRED for delay functions

#define LED_LEFT    PORTAbits.RA3  // QwikFlash red LED (left) to toggle
#define LED_CENTER  PORTAbits.RA2  // QwikFlash red LED (center) to toggle
#define LED_RIGHT   PORTAbits.RA1  // QwikFlash red LED (right) to toggle

// PIC18F452 Configuration Bit Settings
#pragma config OSC     = HS     // Oscillator Selection bits (HS oscillator)
#pragma config OSCS    = OFF    // Oscillator System Clock Switch Enable bit (Oscillator system clock switch option is disabled (main oscillator is source))
#pragma config PWRT    = OFF    // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR     = OFF    // Brown-out Reset Enable bit (Brown-out Reset disabled)
#pragma config WDT     = OFF    // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config CCP2MUX = ON     // CCP2 Mux bit (CCP2 input/output is multiplexed with RC1)
#pragma config STVR    = ON     // Stack Full/Underflow Reset Enable bit (Stack Full/Underflow will cause RESET)
#pragma config LVP     = OFF    // Low Voltage ICSP Enable bit (Low Voltage ICSP disabled)

/*******************************
 * Global Vars
 * 
 * Strings for LCD Initialization and general use
 * For stability reasons, each array must have 10 total elements (no less, no more)
 ********************************/
const char LCDstr[]  = {0x33,0x32,0x28,0x01,0x0c,0x06,0x00,0x00}; // LCD Initialization string (do not change)

//Never change element [0] or [9] of the following char arrays
//You may only change the middle 8 elements for displaying on the LCD
char Str_1[] = {0x80,' ',' ',' ',' ',' ',' ',' ',' ',0};    // First line of LCD
char Str_2[] = {0xC0,' ',' ',' ',' ',' ',' ',' ',' ',0};    // Second line of LCD

const char Clear1[] = {0x80,' ',' ',' ',' ',' ',' ',' ',' ',0};  // Clear first line of LCD
const char Clear2[] = {0xC0,' ',' ',' ',' ',' ',' ',' ',' ',0};  // Clear second line of LCD
int hours = 0;   // Variable to store the number of hours
int minutes = 0; // Variable to store the number of minutes
int seconds = 0; // Variable to store the number of seconds

/*******************************
 * Function prototypes
 ********************************/
void Initialize_PIC(void);
void Initialize_LCD(void);
void Print_To_LCD(const char *);
void Toggle_LEDs(void);
void interrupt My_ISR_High(void);
void interrupt low_priority My_ISR_Low(void);

void main(void)
{
    Initialize_PIC();       //Initialize all settings required for general QwikFlash and LCD operation
	Print_To_LCD(Clear1);   //Clear the LCD line 1 to start your program
	Print_To_LCD(Clear2);   //Clear the LCD line 2 to start your program

	// Your personal PORT/TRIS/ADCON/etc settings or configurations can go here 
	// Or make your own function and call it
    
    // SFR settings for this program
    T0CON     = 0b00000101; // Adjust Timer0 settings for this program
    INTCON    = 0b00110000; // Adjust INTCON settings for this program
    INTCON2   = 0b11110000; // Adjust INTCON2 settings for this program 
    INTCON3   = 0b00001000; // Adjust INTCON3 settings for this program
    RCONbits.IPEN = 1;      // Enable interrupt priority
    
    // Timer0 settings for this program
    TMR0H = 0x67; // Send preload high value to TMR0H register
    TMR0L = 0x69; // Send preload low  value to TMR0L register
    
    // You can use the two delay functions defined in pic18.h (automatically included from xc.h):
    //      __delay_us(x);  // to delay in microsecond increments
    //                      // x is an unsigned long (0 - 4294967295)
    //
    //      __delay_ms(x);  // to delay in millisecond increments
    //                      // x is an unsigned long (0 - 4294967295)
    
    //Enable interrupts and start TMR0
    TRISB = 0b00001111;
    INTCONbits.GIE = 1; // Enable global interrupts
    INTCONbits.PEIE = 1; // Enable peripheral interrupts
    T0CONbits.TMR0ON = 1; // Start Timer0
    
    //Main routine, forever in an infinite while(true) loop
    while(1)
    {    
        T0CONbits.TMR0ON = 1;
    }
}

/*******************************
 * Initialize_PIC(void)
 *
 * This function performs all initializations of variables and registers
 * for the PIC18F452 when specifically on the QwikFlash board.
 *
 * DO NOT CHANGE ANYTHING IN THIS FUNCTION
 ********************************/
void Initialize_PIC(void)
{
    // Reference the QwikFlash schematic (pdf) to understand analog/digital IO decisions
    
    ADCON1 = 0b10001110;    // Enable PORTA & PORTE digital I/O pins
    TRISA  = 0b11100001;    // Set I/O for PORTA
    TRISB  = 0b11011100;    // Set I/O for PORTB
    TRISC  = 0b11010000;    // Set I/0 for PORTC
    TRISD  = 0b00001111;    // Set I/O for PORTD
    TRISE  = 0b00000000;    // Set I/O for PORTE
    PORTA  = 0b00010000;    // Turn off all four LEDs driven from PORTA    
    LED_LEFT    = 0;        // All LEDs initially OFF
    LED_CENTER  = 0;        // All LEDs initially OFF
    LED_RIGHT   = 0;        // All LEDs initially OFF
    Initialize_LCD();       // Initialize LCD
}

/*******************************
 * Initialize_LCD(void)
 *
 * Initialize the Optrex 8x2 character LCD.
 * First wait for 0.1 second, to get past the displays power-on reset time.
 *
 * DO NOT CHANGE ANYTHING IN THIS FUNCTION
 *******************************/
void Initialize_LCD(void)
{
    char currentChar;
    char *tempPtr;

    __delay_ms(100);                // wait 0.1 sec (100 ms)

    PORTEbits.RE0 = 0;              // RS=0 for command
    tempPtr = LCDstr;

    while (*tempPtr)                // if the byte is not zero (end of string)
    {
        currentChar = *tempPtr;
        PORTEbits.RE1 = 1;          // Drive E pin high
        PORTD = currentChar;        // Send upper nibble
        PORTEbits.RE1 = 0;          // Drive E pin low so LCD will accept nibble          
        __delay_ms(10);             // wait 10 ms
        currentChar <<= 4;          // Shift lower nibble to upper nibble
        PORTEbits.RE1 = 1;          // Drive E pin high again
        PORTD = currentChar;        // Write lower nibble
        PORTEbits.RE1 = 0;          // Drive E pin low so LCD will process byte        
        __delay_ms(10);             // wait 10 ms	
        tempPtr++;                  // Increment pointer to next character
    }
}

/*******************************
 * Print_To_LCD(const char * tempPtr) 
 *
 * This function is called with the passing in of an array of a constant
 * display string.  It sends the bytes of the string to the LCD.  The first
 * byte sets the cursor position.  The remaining bytes are displayed, beginning
 * at that position.
 * This function expects a normal one-byte cursor-positioning code, 0xhh, or
 * an occasionally used two-byte cursor-positioning code of the form 0x00hh.
 *
 * DO NOT CHANGE ANYTHING IN THIS FUNCTION
 ********************************/
void Print_To_LCD(const char * tempPtr)
{
	char currentChar;
    PORTEbits.RE0 = 0;          // Drive RS pin low for cursor-positioning code

    while (*tempPtr)            // if the byte is not zero (end of string)
    {
        currentChar = *tempPtr;
        PORTEbits.RE1 = 1;      // Drive E pin high
        PORTD = currentChar;    // Send upper nibble
        PORTEbits.RE1 = 0;      // Drive E pin low so LCD will accept nibble
        currentChar <<= 4;      // Shift lower nibble to upper nibble
        PORTEbits.RE1 = 1;      // Drive E pin high again
        PORTD = currentChar;    // Write lower nibble
        PORTEbits.RE1 = 0;      // Drive E pin low so LCD will process byte
        __delay_ms(10);         // wait 10 ms	
        PORTEbits.RE0 = 1;      // Drive RS pin high for displayable characters
        tempPtr++;              // Increment pointerto next character
    }
}

/*******************************
 * Toggle_LEDs(void)
 *
 * This function simply toggles the QwikFlash's red LEDs in a simple sequence
 * The LED_X defines are at the top of this .c file
 * 
 * You may alter this function if you like or directly manipulate LEDs in other functions
 ********************************/
void Toggle_LEDs(void)
{
    LED_LEFT ^= 1;      // regardless of the bit's previous state, this flips it to 1 or 0
    __delay_ms(100);
    
    LED_CENTER ^= 1;
    __delay_ms(100);
    
    LED_RIGHT ^= 1;
    __delay_ms(100);
}

void interrupt My_ISR_High(void)
{
    seconds = 0;
    minutes = 0;
    hours = 0;
    Str_1[1] = '0' + (hours/10)%10;
    Str_1[2] = '0' + hours%10;
    Str_1[3] = ':';
    Str_1[4] = '0' + (minutes/10)%10;
    Str_1[5] = '0' + minutes%10;
    Str_1[6] = ':';
    Str_1[7] = '0' + (seconds/10)%10;
    Str_1[8] = '0' + seconds%10;
    Print_To_LCD(Str_1);
    INTCONbits.INT0IF = 0;
}

void interrupt low_priority My_ISR_Low(void)
{
    if(INTCON3bits.INT1IF == 1) // Interrupt handling for pausing clock
    {
        T0CONbits.TMR0ON = 0;
        Str_2[1] = '^';
        Print_To_LCD(Str_2);
        if(PORTBbits.RB1 == 0)
        {
            Str_2[1] = ' ';
            Print_To_LCD(Str_2);
            T0CONbits.TMR0ON = 1;
            INTCON3bits.INT1IF = 0;
        }
    }

    if(INTCONbits.TMR0IF == 1) // Interrupt handling for incrementing clock
    {
        seconds++;
        if(seconds/60 == 1)
        {
            seconds = 0;
            minutes++;
            if(minutes/60 == 1)
            {
                minutes = 0;
                hours++;
                if(hours/24 == 1)
                {
                    hours = 0;
                }
            }
        }
        Str_1[1] = '0' + (hours/10)%10;
        Str_1[2] = '0' + hours%10;
        Str_1[3] = ':';
        Str_1[4] = '0' + (minutes/10)%10;
        Str_1[5] = '0' + minutes%10;
        Str_1[6] = ':';
        Str_1[7] = '0' + (seconds/10)%10;
        Str_1[8] = '0' + seconds%10;
        Print_To_LCD(Str_1);
        TMR0H = 0x67; // Send preload high value to TMR0H register
        TMR0L = 0x69; // Send preload low  value to TMR0L register
        INTCONbits.TMR0IF = 0;
    }
}
