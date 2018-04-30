/*******************************
 * Name:
 * Student ID#:
 * Lab Day:
 * CSE 3442/5442 - Embedded Systems 1
 * Lab x: Lab Title
 ********************************/

#include <xc.h> // For the XC8 Compiler (automatically finds/includes the specific PIC18F452.h header file)
#include <String.h>

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


/*******************************
 * Function prototypes
 ********************************/
void Initialize_PIC(void);
void Initialize_LCD(void);
void Print_To_LCD(const char *);
void Toggle_LEDs(void);
void Addition(void);
void Subtraction(void);
void BandC(void);
void notB(void);

void main(void)
{
    Initialize_PIC();       //Initialize all settings required for general QwikFlash and LCD operation
	Print_To_LCD(Clear1);   //Clear the LCD line 1 to start your program
	Print_To_LCD(Clear2);   //Clear the LCD line 2 to start your program

	// Your personal PORT/TRIS/ADCON/etc settings or configurations can go here 
	// Or make your own function and call it
            
    // You can use the two delay functions defined in pic18.h (automatically included from xc.h):
    //      __delay_us(x);  // to delay in microsecond increments
    //                      // x is an unsigned long (0 - 4294967295)
    //
    //      __delay_ms(x);  // to delay in millisecond increments
    //                      // x is an unsigned long (0 - 4294967295)
    
    //Main routine, forever in an infinite while(true) loop
    
    TRISB = 0b11111111; //PORTB is input
    TRISC = 0b11111111; //PORTC is input
    
    while(1)
    {
        if(PORTBbits.RB4 == 1 && PORTBbits.RB5 == 1) {
            notB();
        }
        else if(PORTBbits.RB4 == 1 && PORTBbits.RB5 == 0) {
            BandC();
        }
        else if(PORTBbits.RB4 == 0 && PORTBbits.RB5 == 1) {
            Subtraction();
        }
        else if(PORTBbits.RB4 == 0 && PORTBbits.RB5 == 0) {
            Addition();
        }
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

/*******************************
 * Addition(void)
 * Print out the operation to be done on the top row of the LCD
 * Find the sum of the A bits and B bits
 * Multiply the two sums
 * Print the product on the bottom row of the LCD
 ********************************/
void Addition(void) // Now modified to handle 4-bit multiplication
{
    char B = 0;
    char C = 0;
    char product = 0;
    char loc = 6;
    char num = 0;
    strcpy(Str_1, Clear1);
    strcpy(Str_2, Clear2);
    Str_1[2] = 'B';
    Str_1[3] = '*';
    Str_1[4] = 'C';
    Str_1[5] = '=';
    Str_2[3] = '+';
    Print_To_LCD(Str_1);  //Up to this point in the function the upper part of the LCD prints out the operation to be done and the lower prints '+'
    
    if(PORTCbits.RC3 == 1){ //Series of if statements to determine the sum of C bits
        C = C + 8;
    }
    if(PORTCbits.RC2 == 1){
        C = C + 4;
    }
    if(PORTCbits.RC1 == 1){
        C = C + 2;
    }
    if(PORTCbits.RC0 == 1){
        C = C + 1;
    }
    if(PORTBbits.RB3 == 1){ // Series of if statements to determine the sum of the B bits
        B = B + 8;
    }
    if(PORTBbits.RB2 == 1){
        B = B + 4;
    }
    if(PORTBbits.RB1 == 1){
        B = B + 2;
    }
    if(PORTBbits.RB0 == 1){
        B = B + 1;
    }
    Str_2[3] = '+';
    
    product = B * C; // Calculate the product
    
    if(product < 10){ // If statements to insert leading zeros
        Str_2[4] = '0';
        Str_2[5] = '0';
    }
    if(product < 100){
        Str_2[4] = '0';
    }
    if(product == 0){
        Str_2[4] = '0';
        Str_2[5] = '0';
        Str_2[6] = '0';
    }
    while(product > 0){ // Take the individual values of the product and add them to Str_2 for printing
        num = product % 10;
        Str_2[loc] = '0' + num;
        product = product / 10;
        loc--;
    }
    Print_To_LCD(Str_2);
}

/*******************************
 * Subtraction(void)
 * Print out the operation to be done on the top row of the LCD
 * Calculate the difference
 * Print the difference to the bottom row of the LCD
 ********************************/
void Subtraction(void)
{
    signed char diff = 0;
    char num = 0;
    char loc = 5;
    strcpy(Str_1, Clear1);
    strcpy(Str_2, Clear2);
    Str_1[2] = 'B';
    Str_1[3] = '-';
    Str_1[4] = 'C';
    Str_1[5] = '=';
    Str_2[3] = '+';
    Print_To_LCD(Str_1); //Up to this point in the function the upper part of the LCD prints out the operation to be done and the lower prints '+'
    
    if(PORTCbits.RC3 == 1){ //Series of if statements to determine the difference of B and C
        diff = diff - 8;
    }
    if(PORTCbits.RC2 == 1){
        diff = diff - 4;
    }
    if(PORTCbits.RC1 == 1){
        diff = diff - 2;
    }
    if(PORTCbits.RC0 == 1){
        diff = diff - 1;
    }
    if(PORTBbits.RB3 == 1){
        diff = diff + 8;
    }
    if(PORTBbits.RB2 == 1){
        diff = diff + 4;
    }
    if(PORTBbits.RB1 == 1){
        diff = diff + 2;
    }
    if(PORTBbits.RB0 == 1){
        diff = diff + 1;
    }
    
    if(diff < 0){ // Check if the difference of the two numbers is negative if so change the sign in the output to '-' and make the difference positive
        Str_2[3] = '-';
        diff = diff * (-1);
    }
    if(diff < 10){ // If the difference is less than 10 put a zero in front of the single digit
        Str_2[4] = '0';
    }
    if(diff == 0){ // Print '00' if the difference is 0
        Str_2[4] = '0';
        Str_2[5] = '0';
    }
    
    while(diff > 0){ // Take the individual values of the difference and add them to Str_2 for printing
        num = diff % 10;
        Str_2[loc] = '0' + num;
        diff = diff / 10;
        loc--;
    }
    Print_To_LCD(Str_2);
}

/*******************************
 * BandC(void)
 * Print operation to be done of the first row of the LCD
 * Calculate the logical result of B and C
 * Print the result out the bottom row of the LCD
 ********************************/
void BandC(void)
{
    strcpy(Str_1, Clear1);
    strcpy(Str_2, Clear2);
    Str_1[2] = 'B';
    Str_1[3] = 'A';
    Str_1[4] = 'N';
    Str_1[5] = 'D';
    Str_1[6] = 'C';
    Print_To_LCD(Str_1); // Up to this point in the function the upper part of the LCD prints out the operation to be done
    
    if(PORTBbits.RB3 && PORTCbits.RC3){  // Series of if statements to compute the logical values of B && C
        Str_2[3] = '1';
    }
    else{
        Str_2[3] = '0';
    }
    if(PORTBbits.RB2 && PORTCbits.RC2){
        Str_2[4] = '1';
    }
    else{
        Str_2[4] = '0';
    }
    if(PORTBbits.RB1 && PORTCbits.RC1){
        Str_2[5] = '1';
    }
    else{
        Str_2[5] = '0';
    }
    if(PORTBbits.RB0 && PORTCbits.RC0){
        Str_2[6] = '1';
    }
    else{
        Str_2[6] = '0';
    }
    Print_To_LCD(Str_2); // Print results to bottom row of LCD
}

/*******************************
 * notB(void)
 * Print operation to be done of the first row of the LCD
 * Calculate the logical result of not B
 * Print the result on the bottom row of the LCD
 ********************************/
void notB(void)
{
    strcpy(Str_1, Clear1);
    Str_1[2] = 'N';
    Str_1[3] = 'O';
    Str_1[4] = 'T';
    Str_1[5] = '(';
    Str_1[6] = 'B';
    Str_1[7] = ')';
    Print_To_LCD(Str_1); //Up to this point in the function the upper part of the LCD prints out the operation to be done
    
    if(PORTBbits.RB3){ //Series of if statements to check what the pins for B are and find the results of not B
        Str_2[3] = '0';
    }
    else{
        Str_2[3] = '1';
    }
    if(PORTBbits.RB2){
        Str_2[4] = '0';
    }
    else{
        Str_2[4] = '1';
    }
    if(PORTBbits.RB1){
        Str_2[5] = '0';
    }
    else{
        Str_2[5] = '1';
    }
    if(PORTBbits.RB0){
        Str_2[6] = '0';
    }
    else{
        Str_2[6] = '1';
    }
    Print_To_LCD(Str_2); // Print the results of not B to the bottom row of the LCD
}
