#include "mpu6050.h"
#include "i2c.h" // assuming your RTOS includes this

static int16_t read_word(uint8_t *data) {
    return ((int16_t)data[0] << 8) | data[1];
}

int mpu6050_init(mpu6050_t *imu, int i2c_instance, uint8_t addr) {
    imu->i2c_instance = i2c_instance;
    imu->addr = addr;

    // Wake up the MPU6050 (clear sleep bit)
    uint8_t data[2] = {MPU6050_REG_PWR_MGMT_1, 0x00};
    return i2c_write(imu->i2c_instance, imu->addr, data, 2);
}

int mpu6050_read_accel(mpu6050_t *imu, int16_t *ax, int16_t *ay, int16_t *az) {
    uint8_t reg = MPU6050_REG_ACCEL_XOUT_H;
    uint8_t buf[6];
    if (i2c_write_read(imu->i2c_instance, imu->addr, &reg, 1, buf, 6) != 0) {
        return -1;
    }
    *ax = read_word(&buf[0]);
    *ay = read_word(&buf[2]);
    *az = read_word(&buf[4]);
    return 0;
}

int mpu6050_read_gyro(mpu6050_t *imu, int16_t *gx, int16_t *gy, int16_t *gz) {
    uint8_t reg = MPU6050_REG_GYRO_XOUT_H;
    uint8_t buf[6];
    if (i2c_write_read(imu->i2c_instance, imu->addr, &reg, 1, buf, 6) != 0) {
        return -1;
    }
    *gx = read_word(&buf[0]);
    *gy = read_word(&buf[2]);
    *gz = read_word(&buf[4]);
    return 0;
}