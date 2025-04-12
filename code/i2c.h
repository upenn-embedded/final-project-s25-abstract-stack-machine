/* 
 * File:   i2c.h
 *  simple i2c driver adapted for ESE 3500 WS3: Serial
 *  Note: this is an adaption of code I wrote previously and updated quickly
 *          to meet the needs of this assignment. If you are looking at this for
 *          your final project, note your project needs and particular i2c.c 
 *          implementation will drive what functions you need, trying to 
 *          recreate these functions may be insuffient, unecessary, or incorrect
 * 
 * Author: James Steeman
 * 
 * Created on November 17, 2024
 * Updated Feb 2025
 */

 #include <stdint.h>
 
 /* Function Declarations */

 void I2C_init();
 void start_con();
 void send(uint8_t data);
 void stop_con();
 void write_register(uint8_t reg, uint8_t data);
 uint8_t read_register(uint8_t reg);
 
