#ifndef _H_MPU_6050
#define _H_MPU_6050

#include <stdint.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////
//                         宏开关
////////////////////////////////////////////////////////////////

/* 若要覆盖相关宏，请在 'mpu6050_conf.h' 中进行定义 */
#if defined __has_include
#if __has_include("mpu6050_conf.h")
#include "mpu6050_conf.h"
#endif
#else
#include "mpu6050_conf.h"
#endif

/* IIC 写入时的地址，+1 为读取 */
#ifndef MPU6050_AD0_HIGH
#define MPU6050_DEVICE_ADDR 0xD0
#else
#define MPU6050_DEVICE_ADDR 0xD2
#endif

////////////////////////////////////////////////////////////////
//                 加计/陀螺相关寄存器及可用值
////////////////////////////////////////////////////////////////

/**
 *          |   ACCELEROMETER    |           GYROSCOPE
 * DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
 * ---------+-----------+--------+-----------+--------+-------------
 * 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
 * 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
 * 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
 * 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
 * 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
 * 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
 * 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
 * 7        |   -- Reserved --   |   -- Reserved --   | Reserved
*/

/* 陀螺仪采样率分频，典型值：0x07(125Hz), DLPF_CFG enable: 1KHz/DIV, disable: 8KHz/DIV */
#define MPU6050_SMPLRT_DIV_CONFIG 0x19

/* 低通滤波频率，典型值：0x06(5Hz) */
#define MPU6050_CONFIG 0x1A

/** 
 * 加速计自检、测量范围及高通滤波频率，典型值：0x01(2G，5Hz)
 * 
 * 0 = +/- 2g   16384 LSB
 * 1 = +/- 4g   8192  LSB
 * 2 = +/- 8g   4096  LSB
 * 3 = +/- 16g  2048  LSB
 * 
 * ACCEL_HPF | Filter Mode | Cut-off Frequency
 * ----------+-------------+------------------
 * 0         | Reset       | None
 * 1         | On          | 5Hz
 * 2         | On          | 2.5Hz
 * 3         | On          | 1.25Hz
 * 4         | On          | 0.63Hz
 * 7         | Hold        | None
*/
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_CONFIG_FS_2 0x00
#define MPU6050_ACCEL_CONFIG_FS_4 0x08
#define MPU6050_ACCEL_CONFIG_FS_8 0x10
#define MPU6050_ACCEL_CONFIG_FS_16 0x18
#define MPU6050_ACCEL_CONFIG_DHPF_None 0x00
#define MPU6050_ACCEL_CONFIG_DHPF_5Hz 0x01
#define MPU6050_ACCEL_CONFIG_DHPF_2_5Hz 0x02
#define MPU6050_ACCEL_CONFIG_DHPF_1_25Hz 0x03
#define MPU6050_ACCEL_CONFIG_DHPF_0_63Hz 0x04
#define MPU6050_ACCEL_CONFIG_DHPF_HOLD 0x07
#define MPU6050_ACCEL_CONFIG_TEST_FLAG 0xE0 // 300mg ~ 950mg

/* 输出寄存器（只读）*/
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40

/**
 * 陀螺仪自检及测量范围，典型值：0x18(2000deg/s)
 * 
 * FS_SEL | Full Scale Range   | LSB Sensitivity
 * -------+--------------------+----------------
 * 0      | +/- 250 degrees/s  | 131 LSB/deg/s
 * 1      | +/- 500 degrees/s  | 65.5 LSB/deg/s
 * 2      | +/- 1000 degrees/s | 32.8 LSB/deg/s
 * 3      | +/- 2000 degrees/s | 16.4 LSB/deg/s
*/
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_GYRO_CONFIG_FS_250 0x00
#define MPU6050_GYRO_CONFIG_FS_500 0x08
#define MPU6050_GYRO_CONFIG_FS_1000 0x10
#define MPU6050_GYRO_CONFIG_FS_2000 0x18
#define MPU6050_GYRO_CONFIG_TEST_FLAG 0xE0 // 10 ~ 105 deg/s

