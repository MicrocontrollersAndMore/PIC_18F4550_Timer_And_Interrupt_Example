// 18F4550TimerAndInterruptExample.c

// chip config ////////////////////////////////////////////////////////////////////////////////////
					// clock options, see 18F4550 data sheet figure 2-1 "clock diagram" for explanation
#pragma config PLLDIV = 5				// this does not matter since we are using the internal clock & not using USB, see data sheet fig. 2-1
#pragma config CPUDIV = OSC1_PLL2		// this does not matter since we are using the internal clock & not using USB, see data sheet fig. 2-1
#pragma config USBDIV = 2				// this does not matter since we are using the internal clock & not using USB, see data sheet fig. 2-1

										// internal clock, pin #14 (RA6) as I/O pin, pin #13 unused,
#pragma config FOSC = INTOSCIO_EC		// if OSCCON is left as default clock speed will be 1Mhz

					// now the other less confusing options . . .
#pragma config FCMEN = OFF				// fail-safe clock monitor disabled
#pragma config IESO = OFF				// internal / external osc switchover bit disabled
#pragma config PWRT = OFF				// power-up timer disabled
#pragma config BOR = OFF				// brown-out reset disabled in hardware & software
#pragma config BORV = 3					// brown-out reset voltage bits, does not matter since brown-out is disabled 
#pragma config VREGEN = OFF				// USB voltage regulator, would need to turn on if using internal USB voltage regulator for USB comm.
#pragma config WDT = OFF				// watchdog timer disabled
#pragma config WDTPS = 32768			// watchdog timer postscale, does not matter since watchdog timer is disabled
#pragma config CCP2MX = ON				// use RC1 (pin #16) as CCP2 MUX (this is the default pin for CCP2 MUX)
#pragma config PBADEN = OFF				// RB0, RB1, RB2, RB3, & RB4 are configured as digital I/O on reset
#pragma config LPT1OSC = OFF			// disable low-power option for timer 1 (timer 1 in regular mode)
#pragma config MCLRE = OFF				// master clear disabled, pin #1 is for VPP and / or RE3 use
#pragma config STVREN = ON				// stack full/underflow will cause reset
#pragma config LVP = OFF				// single-supply ICSP disabled
#pragma config ICPRT = OFF				// in-circuit debug/programming port (ICPORT) disabled, this feature is not available on 40 pin DIP package
#pragma config XINST = OFF				// instruction set extension and indexed addressing mode disabled (this is the default setting)
#pragma config DEBUG = OFF				// background debugger disabled, RA6 & RB7 configured as general purpose I/O pins
#pragma config CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF			// code protection bits off
#pragma config CPB = OFF				// boot block code protection off
#pragma config CPD = OFF				// data EEPROM code protection off
#pragma config WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF		// write protection bits off
#pragma config WRTC = OFF				// config registers write protection off
#pragma config WRTB = OFF				// boot block is not write protected
#pragma config WRTD = OFF				// data EEPROM is not write protected
#pragma config EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF	// table read protection bits off
#pragma config EBTRB = OFF				// boot block table read protection off

// include ////////////////////////////////////////////////////////////////////////////////////////
#include<p18f4550.h>
#include<timers.h>

// global variables ///////////////////////////////////////////////////////////////////////////////
int g_LED0IsOn = 0;
int g_LED1IsOn = 0;
int g_LED2IsOn = 0;
int g_LED3IsOn = 0;

// function prototypes ////////////////////////////////////////////////////////////////////////////
void high_ISR(void);
void yourHighPriorityInterruptCode(void);

