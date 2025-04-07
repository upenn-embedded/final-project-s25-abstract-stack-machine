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
#include "LCD_GFX.h"
#include "ST7735.h"

#define F_CPU 16000000UL
 
volatile int finger_adcs[6] = {0, 0, 0, 0, 0, 0}; // drum is 0, fingers are 1-5
static int throwaway = 1;
volatile int curr_finger;
volatile uint16_t drum_times[5] = {0, 0, 0, 0, 0};
volatile int drum_time_idx = 0;
volatile int overflows;
volatile uint16_t last_drum;
volatile uint16_t curr_drum;
volatile int bpm;
volatile int num_beats;

void init_adc() {
    
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

}

void drum_timer_init() {

   // Timer1, prescale 1024
   TCCR1B |= (1 << CS12) | (1 << CS10);
   TCCR1B &= ~(1 << CS11);
   
   // Set Timer 1 to Normal
   TCCR1A &= ~(1 << WGM10);
   TCCR1A &= ~(1 << WGM11);
   TCCR1B &= ~(1 << WGM12);
   
   TCNT1 = 0;
   overflows = 0;
   
   TIMSK1 |= (1 << TOIE1); // enable overflow interrupt

}

ISR(TIMER1_OVF_vect) {
    overflows++;
}
 
 void Initialize() {
     
    init_adc();
    lcd_init();
    drum_timer_init();

    DDRD |= (1 << PD2);
    PORTD &= ~(1 << PD2);

    DDRD |= (1 << PD3);
    PORTD &= ~(1 << PD3);

    DDRD |= (1 << PD4);
    PORTD &= ~(1 << PD4);

    DDRD |= (1 << PD5);
    PORTD &= ~(1 << PD5);

    DDRD |= (1 << PD7);
    PORTD &= ~(1 << PD7);
     
    sei();
     
 }

 // todo timer overflow interrupt !!!!!
 
 void select_channel(int finger) {
 
    switch (finger) {
        case 0: // drum, ADC0 aka PC0
            ADMUX &= ~(1 << MUX0);
            ADMUX &= ~(1 << MUX1);
            ADMUX &= ~(1 << MUX2);
            ADMUX &= ~(1 << MUX3);
            break;
        case 1: // C, ADC1 aka PC1
            ADMUX |= (1 << MUX0);
            ADMUX &= ~(1 << MUX1);
            ADMUX &= ~(1 << MUX2);
            ADMUX &= ~(1 << MUX3);
            break;
        case 2: // D, ADC2 aka PC2
            ADMUX &= ~(1 << MUX0);
            ADMUX |= (1 << MUX1);
            ADMUX &= ~(1 << MUX2);
            ADMUX &= ~(1 << MUX3);
            break;
        case 3: // E, ADC3 aka PC3
            ADMUX |= (1 << MUX0);
            ADMUX |= (1 << MUX1);
            ADMUX &= ~(1 << MUX2);
            ADMUX &= ~(1 << MUX3);
            break;
        case 4: // F, ADC4 aka PC4
            ADMUX &= ~(1 << MUX0);
            ADMUX &= ~(1 << MUX1);
            ADMUX |= (1 << MUX2);
            ADMUX &= ~(1 << MUX3);
            break;
        case 5: // G, ADC5 aka PC5
            ADMUX |= (1 << MUX0);
            ADMUX &= ~(1 << MUX1);
            ADMUX |= (1 << MUX2);
            ADMUX &= ~(1 << MUX3);
        default: return;
    }
 
 }
 
 ISR(ADC_vect) {

    if (throwaway) {
        throwaway = 0;
        return;
    }
 
     finger_adcs[curr_finger] = ADC;
     curr_finger = (curr_finger + 1) % 6;
     select_channel(curr_finger);
     throwaway = 1;
 
     // Hi Aarti ! This is Sydney. I am writing on your laptop because you said I could.
 }

 void produce_sound(int finger) {
    if (finger==1) {
        //finger 1 will be C and the C file will be connected to T00 on the sound board which is connected to PD2
        PORTD |= (1 << PD2);
        _delay_ms(100);
        PORTD &= ~(1 << PD2);

    }
    else if (finger==2)
    {
        //finger 2 will be D and the D file will be connected to T01 on the sound board which is connected to PD3
        PORTD |= (1 << PD3);
        _delay_ms(100);
        PORTD &= ~(1 << PD3);
    }
    else if (finger==3) {
        //finger 3 will be E and the E file will be connected to T02 on the sound board which is connected to PD4
        PORTD |= (1 << PD4);
        _delay_ms(100);
        PORTD &= ~(1 << PD4);
    }
    else if (finger==4) {
        //finger 4 will be F and the F file will be connected to T03 on the sound board which is connected to PD5
        PORTD |= (1 << PD5);
        _delay_ms(100);
        PORTD &= ~(1 << PD5);
    }
    else if (finger==5) {
        //finger 5 will be G and the G file will be connected to T04 on the sound board which is connected to PD7
        PORTD |= (1 << PD7);
        _delay_ms(100);
        PORTD &= ~(1 << PD7);
    }
    else {
        printf("Invalid finger number\n");
    }

}

