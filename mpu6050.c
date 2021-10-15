#include "mpu6050.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

static mpu6050_drv_t *_drv = NULL;

void mpu6050_init(mpu6050_drv_t *drv, mpu6050_cmd_list_t init_sequence, uint8_t len)
{
    uint8_t i;

    /* save drv */
    _drv = drv;

    /* init by default conf */
    _drv->write(MPU6050_PWR_MGMT_1, MPU6050_PWR1_NORMAL);
    _drv->write(MPU6050_SMPLRT_DIV_CONFIG, 0x00);
    _drv->write(MPU6050_CONFIG, 0x06);
    _drv->write(MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_CONFIG_FS_16);
    _drv->write(MPU6050_GYRO_CONFIG, MPU6050_GYRO_CONFIG_FS_2000);

    /* write user init sequence */
    if (init_sequence != NULL)
    {
        for (i = 0; i < len; i++)
        {
            _drv->write(init_sequence[i][0], init_sequence[i][1]);
        }
    }
}

bool mpu6050_test()
{
    mpu6050_data_t base_data, test_data;

    bool test_passed = true;
    uint8_t i;
    uint8_t accel_conf, gyro_conf;
    int16_t temp;

    accel_conf = _drv->read(MPU6050_ACCEL_CONFIG);
    gyro_conf = _drv->read(MPU6050_GYRO_CONFIG);

    /* read base data */
    _drv->write(MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_CONFIG_FS_8);
    _drv->write(MPU6050_GYRO_CONFIG, MPU6050_GYRO_CONFIG_FS_250);
    _drv->delay(100);
    mpu6050_read_data(&base_data);

    /* read test data */
    _drv->write(MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_CONFIG_FS_8 | MPU6050_ACCEL_CONFIG_TEST_FLAG);
    _drv->write(MPU6050_GYRO_CONFIG, MPU6050_GYRO_CONFIG_FS_250 | MPU6050_GYRO_CONFIG_TEST_FLAG);
    _drv->delay(100);
    mpu6050_read_data(&test_data);

    /* test gyro */
    for (i = 0; i < 3; i++)
    {
        temp = (test_data.angle[i] - base_data.angle[i]) * 1000.0f;

        if (temp < 300U || temp > 950U)
        {
            test_passed = false;
            break;
        }
    }

    /* test accel */
    if (test_passed)
    {
        for (i = 0; i < 3; i++)
        {
            temp = test_data.accel[i] - base_data.accel[i];

            if (temp < 10U || temp > 105U)
            {
                test_passed = false;
                break;
            }
        }
    }

    // recovery old conf
    _drv->write(MPU6050_ACCEL_CONFIG, accel_conf);
    _drv->write(MPU6050_GYRO_CONFIG, gyro_conf);

    return test_passed;
}

static float _shift_right_f(float value, uint16_t num)
{
    float tmp = value;

    for (uint16_t i = 0; i < num; i++)
        tmp /= 2;

    return tmp;
}

mpu6050_data_t *mpu6050_read_data(mpu6050_data_t *dat)
{
    int16_t tmp;
    float unit;

    /* temperature */
    tmp = _drv->read(MPU6050_TEMP_OUT_H);
    tmp = (tmp << 8) | _drv->read(MPU6050_TEMP_OUT_L);
    dat->temp = 35.0f + (tmp / 340.0f); // 36.53f + res / 340.0f;

    /* accel */
    unit = (float)(16384 >> ((_drv->read(MPU6050_ACCEL_CONFIG) >> 3) & 0x03));
    dat->accel[X] = (((int16_t)_drv->read(MPU6050_ACCEL_XOUT_H) << 8) | _drv->read(MPU6050_ACCEL_XOUT_L)) / unit;
    dat->accel[Y] = (((int16_t)_drv->read(MPU6050_ACCEL_YOUT_H) << 8) | _drv->read(MPU6050_ACCEL_YOUT_L)) / unit;
    dat->accel[Z] = (((int16_t)_drv->read(MPU6050_ACCEL_ZOUT_H) << 8) | _drv->read(MPU6050_ACCEL_ZOUT_L)) / unit;

    /* gyro */
    unit = _shift_right_f(131, (_drv->read(MPU6050_GYRO_CONFIG) >> 3) & 0x03);
    dat->angle[X] = (((int16_t)_drv->read(MPU6050_GYRO_XOUT_H) << 8) | _drv->read(MPU6050_GYRO_XOUT_L)) / unit;
    dat->angle[Y] = (((int16_t)_drv->read(MPU6050_GYRO_YOUT_H) << 8) | _drv->read(MPU6050_GYRO_YOUT_L)) / unit;
    dat->angle[Z] = (((int16_t)_drv->read(MPU6050_GYRO_ZOUT_H) << 8) | _drv->read(MPU6050_GYRO_ZOUT_L)) / unit;

    return dat;
}

void mpu6050_write_cmd(uint8_t regAddr, uint8_t val)
{
    _drv->write(regAddr, val);
}
