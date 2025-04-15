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
//#include "imu.h"
#include "i2c.h"

#define F_CPU 16000000UL
#define MPU6050_ADDR 0x68
#define WHO_AM_I 0x75 // Contains device address

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
 
volatile int finger_adcs[6] = {0, 0, 0, 0, 0, 0}; // drum is 0, fingers are 1-5
static int throwaway = 1;
volatile int curr_finger = 0;
volatile uint16_t drum_times[5] = {0, 0, 0, 0, 0};
volatile int drum_time_idx = 0;
volatile int overflows;
volatile uint16_t last_drum;
volatile uint16_t curr_drum;
volatile int bpm;
volatile int num_beats;
volatile int last_played[6] = {0};
volatile float volume = 50;

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

    //ADCSRA |= (1 << ADATE); // Autotriggering of ADC
    ADCSRA &= ~(1 << ADATE); // Disable auto-trigger

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
   
    DDRD |= (1 << PD1);
    PORTD |= (1 << PD1);

    DDRD |= (1 << PD3);
    PORTD |= (1 << PD3);

    DDRD |= (1 << PD4);
    PORTD |= (1 << PD4);

    DDRD |= (1 << PD5);
    PORTD |= (1 << PD5);

    DDRD |= (1 << PD7);
    PORTD |= (1 << PD7);
    
    DDRB |= (1 << PB4); // volume output
    PORTB |= (1 << PB4);
     
    sei();
     
 }
 
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
        case 4: // F, ADC6 aka PE2
            ADMUX &= ~(1 << MUX0);
            ADMUX |= (1 << MUX1);
            ADMUX |= (1 << MUX2);
            ADMUX &= ~(1 << MUX3);
            break;
        case 5: // G, ADC7 aka PE3
            ADMUX |= (1 << MUX0);
            ADMUX |= (1 << MUX1);
            ADMUX |= (1 << MUX2);
            ADMUX &= ~(1 << MUX3);
            break;
        default: return;
    }
 
 }
 
 ISR(ADC_vect) {
    finger_adcs[curr_finger] = ADC;
   
    curr_finger = (curr_finger + 1) % 6;
    select_channel(curr_finger);

    _delay_us(10);
    ADCSRA |= (1 << ADSC); // Start next conversion
 }

 void produce_sound(int finger) {
    if (finger==1) {
        //finger 1 will be C and the C file will be connected to T00 on the sound board which is connected to PD2
        PORTD &= ~(1 << PD1);
        _delay_ms(28000);
        PORTD |= (1 << PD1);

    }
    else if (finger==2)
    {
        //finger 2 will be D and the D file will be connected to T01 on the sound board which is connected to PD3
        PORTD &= ~(1 << PD3);
        _delay_ms(28000);
        PORTD |= (1 << PD3);
    }
    else if (finger==3) {
        //finger 3 will be E and the E file will be connected to T02 on the sound board which is connected to PD4
        PORTD &= ~(1 << PD4);
        _delay_ms(28000);
        PORTD |= (1 << PD4);
    }
    else if (finger==4) {
        //finger 4 will be F and the F file will be connected to T03 on the sound board which is connected to PD5
        PORTD &= ~(1 << PD5);
        _delay_ms(28000);
        PORTD |= (1 << PD5);
    }
    else if (finger==5) {
        //finger 5 will be G and the G file will be connected to T04 on the sound board which is connected to PD7
        PORTD &= ~(1 << PD7);
        _delay_ms(28000);
        PORTD |= (1 << PD7);
    }
    else {
        printf("Invalid finger number\n");
    }

}

uint16_t bpm_calc() {
   
    if (num_beats < 5) {
        return -1;
    }
   
    uint16_t total_ticks = 0;
    uint16_t time_diff;
    uint16_t average_ticks;
    float secs_per_beat;

    int prev_time = drum_times[0];
    int curr_time;
    for (int i = 1; i < 5; i++) {
        curr_time = drum_times[i];
        time_diff = curr_time - prev_time;
        total_ticks+=time_diff;
       
        prev_time = curr_time;
    }

    average_ticks = total_ticks/4;
    secs_per_beat = (float)(average_ticks)/15625.0;

    bpm = (uint16_t)(60.0/secs_per_beat);
    return bpm;
   
}