uint16_t bpm_calc() {
    uint16_t total_ticks = 0;
    uint16_t time_diff;
    uint16_t average_ticks;
    float secs_per_beat;

    int prev_time = drum_times[0];
    int curr_time;
    for (int i = 1; i < 5; i++) {
        curr_time = drum_times[i];
        time_diff = curr_time - prev_time; // todo overflow !!!!
        // convert ticks to seconds
        // 1024 prescaler. 15625 hz.

//        if (time_diff < 0) {
//            time_diff+=65536;
//        }
        
        printf("time diff: %d\t", time_diff);

        total_ticks+=time_diff;
        
        prev_time = curr_time;
    }

    average_ticks = total_ticks/4;
    secs_per_beat = (float)(average_ticks)/15625.0;

    bpm = (uint16_t)(60.0/secs_per_beat);
    return bpm;
    
}
    

 void drum() {
 
    int adc_threshold = 500; // todo figure out value
    int drum_adc = finger_adcs[0];
    if (drum_adc > adc_threshold) {
        
        if (abs(TCNT1 - drum_times[(drum_time_idx - 1) % 5]) < 2000) {
//            printf("debounce");
            return;
        }

        num_beats++;
        drum_times[drum_time_idx] = TCNT1;
        printf("timer value: %d\n", drum_times[drum_time_idx]);
        drum_time_idx = (drum_time_idx + 1) % 5;

        //finger 0 will be drum and the drum file will be connected to T05 on the sound board which is connected to PE1
        PORTD |= (1 << PE1);
        _delay_ms(100);
        PORTD &= ~(1 << PE1);
        
    }
    
 }


 void repaint(int finger) {
    int xNote = LCD_WIDTH/2;
    int yNote = LCD_WIDTH/2;
    int textColor = BLACK;
    int backgroundColor = WHITE;
    char buffer[20];
    switch(finger) {
        case 1:
            LCD_drawString(xNote, yNote, "Current Note:  C", textColor, backgroundColor);
            break;
        case 2:
            LCD_drawString(xNote, yNote, "Current Note:  D", textColor, backgroundColor);
            break;
        case 3:
            LCD_drawString(xNote, yNote, "Current Note:  E", textColor, backgroundColor);
            break;
        case 4:
            LCD_drawString(xNote, yNote, "Current Note:  F", textColor, backgroundColor);
            break;
        case 5:
            LCD_drawString(xNote, yNote, "Current Note:  G", textColor, backgroundColor);
            break;
        default: return;
    }
    if (num_beats >= 5) {
        bpm_calc();
        sprintf(buffer, "BPM:  %d", bpm);
        LCD_drawString(LCD_WIDTH/2 - 50, LCD_WIDTH/2 - 50, buffer, textColor, backgroundColor);
    }
 }
 
 /*
  * 
  */
 int main() {
     
     Initialize();
     uart_init();
     lcd_init();
     
     printf("Start\n");
     
     int count = 0;
     
     while(1) {
         
        count++;
        drum();
         
         if (count % 10000 == 0) {
             printf("DRUM: %d\t", finger_adcs[0]);
            if (num_beats >= 5) {
                printf("BPM:  %d\n", bpm_calc());
            }
         }
         
//        printf("TCNT1: %u\n", TCNT1); // should be ~15625 (16MHz / 1024)

         
//        printf("FINGER: %d\n", finger_adcs[1]);
        
         
         
         
//         for (int i = 0; i <= 5; i++) {
//            if (i == 0) {
//                drum();
//            }
//            else if (finger_adcs[i] > 100) {
//                produce_sound(i);
//                repaint(i);
//            }
//         }
         
     }
 
     return (EXIT_SUCCESS);
 }
 
 