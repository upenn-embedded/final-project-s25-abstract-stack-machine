#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

#define IMU_ADDR 0x68
// "WHO_AM_I" in main will determine addr

void I2C_init() {
    TWBR0 = 0x48; // set the bit rate // 100kHz at 16MHz
    TWCR0 = (1 << TWEN); // enable TWI
}

void start_con() {
    // send the start condition
    TWCR0 = (1<<TWINT)| (1<<TWSTA)|(1<<TWEN);
    while(!(TWCR0 & (1<<TWINT))); // wait for the flag to be set
}

void send(uint8_t data) {
    // send any data onto the I2C bus (could be device address or data)
    TWDR0 = data;
    TWCR0 = (1<<TWINT) | (1<<TWEN);
    while (!(TWCR0 & (1<<TWINT)));
}

void stop_con() {
    TWCR0 = (1<<TWINT)| (1<<TWEN)|(1<<TWSTO);
}

void write_register(uint8_t reg, uint8_t data) {
    start_con(); // start condition
    send(IMU_ADDR << 1); // send device address + W
    send(reg); // send register address
    send(data); // send data to be written
    stop_con();
}

uint8_t read_register(uint8_t reg) {
    uint8_t data;

    start_con();                     // Send start condition
    send(0x68 << 1);            // Send device address + write
    send(reg);                      // Send register address

    start_con();                    // Repeated start
    send((0x68 << 1) | 1);      // Send device address + read

    TWCR0 = (1 << TWINT) | (1 << TWEN); // Trigger reception, NACK after 1 byte
    while (!(TWCR0 & (1 << TWINT)));    // Wait for reception

    data = TWDR0;

    stop_con();                     // Send stop condition
    return data;
}