void make_drum_sound() {
   
    DDRD |= (1<<PD0);
   
   
    TCCR3A = (1 << COM3A1) | (1 << COM3B1) | (1 << WGM31);
    TCCR3B = (1 << WGM33) | (1 << CS30);  // No prescaler

    ICR3 = 65535;              // Max period
    OCR3A = ICR3 / 2;          // 50% duty cycle
    
    _delay_ms(200);
    
    TCCR3A &= ~((1 << COM3A1) | (1 << COM3B1));
    
    PORTD &= ~(1 << PD0); // Turn off PD0

   
   
}
   

 void drum(int val) {
 
    int adc_threshold = 500; // todo figure out value
    int drum_adc = val;
    if (drum_adc > adc_threshold) {
       
       
        if (num_beats < 5) {
           
            if (abs(TCNT1 - drum_times[num_beats-1]) < 2000) {
                return;
            }
           
            drum_times[drum_time_idx] = TCNT1;
            
            drum_time_idx = (drum_time_idx + 1) % 5;
           
        } else {
           
           
            if (abs(TCNT1 - drum_times[4]) < 2000) {
                return;
            }
           
            // shift
            for (int i = 0; i < 4; i++) {
                drum_times[i] = drum_times[i+1];
            }
           
            drum_times[4] = TCNT1;
            drum_time_idx = (drum_time_idx + 1) % 5;
           
        }
       
        num_beats++;
       
        make_drum_sound();
       
               
    }
   
 }
 
 void setPaintDisplay() {
     int xNote = LCD_WIDTH/2;
    int yNote = LCD_WIDTH/2;
    int textColor = BLACK;
    int backgroundColor = WHITE;
    char buffer[20];
    LCD_setScreen(WHITE);
    LCD_drawString(xNote-50, yNote-50, "Current Note:", textColor, backgroundColor);
    sprintf(buffer, "BPM:");
    LCD_drawString(LCD_WIDTH/2 - 50, LCD_WIDTH/2, buffer, textColor, backgroundColor);
 }


 void repaint(int finger) {
    int xNote = LCD_WIDTH/2;
    int yNote = LCD_WIDTH/2;
    int textColor = BLACK;
    int backgroundColor = WHITE;
    char buffer[20];
    switch(finger) {
        case 1:
            LCD_drawString(xNote+40, yNote-50, "C", textColor, backgroundColor);
            break;
        case 2:
            LCD_drawString(xNote+40, yNote-50, "D", textColor, backgroundColor);
            break;
        case 3:
            LCD_drawString(xNote+40, yNote-50, "E", textColor, backgroundColor);
            break;
        case 4:
            LCD_drawString(xNote+40, yNote-50, "F", textColor, backgroundColor);
            break;
        case 5:
            LCD_drawString(xNote+40, yNote-50, "G", textColor, backgroundColor);
            break;
        default: return;
    }
    char buf[4];
    sprintf(buf, "%d", (int)(volume));
    LCD_drawString(xNote, yNote-75, buf, textColor, backgroundColor);
    if (num_beats >= 5) {
        bpm_calc();
        sprintf(buffer, "%d", bpm);
        LCD_drawString(LCD_WIDTH/2 - 20, LCD_WIDTH/2, buffer, textColor, backgroundColor);
    }
 }
 
 /*
  * 
  */
 int main() {
    int imu_addr;
    uint8_t x_l;
    uint8_t x_h;
    int16_t x;
    uint8_t y_l;
    uint8_t y_h;
    int16_t y;
    uint8_t z_l;
    uint8_t z_h;
    int16_t z;

    int16_t xg;
    int16_t yg;
    int16_t zg;

    I2C_init();
     
     Initialize();
     uart_init();
     setPaintDisplay();
     
     int count = 0;
     
//     printf("Start\n");
     
     repaint(1);
     
     int last_played[6] = {0};
     
     imu_addr = read_register(WHO_AM_I);
    write_register(0x6B, 0x00); // Exit sleep mode
     
     while(1) {  
        int16_t z = (int16_t)(((uint16_t)read_register(ACCEL_ZOUT_H) << 8) | read_register(ACCEL_ZOUT_L));
//        float volume = ((float)(z + 16384) / (2 * 16384)) * 100.0;
//        if (volume < 0) volume = 0;
//        if (volume > 100) volume = 100;
        
        int16_t z_offset = 16384;

        // Calculate volume
        volume = ((float)(z + z_offset) / (2 * (float)z_offset)) * 100.0;

        // Clamp volume to a range of 0 to 100%
        if (volume < 0.0) {
            volume = 0;
        }
        if (volume > 100.0) {
            volume = 100;
        }
        
        if ((int) volume > 50) {
            DDRB &= ~(1 << PB4);
        }
        
        count++;
        if (count % 1000 == 0) {
            printf("z: %d\t", z);
            printf("volume: %d\n", (int)(volume));
        }
        
        
         for (int i = 0; i <= 5; i++) {
            int val = finger_adcs[i];
            if (i == 0) {
                repaint(i);
                drum(val);
                last_played[0] = 1;
            }
            else if (val < 800 && !last_played[i]) {
                printf("FINGER %d: %d\n", i, val);
                repaint(i);
                produce_sound(i);
                last_played[i] = 1;
            }
            else if (val >= 800) {
                last_played[i] = 0;
            }
         }
         
     }
     
     
 
    return (EXIT_SUCCESS);
 }
 
 