/* 输出寄存器（只读）*/
#define MPU6050_GYRO_XOUT_H 0x43
#define MPU6050_GYRO_XOUT_L 0x44
#define MPU6050_GYRO_YOUT_H 0x45
#define MPU6050_GYRO_YOUT_L 0x46
#define MPU6050_GYRO_ZOUT_H 0x47
#define MPU6050_GYRO_ZOUT_L 0x48

////////////////////////////////////////////////////////////////
//                         温度
////////////////////////////////////////////////////////////////

#define MPU6050_TEMP_OUT_H 0x41
#define MPU6050_TEMP_OUT_L 0x42

////////////////////////////////////////////////////////////////
//            自由落体/零运动检测相关寄存器及可用值
////////////////////////////////////////////////////////////////

#define MPU6050_FF_THR 0x1D // 自由落体检测阈值, 2mg/LSB
#define MPU6050_FF_DUR 0x1E // 自由落体检测持续时间，单位 ms

#define MPU6050_MOT_THR 0x1F // 运动检测阈值, 2mg/LSB
#define MPU6050_MOT_DUR 0x20 // 运动检测持续时间，单位 ms

#define MPU6050_MOT_STATUS 0x61       // 运动状态，只读
#define MPU6050_MOT_STATUS_NEG_X 0x80 // X 负轴
#define MPU6050_MOT_STATUS_POS_X 0x40 // X 正轴
#define MPU6050_MOT_STATUS_NEG_Y 0x20
#define MPU6050_MOT_STATUS_POS_Y 0x10
#define MPU6050_MOT_STATUS_NEG_Z 0x08
#define MPU6050_MOT_STATUS_POS_Z 0x04
#define MPU6050_MOT_STATUS_ZERO_MOT 0x01

#define MPU6050_MOT_DETECT_CTRL 0x69
#define MPU6050_MOT_DETECT_CTRL_ACCEL_DELAY_X(x) (((x)&0x03) << 4)
#define MPU6050_MOT_DETECT_CTRL_FF_COUNT_X(x) (((x)&0x03) << 2)
#define MPU6050_MOT_DETECT_CTRL_MOT_COUNT_X(x) ((x)&0x03)

// * 与自由落体或运动检测不同，零运动检测在首次检测到零运动和不再检测到零运动时都触发中断。
// * 当检测到零运动事件时，将在 MOT_DETECT_STATUS 寄存器(寄存器97)中显示零运动状态。
// * 当检测到从运动到零运动的条件时，状态位被设置为1。
// * 当检测到从零运动到运动的条件时，状态位被设置为0。

#define MPU6050_ZERO_MOT_THR 0x21 //零运动检测阈值
#define MPU6050_ZERO_MOT_DUR 0x22 //零运动检测持续时间，单位 ms

////////////////////////////////////////////////////////////////
//                         中断配置
////////////////////////////////////////////////////////////////

#define MPU6050_INT_PIN_CFG 0x37
#define MPU6050_INT_CTRL 0x38
#define MPU6050_INT_CTRL_FF_EN 0x80
#define MPU6050_INT_CTRL_MOT_EN 0x40
#define MPU6050_INT_CTRL_ZMOT_EN 0x20
#define MPU6050_INT_CTRL_FIFO_OV_EN 0x10
#define MPU6050_INT_CTRL_I2C_MST_INT_EN 0x08
#define MPU6050_INT_CTRL_DATA_RDY_EN 0x01

#define MPU6050_INT_STATUS 0x3A
#define MPU6050_INT_STATUS_FF_FLAG 0x80
#define MPU6050_INT_STATUS_MOT_FLAG 0x40
#define MPU6050_INT_STATUS_ZMOT_FLAG 0x20
#define MPU6050_INT_STATUS_FIFO_OV_FLAG 0x10
#define MPU6050_INT_STATUS_I2C_MST_FLAG 0x08
#define MPU6050_INT_STATUS_DATA_RDY_FLAG 0x01

