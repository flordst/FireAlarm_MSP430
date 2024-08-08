#include <msp430.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "SSD1306.h"
#include "i2c.h"
#define OLED_PWR    BIT0

#define ANUMBER     19052019
#define MAX_COUNT   4.2e9

#define LONG_DELAY  __delay_cycles(100000)

char txtBlock[8] = "WARNING ";

volatile unsigned int ADC_value;
int flag=0;
void config_ADC(void);
void config_clk(void);
void config_timer(void);
void config_port(void);
void config_interuptIO(void);

void Out_handle(void);


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    config_port();
    config_clk();
    config_interuptIO();
    config_timer();
    config_ADC();
//    P1DIR = OLED_PWR;                           // P1.0 output, else input
//    P1OUT ^= OLED_PWR;                          // turn on power to display
//
//   ssd1306_init();                             // Initialize SSD1306 OLED
//   ssd1306_clearDisplay();
    while (1){

        __bis_SR_register(CPUOFF + GIE);
        Out_handle();

    }

}

void config_clk(void){

    DCOCTL=0;
    BCSCTL1=CALBC1_8MHZ;
    DCOCTL=CALDCO_8MHZ;

}

void config_ADC(void){

    ADC10CTL1 |= INCH_5 + ADC10SSEL_2  ;
    ADC10CTL0 |= SREF_0 + ADC10SHT_1 + ADC10ON ;
    ADC10AE0 |=BIT5;
}
void config_timer(void){
    TACTL = TASSEL_2 + MC_1 ;
    CCTL0 = CCIE;
    CCR0 = 10000;
}

void config_port(void){
    P1DIR |=BIT2+BIT3 ;
    P1OUT &=~BIT3;
    P1OUT |=BIT2;

}

#pragma vector =TIMER0_A0_VECTOR
__interrupt void Timer_ISR(void){

    ADC10CTL0 &= ~ENC;
    ADC10CTL0 |= ENC + ADC10SC;
    while (ADC10CTL1 & ADC10BUSY);
     ADC_value=ADC10MEM;
    __bic_SR_register_on_exit(CPUOFF);

}
//#pragma vector=ADC10_VECTOR
//__interrupt void ADC10_ISR(void)
//{
////    while (ADC10CTL1 & ADC10BUSY);
////     ADC_value=ADC10MEM;
//    __bic_SR_register_on_exit(CPUOFF); // Exit LPM0
//}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
 {
   flag=1;


   P2IFG &= ~BIT5;                           // P1.0 IFG cleared

 }
void config_interuptIO(void){
    P2DIR &=~BIT5;                          //set P2.0 input
    P2REN |=BIT5;
    P2OUT &=~BIT5;

    P2IE |=BIT5;
    P2IES &= ~BIT5;
}

void Out_handle(void){

    if ((ADC_value>250) || (P2IN & BIT5) || flag==1){
        P1OUT |=BIT3;
        P1OUT &= ~BIT2;
        //ssd1306_clearDisplay();
        //ssd1306_printText(0, 1, txtBlock);
        // LONG_DELAY;
        //
        flag=0;
         }
    else {
        P1OUT &=~BIT3;
        P1OUT |=BIT2;
        //        ssd1306_clearDisplay();
    }
}


