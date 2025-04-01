/* 
 * File:   main.c
 * Author: mcnic
 *
 * Created on March 31, 2025, 5:26 PM
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 #include "uart.h"
 
 #define F_CPU 16000000UL
 
 volatile int finger_adcs[5] = {0, 0, 0, 0, 0};
 volatile int curr_finger;
 
 void Initialize() {
     
     // Setup for ADC (10bit = 0-1023)
     // Clear power reduction bit for ADC
     PRR0 &= ~(1 << PRADC);
 
     // Select Vref = 1.1 V
     ADMUX &= ~(1 << REFS0);
     ADMUX |= (1 << REFS1);
 
     // Set the ADC clock div by 128
     // 16M/128=125kHz
     ADCSRA |= (1 << ADPS0);
     ADCSRA |= (1 << ADPS1);
     ADCSRA |= (1 << ADPS2);
 
     // Select Channel ADC0 (pin C0)
     ADMUX &= ~(1 << MUX0);
     ADMUX &= ~(1 << MUX1);
     ADMUX &= ~(1 << MUX2);
     ADMUX &= ~(1 << MUX3);
     curr_finger = 0;
 
     ADCSRA |= (1 << ADATE); // Autotriggering of ADC
 
     // Free running mode ADTS[2:0] = 000
     ADCSRB &= ~(1 << ADTS0);
     ADCSRB &= ~(1 << ADTS1);
     ADCSRB &= ~(1 << ADTS2);
     
     ADCSRA |= (1 << ADIE); // Enable ADC Interrupt
 
     DIDR0 |= (1 << ADC0D); // Disable digital input buffer on ADC pin
     ADCSRA |= (1 << ADEN); // Enable ADC
     ADCSRA |= (1 << ADSC); // Start conversion
     
     sei();
     
 }
 
 void select_channel(int finger) {
 
     switch (finger) {
         case 0:
             ADMUX &= ~(1 << MUX0);
             ADMUX &= ~(1 << MUX1);
             ADMUX &= ~(1 << MUX2);
             ADMUX &= ~(1 << MUX3);
             break;
         case 1:
             ADMUX |= (1 << MUX0);
             ADMUX &= ~(1 << MUX1);
             ADMUX &= ~(1 << MUX2);
             ADMUX &= ~(1 << MUX3);
             break;
         case 2:
             ADMUX &= ~(1 << MUX0);
             ADMUX |= (1 << MUX1);
             ADMUX &= ~(1 << MUX2);
             ADMUX &= ~(1 << MUX3);
             break;
         case 3:
             ADMUX |= (1 << MUX0);
             ADMUX |= (1 << MUX1);
             ADMUX &= ~(1 << MUX2);
             ADMUX &= ~(1 << MUX3);
             break;
         case 4:
             ADMUX &= ~(1 << MUX0);
             ADMUX &= ~(1 << MUX1);
             ADMUX |= (1 << MUX2);
             ADMUX &= ~(1 << MUX3);
             break;
         default: return;
     }
 
 }
 
 ISR(ADC_vect) {
 
     finger_adcs[curr_finger] = ADC;
     curr_finger = (curr_finger + 1) % 5;
     select_channel(curr_finger);
 
     // Hi Aarti ! This is Sydney. I am writing on your laptop because you said I could.
 }
 
 /*
  * 
  */
 int main() {
     
     Initialize();
     uart_init();
     
     printf("Start\n");
     
     while(1) {
         
         for (int i = 0; i < 5; i++) {
             printf("Finger %d, ADC: %d\n", i, finger_adcs[i]);
         }
         _delay_ms(1000);
         
     }
 
     return (EXIT_SUCCESS);
 }
 
 