////////////////////////////////////////////////////////////////
//                          FIFO
////////////////////////////////////////////////////////////////

#define MPU6050_FIFO_EN 0x23 // FIFO 缓冲区使能
#define MPU6050_FIFO_EN_TEMP 0x80
#define MPU6050_FIFO_EN_GYRO_X 0x40
#define MPU6050_FIFO_EN_GYRO_Y 0x20
#define MPU6050_FIFO_EN_GYRO_Z 0x10
#define MPU6050_FIFO_EN_ACCEL 0x08
#define MPU6050_FIFO_EN_SLV2 0x04
#define MPU6050_FIFO_EN_SLV1 0x02
#define MPU6050_FIFO_EN_SLV0 0x01

#define MPU6050_I2C_MST_CTRL 0x24
#define MPU6050_I2C_SLV0_ADDR 0x25
#define MPU6050_I2C_SLV0_REG 0x26
#define MPU6050_I2C_SLV0_CTRL 0x27
#define MPU6050_I2C_SLV1_ADDR 0x28
#define MPU6050_I2C_SLV1_REG 0x29
#define MPU6050_I2C_SLV1_CTRL 0x2A
#define MPU6050_I2C_SLV2_ADDR 0x2B
#define MPU6050_I2C_SLV2_REG 0x2C
#define MPU6050_I2C_SLV2_CTRL 0x2D
#define MPU6050_I2C_SLV3_ADDR 0x2E
#define MPU6050_I2C_SLV3_REG 0x2F
#define MPU6050_I2C_SLV3_CTRL 0x30
#define MPU6050_I2C_SLV4_ADDR 0x31
#define MPU6050_I2C_SLV4_REG 0x32
#define MPU6050_I2C_SLV4_DO 0x33
#define MPU6050_I2C_SLV4_CTRL 0x34
#define MPU6050_I2C_SLV4_DI 0x35
#define MPU6050_I2C_MST_STATUS 0x36

////////////////////////////////////////////////////////////////
//         external sensor data by the Slave 0 1 2 3
////////////////////////////////////////////////////////////////

#define MPU6050_EXT_SENS_DATA_00 0x49
#define MPU6050_EXT_SENS_DATA_01 0x4A
#define MPU6050_EXT_SENS_DATA_02 0x4B
#define MPU6050_EXT_SENS_DATA_03 0x4C
#define MPU6050_EXT_SENS_DATA_04 0x4D
#define MPU6050_EXT_SENS_DATA_05 0x4E
#define MPU6050_EXT_SENS_DATA_06 0x4F
#define MPU6050_EXT_SENS_DATA_07 0x50
#define MPU6050_EXT_SENS_DATA_08 0x51
#define MPU6050_EXT_SENS_DATA_09 0x52
#define MPU6050_EXT_SENS_DATA_10 0x53
#define MPU6050_EXT_SENS_DATA_11 0x54
#define MPU6050_EXT_SENS_DATA_12 0x55
#define MPU6050_EXT_SENS_DATA_13 0x56
#define MPU6050_EXT_SENS_DATA_14 0x57
#define MPU6050_EXT_SENS_DATA_15 0x58
#define MPU6050_EXT_SENS_DATA_16 0x59
#define MPU6050_EXT_SENS_DATA_17 0x5A
#define MPU6050_EXT_SENS_DATA_18 0x5B
#define MPU6050_EXT_SENS_DATA_19 0x5C
#define MPU6050_EXT_SENS_DATA_20 0x5D
#define MPU6050_EXT_SENS_DATA_21 0x5E
#define MPU6050_EXT_SENS_DATA_22 0x5F
#define MPU6050_EXT_SENS_DATA_23 0x60

