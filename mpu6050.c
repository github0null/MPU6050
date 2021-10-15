#include "mpu6050.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef ABS
#define ABS(x) ((x) >= 0) ? (x) : (-(x))
#endif

static mpu6050_drv_t *_drv = NULL;

bool mpu6050_init(mpu6050_drv_t *drv, mpu6050_cmd_list_t init_sequence, uint8_t len)
{
    uint8_t i;
    uint8_t who_am_i;

    /* save drv */
    _drv = drv;

    /* reset dev */
    _drv->write(MPU6050_PWR_MGMT_1, MPU6050_PWR1_DEVICE_RESET);
    _drv->delay(100);
    _drv->write(MPU6050_PWR_MGMT_1, MPU6050_PWR1_NORMAL);
    _drv->delay(100);

    /* read self addr */
    _drv->read(MPU6050_WHO_AM_I, &who_am_i);
    if (who_am_i != _drv->dev_addr) return false;

    /* init by default conf */
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

    _drv->write(MPU6050_PWR_MGMT_2, 0x00);

    return true;
}

bool mpu6050_test()
{
    mpu6050_data_t base_data, test_data;

    bool test_passed = true;
    uint8_t i;
    uint8_t accel_conf, gyro_conf;
    int16_t temp;

    _drv->read(MPU6050_ACCEL_CONFIG, &accel_conf);
    _drv->read(MPU6050_GYRO_CONFIG, &gyro_conf);

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

    /* test acce */
    for (i = 0; i < 3; i++)
    {
        temp = ABS((test_data.accel[i] - base_data.accel[i]) * 1000.0f);

        if (temp < 300U || temp > 950U)
        {
            test_passed = false;
            break;
        }
    }

    /* test gyro */
    if (test_passed)
    {
        for (i = 0; i < 3; i++)
        {
            temp = ABS(test_data.angle[i] - base_data.angle[i]);

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
    uint16_t i;

    for (i = 0; i < num; i++) tmp /= 2;

    return tmp;
}

bool mpu6050_read_data(mpu6050_data_t *dat)
{
#define DAT_REG_NUM 14

    uint8_t buf[DAT_REG_NUM];
    uint8_t addr, idx, tmp;
    int16_t tmp_val;

    float acce_unit, gyro_unit;

    /* read all reg */
    for (addr = MPU6050_ACCEL_XOUT_H, idx = 0;
         idx < DAT_REG_NUM;
         idx++, addr++)
    {
        if (_drv->read(addr, &buf[idx]) == false) return false;
    }

    /* get sensor unit */
    if (_drv->read(MPU6050_ACCEL_CONFIG, &tmp) == false) return false;
    acce_unit = (float)(16384 >> ((tmp >> 3) & 0x03));
    if (_drv->read(MPU6050_GYRO_CONFIG, &tmp) == false) return false;
    gyro_unit = _shift_right_f(131, (tmp >> 3) & 0x03);

#define DAT_ACCE_IDX 0
#define DAT_TEMP_IDX 6
#define DAT_GYRO_IDX 8

    /* accel */
    for (idx = 0, addr = 0; idx < 3; idx++, addr += 2)
    {
        tmp_val         = (buf[DAT_ACCE_IDX + addr + 0] << 8) | buf[DAT_ACCE_IDX + addr + 1];
        dat->accel[idx] = tmp_val / acce_unit;
    }

    /* temperature 36.53f + res / 340.0f */
    tmp_val   = (buf[DAT_TEMP_IDX + 0] << 8) | buf[DAT_TEMP_IDX + 1];
    dat->temp = 35.0f + (tmp_val / 340.0f);

    /* gyro */
    for (idx = 0, addr = 0; idx < 3; idx++, addr += 2)
    {
        tmp_val         = (buf[DAT_GYRO_IDX + addr + 0] << 8) | buf[DAT_GYRO_IDX + addr + 1];
        dat->angle[idx] = tmp_val / gyro_unit;
    }

    return true;
}

void mpu6050_write_cmd(uint8_t regAddr, uint8_t val)
{
    _drv->write(regAddr, val);
}