void flipLED0(void);
void flipLED1(void);
void flipLED2(void);
void flipLED3(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma code HIGH_INTERRUPT_VECTOR = 0x000008
void high_ISR(void) {
	_asm goto yourHighPriorityInterruptCode _endasm
}
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma interrupt yourHighPriorityInterruptCode
void yourHighPriorityInterruptCode(void) {
	
	if(INTCONbits.TMR0IF == 1) {			// if timer 0 interrupt just occurred . . .
		INTCONbits.TMR0IF = 0;
		flipLED0();
		TMR0L = 30;
	}
	
	if(PIR1bits.TMR1IF == 1) {				// if timer 1 interrupt just occurred . . .
		PIR1bits.TMR1IF = 0;
		flipLED1();
	}
	
	if(PIR1bits.TMR2IF == 1) {				// if timer 2 interrupt just occurred . . .
		PIR1bits.TMR2IF = 0;
		flipLED2();
	}
	
	if(PIR2bits.TMR3IF == 1) {				// if timer 3 interrupt just occurred . . .
		PIR2bits.TMR3IF = 0;
		flipLED3();
	}
	
}
#pragma code

///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void) {
								// until we have configured timers and related functionality, turn timers off
	T0CONbits.TMR0ON = 0;			// timer 0 off
	T1CONbits.TMR1ON = 0;			// timer 1 off
	T2CONbits.TMR2ON = 0;			// timer 2 off
	T3CONbits.TMR3ON = 0;			// timer 3 off
	
	ADCON1bits.PCFG3 = 1;			// set all AN pins to digital I/O
	ADCON1bits.PCFG2 = 1;			//
	ADCON1bits.PCFG1 = 1;			//
	ADCON1bits.PCFG0 = 1;			//
	
	TRISBbits.RB3 = 0;				// config output pins - RB3:RB0
	TRISBbits.RB2 = 0;				//
	TRISBbits.RB1 = 0;				//
	TRISBbits.RB0 = 0;				//
	
	PORTBbits.RB3 = 0;				// init output pins to off - RB3:RB0
	PORTBbits.RB2 = 0;				//
	PORTBbits.RB1 = 0;				//
	PORTBbits.RB0 = 0;				//
	
	RCONbits.IPEN = 1;				// enable priority level on interrupts
	INTCONbits.GIE_GIEH = 1;		// enable high-priority interrupts
	
								// timer 0 config
	INTCONbits.TMR0IE = 1;			// enable timer 0 overflow interrupt
	INTCON2bits.TMR0IP = 1;			// set timer 0 overflow interrupt to high priority
	T0CONbits.T08BIT = 1;			// set timer 0 to 8 bits
	T0CONbits.T0CS = 0;				// use internal clock to increment timer 0
	T0CONbits.PSA = 0;				// use prescaler with timer 0
	
	T0CONbits.T0PS2 = 1;			// timer 0 prescaler 1:256
	T0CONbits.T0PS1 = 1;			//
	T0CONbits.T0PS0 = 1;			//
								// end timer 0 config
								
								// timer 1 config
	PIE1bits.TMR1IE = 1;			// enable timer 1 overflow interrupt
	IPR1bits.TMR1IP = 1;			// timer 1 overflow interrupt priority set to high
	
	T1CONbits.T1CKPS1 = 0;			// timer 1 prescale 1:1
	T1CONbits.T1CKPS0 = 0;			//
	
	T1CONbits.T1OSCEN = 0;			// turn off separate oscillator that is internal to timer 1
	T1CONbits.TMR1CS = 0;			// use internal clock to increment timer 1
								// end timer 1 config
								
								// timer 2 config
	T2CONbits.T2CKPS1 = 1;			// timer 2 prescale 1:16
	T2CONbits.T2CKPS0 = 1;			//
	
	T2CONbits.T2OUTPS3 = 1;			// timer 2 postscale 1:16
	T2CONbits.T2OUTPS2 = 1;			//
	T2CONbits.T2OUTPS1 = 1;			//
	T2CONbits.T2OUTPS0 = 1;			//
	
	PR2 = 200;						// set timer 2 period register
	
	PIE1bits.TMR2IE = 1;			// enable TMR2 to PR2 match interrupt
	
	IPR1bits.TMR2IP = 1;			// set TMR2 to PR2 match interrupt priority to high
								// end timer 2 config
	
								// timer 3 config
	PIE2bits.TMR3IE = 1;			// enable timer 3 overflow interrupt
	IPR2bits.TMR3IP = 1;			// set timer 3 overflow priority to high
	
	T3CONbits.T3CKPS1 = 0;			//timer 3 prescale 1:2
	T3CONbits.T3CKPS0 = 1;			//
	
	T3CONbits.TMR3CS = 0;			// use internal clock to increment timer 3
								// end timer 3 config
	
								// turn timers on
	T0CONbits.TMR0ON = 1;			// timer 0 on
	T1CONbits.TMR1ON = 1;			// timer 1 on
	T2CONbits.TMR2ON = 1;			// timer 2 on
	T3CONbits.TMR3ON = 1;			// timer 3 on
	
	while(1) { }				// loop here forever, let timers do their thing
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void flipLED0(void) {
	if(g_LED0IsOn == 0) {				// if LED0 is off . . .
		PORTBbits.RB0 = 1;				// turn LED0 on
		g_LED0IsOn = 1;					// and update global variable
	} else if(g_LED0IsOn == 1) {		// else if LED0 is on . . .
		PORTBbits.RB0 = 0;				// turn LED0 off
		g_LED0IsOn = 0;					// and update global variable
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void flipLED1(void) {
	if(g_LED1IsOn == 0) {			// if LED1 is off . . .
		PORTBbits.RB1 = 1;			// turn LED1 on
		g_LED1IsOn = 1;				// and update global var
	} else if(g_LED1IsOn == 1) {	// else if LED1 is on . . .
		PORTBbits.RB1 = 0;			// turn LED1 off
		g_LED1IsOn = 0;				// and update global var
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void flipLED2(void) {
	if(g_LED2IsOn == 0) {			// if LED2 is off . . .
		PORTBbits.RB2 = 1;			// turn LED2 on
		g_LED2IsOn = 1;				// and update global var
	} else if(g_LED2IsOn == 1) {	// else if LED2 is on . . .
		PORTBbits.RB2 = 0;			// turn LED2 off
		g_LED2IsOn = 0;				// and update global var
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void flipLED3(void) {
	if(g_LED3IsOn == 0) {			// if LED3 is off . . .
		PORTBbits.RB3 = 1;			// turn LED3 on
		g_LED3IsOn = 1;				// and update global var
	} else if(g_LED3IsOn == 1) {	// else if LED3 is on . . .
		PORTBbits.RB3 = 0;			// turn LED3 off
		g_LED3IsOn = 0;				// and update global var
	}
}