/**
 * 电源管理，典型值：0x00(正常启用)
 * 
 * CLK_SEL | Clock Source
 * --------+--------------------------------------
 * 0       | Internal oscillator
 * 1       | PLL with X Gyro reference
 * 2       | PLL with Y Gyro reference
 * 3       | PLL with Z Gyro reference
 * 4       | PLL with external 32.768kHz reference
 * 5       | PLL with external 19.2MHz reference
 * 6       | Reserved
 * 7       | Stops the clock and keeps the timing generator in reset
*/
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_PWR1_NORMAL 0x00
#define MPU6050_PWR1_DEVICE_RESET 0x80
#define MPU6050_PWR1_SLEEP 0x40
#define MPU6050_PWR1_CYCLE 0x20
#define MPU6050_PWR1_TEMP_DIS 0x08
#define MPU6050_PWR1_CLKSEL_INTERNAL 0x00
#define MPU6050_PWR1_CLKSEL_PLL_GYRO_X 0x01
#define MPU6050_PWR1_CLKSEL_PLL_GYRO_Y 0x02
#define MPU6050_PWR1_CLKSEL_PLL_GYRO_Z 0x03
#define MPU6050_PWR1_CLKSEL_PLL_EXT_32768Hz 0x04
#define MPU6050_PWR1_CLKSEL_PLL_EXT_19200KHz 0x05
#define MPU6050_PWR1_CLKSEL_KEEP_RESET 0x07

////////////////////////////////////////////////////////////////
//                        other
////////////////////////////////////////////////////////////////

#define MPU6050_I2C_SLV0_DO 0x63
#define MPU6050_I2C_SLV1_DO 0x64
#define MPU6050_I2C_SLV2_DO 0x65
#define MPU6050_I2C_SLV3_DO 0x66
#define MPU6050_I2C_MST_DELAY_CTRL 0x67

#define MPU6050_SIGNAL_PATH_RESET 0x68

#define MPU6050_USER_CTRL 0x6A
#define MPU6050_USER_CTRL_FIFO_EN 0x40
#define MPU6050_USER_CTRL_I2C_MST_EN 0x20
#define MPU6050_USER_CTRL_I2C_IF_DIS 0x10
#define MPU6050_USER_CTRL_FIFO_RST 0x04
#define MPU6050_USER_CTRL_I2C_MST_RST 0x02
#define MPU6050_USER_CTRL_SIG_COND_RST 0x01

#define MPU6050_PWR_MGMT_2 0x6C
#define MPU6050_BANK_SEL 0x6D
#define MPU6050_MEM_START_ADDR 0x6E
#define MPU6050_MEM_R_W 0x6F
#define MPU6050_DMP_CFG_1 0x70
#define MPU6050_DMP_CFG_2 0x71
#define MPU6050_FIFO_COUNTH 0x72
#define MPU6050_FIFO_COUNTL 0x73
#define MPU6050_FIFO_R_W 0x74
#define MPU6050_WHO_AM_I 0x75// !< WHO_AM_I register identifies the device. Expected value is 0x68.

////////////////////////////////////////////////////////////////
//                       mpu6050 api
////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t dev_addr;
    bool (*read)(uint8_t reg_addr, uint8_t *dat);
    bool (*write)(uint8_t reg_addr, uint8_t dat);
    void (*delay)(uint16_t ms);
} mpu6050_drv_t;

typedef uint8_t *mpu6050_cmd_list_t[2];

#define X 0
#define Y 1
#define Z 2

typedef struct
{
    float temp;
    float accel[3];
    float angle[3];
} mpu6050_data_t;

bool mpu6050_init(mpu6050_drv_t *drv, mpu6050_cmd_list_t init_sequence, uint8_t len);

bool mpu6050_test();

void mpu6050_write_cmd(uint8_t regAddr, uint8_t val);

bool mpu6050_read_data(mpu6050_data_t *dat);

#ifdef __cplusplus
}
#endif

#endif
