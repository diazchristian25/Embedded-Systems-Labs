#include <xc.h>

double slope = 3.5;
double intercept = -2.1;
double Point = 0.0;
int i = 0 ;

void main()
{
    int diff = 0;
	
	// Initialize the ports
    ADCON1 = 0b10001110;            // Enable PORTA & PORTE digital I/O pins
    TRISA  = 0b11100001;            // Set I/O for PORTA
    TRISB  = 0b11111111;            // Set I/O for PORTB    
    TRISD  = 0b11111111;            // Set I/O for PORTD
	
    PORTA = 0x00;

	while(1)
	{
		for (i = 0; i < 10; i++)
		{
			diff = PORTB - PORTD;
			Point = slope * diff + intercept;
			
			if (Point > 0 )
			{
				PORTAbits.RA1 = 1;
				PORTAbits.RA2 = 0;		
			}
			else 
			{
				PORTAbits.RA1 = 0;
				PORTAbits.RA2 = 1;			
			}
		}
	}
}

