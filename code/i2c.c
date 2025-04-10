#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

// Default I2C address for MPU6050 (AD0 pin low)
#define MPU6050_ADDR 0x68

// MPU6050 Register Addresses
#define MPU6050_REG_PWR_MGMT_1     0x6B
#define MPU6050_REG_ACCEL_XOUT_H   0x3B
#define MPU6050_REG_GYRO_XOUT_H    0x43

// Struct to represent the IMU
typedef struct {
    int i2c_instance;
    uint8_t addr;
} mpu6050_t;

// Initializes the MPU6050 by waking it up
int mpu6050_init(mpu6050_t *imu, int i2c_instance, uint8_t addr);

// Reads raw accelerometer data into ax, ay, az
int mpu6050_read_accel(mpu6050_t *imu, int16_t *ax, int16_t *ay, int16_t *az);

// Reads raw gyroscope data into gx, gy, gz
int mpu6050_read_gyro(mpu6050_t *imu, int16_t *gx, int16_t *gy, int16_t *gz);

#endif // MPU6050_H
