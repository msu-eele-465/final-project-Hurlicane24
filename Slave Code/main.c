#include "intrinsics.h"
#include "msp430fr2310.h"
#include <msp430.h>

#define SLAVE_ADDRESS 0x45
volatile char data[6] = {48, 48, 48, 48, 48, 48};
volatile int new_pattern = 0;
volatile int index = 0; 

void LEDBarSetup();
void setPattern(volatile char[]);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer 

    UCB0CTLW0 |= UCSWRST;

    P1SEL1 &= ~BIT3;
    P1SEL0 |= BIT3;
    
    P1SEL1 &= ~BIT2;
    P1SEL0 |= BIT2;

    UCB0CTLW0 &= ~UCTR;
    UCB0CTLW0 &= ~UCMST;
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;  

    UCB0I2COA0 = SLAVE_ADDRESS | UCOAEN;
    UCB0I2COA0 |= UCGCEN;

    UCB0CTLW0 &= ~UCSWRST;  
    PM5CTL0 &= ~LOCKLPM5;

    UCB0IE |= UCRXIE0;
    __enable_interrupt();  // Enable global interrupts

    LEDBarSetup();
    setPattern(data);
    P2DIR |= BIT7;

    while(1)
    {
        if(new_pattern == 1)
        {
            setPattern(data);
            new_pattern = 0;
        }
    }
    return(0);
}

void LEDBarSetup() {
    P1DIR |= BIT4;              //LED1
    P1DIR |= BIT5;              //LED2
    P1DIR |= BIT6;              //LED3
    P1DIR |= BIT7;              //LED4
    P2DIR |= BIT0;              //LED5
    P2DIR |= BIT6;              //LED6
}

void setPattern(volatile char pattern[])
{
    //LED bar 1
    if(pattern[0] == '1') {
        P1OUT |= BIT4;
    }
    else {
        P1OUT &= ~BIT4;
    }
    if(pattern[1] == '1') {
        P1OUT |= BIT5;
    }
    else {
        P1OUT &= ~BIT5;
    }
    if(pattern[2] == '1') {
        P1OUT |= BIT6;
    }
    else {
        P1OUT &= ~BIT6;
    }
    if(pattern[3] == '1') {
        P1OUT |= BIT7;
    }
    else {
        P1OUT &= ~BIT7;
    }
    if(pattern[4] == '1') {
        P2OUT |= BIT0;
    }
    else {
        P2OUT &= ~BIT0;
    }
    if(pattern[5] == '1') {
        P2OUT |= BIT6;
    }
    else {
        P2OUT &= ~BIT6;
    }
}

//Slave ISR
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_ISR(void)
{   
    data[index] = UCB0RXBUF;
    index++;
    if(index == 6) {
        new_pattern = 1;
        P2OUT ^= BIT7;
        index = 0;
    }
    UCB0IFG &= ~UCRXIFG;
}
