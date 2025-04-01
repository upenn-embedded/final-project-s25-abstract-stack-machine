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

volatile int adc_val;

void Initialize() {
    
    // Setup for ADC (10bit = 0-1023)
    // Clear power reduction bit for ADC
    PRR0 &= ~(1 << PRADC);

    // Select Vref = 1.1 V
    ADMUX |= (1 << REFS0);
    ADMUX |= ~(1 << REFS1);
//    ADMUX |= (1 << 1) | (1 << 2) | (1 << 3);

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

ISR(ADC_vect) {
    adc_val = ADC;
}

/*
 * 
 */
int main() {
    
    Initialize();
    uart_init();
    
    printf("Start\n");
    
    while(1) {
        
        printf("FLEX! %d\n", adc_val);
        _delay_ms(1000);
        
    }

    return (EXIT_SUCCESS);
}

