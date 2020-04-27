#include "mpu6050.h"

ReadByteCallBack _ReadByte;
WriteCmdCallBack _WriteCmd;

MPU6050_Data _offset[2];

#define _INDEX_ACCEL 0
#define _INDEX_GYRO 1
#define _CLEAR_OFFSET(index, _datPtr)   \
    (_datPtr)->x *= _offset[(index)].x; \
    (_datPtr)->y *= _offset[(index)].y; \
    (_datPtr)->z *= _offset[(index)].z

#define GetFieldValue(_datPtr, index) ((float *)(((float *)(_datPtr)) + (index)))

#define square(x) ((x) * (x))

void MPU6050_Init(ReadByteCallBack readByteCallBk, WriteCmdCallBack writeCmdCallBk)
{
    _ReadByte = readByteCallBk;
    _WriteCmd = writeCmdCallBk;

    _offset[_INDEX_ACCEL].x = _offset[_INDEX_ACCEL].y = _offset[_INDEX_ACCEL].z = 1.0f;
    _offset[_INDEX_GYRO].x = _offset[_INDEX_GYRO].y = _offset[_INDEX_GYRO].z = 1.0f;

    _WriteCmd(MPU6050_PWR_MGMT_1, MPU6050_PWR1_NORMAL);
    _WriteCmd(MPU6050_SMPLRT_DIV, 0x00);
    _WriteCmd(MPU6050_CONFIG, 0x06);
    _WriteCmd(MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_CONFIG_FS_4);
    _WriteCmd(MPU6050_GYRO_CONFIG, MPU6050_GYRO_CONFIG_FS_2000);
}

MPU6050_Data *Measure_Error(MPU6050_Data *val1, MPU6050_Data *val2)
{
    val1->x = val1->x > val2->x ? (val1->x - val2->x) : (val2->x - val1->x);
    val1->y = val1->y > val2->y ? (val1->y - val2->y) : (val2->y - val1->y);
    val1->z = val1->z > val2->z ? (val1->z - val2->z) : (val2->z - val1->z);
    return val1;
}

uint8_t MPU6050_Test(DelayMsCallBk _Delay)
{
    MPU6050_Data testBuf[2];
    uint8_t i, testFlag = 1, accelConfig, gyroConfig, delayLen;
    int16_t temp;

    accelConfig = _ReadByte(MPU6050_ACCEL_CONFIG);
    gyroConfig = _ReadByte(MPU6050_GYRO_CONFIG);
    delayLen = _ReadByte(MPU6050_SMPLRT_DIV) + 20;

    // test Accel
    _WriteCmd(MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_CONFIG_FS_8);
    _Delay(delayLen);

    MPU6050_GetAccelData(&testBuf[0]);

    _WriteCmd(MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_CONFIG_FS_8 | MPU6050_ACCEL_CONFIG_TEST_FLAG);
    _Delay(delayLen);

    MPU6050_GetAccelData(&testBuf[1]);
    Measure_Error(&testBuf[0], &testBuf[1]);

    i = 0;
    while (i < 3)
    {
        temp = *GetFieldValue(&testBuf[0], i) * 1000.0f;
        if (temp < 300U || temp > 950U)
        {
            testFlag = 0;
            break;
        }
        i++;
    }

    // test GYRO
    if (testFlag != 0)
    {
        _WriteCmd(MPU6050_GYRO_CONFIG, MPU6050_GYRO_CONFIG_FS_250);
        _Delay(delayLen);

        MPU6050_GetGyroData(&testBuf[0]);

        _WriteCmd(MPU6050_GYRO_CONFIG, MPU6050_GYRO_CONFIG_FS_250 | MPU6050_GYRO_CONFIG_TEST_FLAG);
        _Delay(delayLen);

        MPU6050_GetGyroData(&testBuf[1]);
        Measure_Error(&testBuf[0], &testBuf[1]);

        i = 0;
        while (i < 3)
        {
            temp = *GetFieldValue(&testBuf[0], i);
            if (temp < 10U || temp > 105U)
            {
                testFlag = 0;
                break;
            }
            i++;
        }
    }

    // reset
    _WriteCmd(MPU6050_ACCEL_CONFIG, accelConfig);
    _WriteCmd(MPU6050_GYRO_CONFIG, gyroConfig);
    _Delay(delayLen);

    return testFlag;
}

void MPU6050_Calibration(void)
{
    float vecLen;
    MPU6050_Data _data;
    MPU6050_GetAccelData(&_data);
    vecLen = sqrtf(square(_data.x) + square(_data.y) + square(_data.z));
    _offset[_INDEX_ACCEL].x = _offset[_INDEX_ACCEL].y = _offset[_INDEX_ACCEL].z = 1.0f / vecLen;
}

void MPU6050_WriteCmd(uint8_t regAddr, uint8_t val)
{
    _WriteCmd(regAddr, val);
}

MPU6050_Data *MPU6050_GetAccelData(MPU6050_Data *_dat)
{
    int16_t unit = 16384 / (1 << ((_ReadByte(MPU6050_ACCEL_CONFIG) >> 3) & 0x03));
    _dat->x = ((int16_t)((_ReadByte(MPU6050_ACCEL_XOUT_H) << 8) | _ReadByte(MPU6050_ACCEL_XOUT_L))) / (float)unit;
    _dat->y = ((int16_t)((_ReadByte(MPU6050_ACCEL_YOUT_H) << 8) | _ReadByte(MPU6050_ACCEL_YOUT_L))) / (float)unit;
    _dat->z = ((int16_t)((_ReadByte(MPU6050_ACCEL_ZOUT_H) << 8) | _ReadByte(MPU6050_ACCEL_ZOUT_L))) / (float)unit;
    _CLEAR_OFFSET(_INDEX_ACCEL, _dat);
    return _dat;
}

MPU6050_Data *MPU6050_GetGyroData(MPU6050_Data *_dat)
{
    float unit = 131.0f / (1 << ((_ReadByte(MPU6050_GYRO_CONFIG) >> 3) & 0x03));
    _dat->x = ((int16_t)((_ReadByte(MPU6050_GYRO_XOUT_H) << 8) | _ReadByte(MPU6050_GYRO_XOUT_L))) / unit;
    _dat->y = ((int16_t)((_ReadByte(MPU6050_GYRO_YOUT_H) << 8) | _ReadByte(MPU6050_GYRO_YOUT_L))) / unit;
    _dat->z = ((int16_t)((_ReadByte(MPU6050_GYRO_ZOUT_H) << 8) | _ReadByte(MPU6050_GYRO_ZOUT_L))) / unit;
    _CLEAR_OFFSET(_INDEX_GYRO, _dat);
    return _dat;
}

float MPU6050_GetTemperature(void)
{
    int16_t res = _ReadByte(MPU6050_TEMP_OUT_H);
    res = (res << 8) | _ReadByte(MPU6050_TEMP_OUT_L);
    return 35.0f + res / 340.0f; // 36.53f + res / 340.0f;
}
