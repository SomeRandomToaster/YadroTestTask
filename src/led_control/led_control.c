#include "led_control.h"

#define LED_I2C_ADDR_AD_BITS 0x06
#define LED_I2C_VALID_ADDR_MASK 0x78
#define LED_I2C_MAX_FREQ I2C_FREQ_400_KHZ

#define LED_MAX_DEVICE_COUNT (4 * I2C_CH_MAX_NUM)

/** Enumeration of device registers and offsets inside them */
typedef enum
{
    LED_REG_SHUTDOWN = 0x00,
    LED_REG_PWM1 = 0x05,
    LED_REG_CTRL1 = 0x2A,
    LED_REG_GLOBAL_CTRL = 0x4A,
    LED_REG_CTRL_CUR_OFFSET = 1, // offset of current setting flags in LED control register
    LED_REG_PWM_FREQ = 0x4B,
    LED_REG_RESET = 0x4F
} LED_Register_t;


/**
 * @brief Flags for register read/write operation
 *
 * @var LED_REG_START - Start the bus communication and transmit the (first) register number
 * @var LED_REG_STOP - Stop the bus communication after the operation
 */
typedef enum
{
    LED_REGOP_START = 1U,
    LED_REGOP_STOP = 1U << 1
} LED_RegOpFlags_t;


/** @brief Enumeration possible device states */
typedef enum
{
    LED_DEV_STATE_UNKNOWN = 0U,
    LED_DEV_STATE_INIT = 1U
} LED_DeviceState_t;


/** @brief Structure to store the device address and its settings */
typedef struct
{
    I2C_Ch_t ch;
    uint8_t addr;
    LED_DeviceState_t state;
    LED_Current_t current;
} LED_Device_t;


I2C_Error_t LED_I2C_Errno = I2C_ERR_SUCCESS;

/** @brief Array of registered devices and its current size */
static LED_Device_t LED_Devices[LED_MAX_DEVICE_COUNT] = {};
static uint8_t LED_DeviceCount = 0;


/**
 * @brief Writes a byte to the device register
 *
 * @param device_id Device ID to work with.
 * @param reg_num Register number.
 * @param value Byte to write.
 * @param flags Register operation flags.
 * @return LED_Error_t Status of the operation.
 */
static LED_Error_t LED_WriteReg(uint8_t device_id, const LED_Register_t reg_num, uint8_t value, LED_RegOpFlags_t flags)
{
    if (device_id >= LED_DeviceCount) {
        return LED_ERR_BAD_PARAMS;
    }
    I2C_Ch_t ch = LED_Devices[device_id].ch;
    uint8_t addr = LED_Devices[device_id].addr;

    uint8_t msg;

    if (flags & LED_REGOP_START) {
        LED_I2C_Errno = I2C_Start(ch, addr, I2C_DIR_WRITE);
        if (LED_I2C_Errno != I2C_ERR_SUCCESS) {
            return LED_ERR_I2C_ERR;
        }

        // Transmit the register number
        msg = reg_num;
        LED_I2C_Errno = I2C_Write(ch, &msg, sizeof(msg));
        if (LED_I2C_Errno != I2C_ERR_SUCCESS) {
            return LED_ERR_I2C_ERR;
        }
    }

    // Transmit the byte
    msg = value;
    LED_I2C_Errno = I2C_Write(ch, &msg, sizeof(msg));
    if (LED_I2C_Errno != I2C_ERR_SUCCESS) {
        return LED_ERR_I2C_ERR;
    }

    if (flags & LED_REGOP_STOP) {
        LED_I2C_Errno = I2C_Stop(ch);
        if (LED_I2C_Errno != I2C_ERR_SUCCESS) {
            return LED_ERR_I2C_ERR;
        }
    }
    return LED_ERR_SUCCESS;
}

LED_Error_t LED_GetDeviceId(uint8_t* device_id, const I2C_Ch_t i2c_ch, uint8_t i2c_addr)
{
    if (!device_id) {
        return LED_ERR_BAD_PARAMS;
    }
    if (i2c_ch >= I2C_CH_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }
    if ((i2c_addr & ~LED_I2C_ADDR_AD_BITS) != LED_I2C_VALID_ADDR_MASK) {
        return LED_ERR_BAD_PARAMS;
    }

    // Search this device among added devices
    uint8_t id = LED_MAX_DEVICE_COUNT;
    for (uint8_t i = 0; i < LED_DeviceCount; i++) {
        if (LED_Devices[i].ch == i2c_ch && LED_Devices[i].addr == i2c_addr) {
            id = i; // It's already added'
            break;
        }
    }

    // Not found. Add a new device
    if (id == LED_MAX_DEVICE_COUNT) {
        id = LED_DeviceCount;
        LED_DeviceCount++;
        LED_Devices[id].ch = i2c_ch;
        LED_Devices[id].addr = i2c_addr;
        LED_Devices[id].state = LED_DEV_STATE_UNKNOWN;
    }
    *device_id = id;

    return LED_ERR_SUCCESS;
}


LED_Error_t LED_InitBus(const I2C_Ch_t i2c_ch, const I2C_Freq_t i2c_freq)
{
    if (i2c_ch >= I2C_CH_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }
    if (i2c_freq > LED_I2C_MAX_FREQ) {
        return LED_ERR_BAD_PARAMS;
    }

    LED_I2C_Errno = I2C_Init(i2c_ch, i2c_freq);
    if (LED_I2C_Errno != I2C_ERR_SUCCESS) {
        return LED_ERR_I2C_ERR;
    }
    return LED_ERR_SUCCESS;
}


LED_Error_t LED_InitDevice(uint8_t device_id, const LED_Current_t current, const LED_Freq_t pwm_freq)
{
    if (device_id >= LED_DeviceCount) {
        return LED_ERR_BAD_PARAMS;
    }
    if (current >= LED_CUR_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }
    if (pwm_freq >= LED_PWM_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }

    // Reset the device
    LED_Error_t error;
    if ((error = LED_WriteReg(device_id, LED_REG_RESET, 0, LED_REGOP_START | LED_REGOP_STOP)) != LED_ERR_SUCCESS) {
        return error;
    }
    LED_Devices[device_id].state = LED_DEV_STATE_INIT;

    // Turn off software shutdown
    if ((error = LED_WriteReg(device_id, LED_REG_SHUTDOWN, 1, LED_REGOP_START | LED_REGOP_STOP)) != LED_ERR_SUCCESS) {
        return error;
    }

    // Set PWM frequency
    if (pwm_freq != LED_PWM_3_KHZ) {
        if ((error = LED_WriteReg(device_id, LED_REG_PWM_FREQ, pwm_freq, LED_REGOP_START | LED_REGOP_STOP)) != LED_ERR_SUCCESS) {
            return error;
        }
    }

    // Set current for every channel (while keeping them shut down)

    LED_Devices[device_id].current = current;
    if (current != LED_CUR_IMAX) {
        for (uint8_t i = 0; i < LED_CH_MAX_NUM; i++) {
            uint8_t flags = 0;
            if (i == 0) {
                flags = LED_REGOP_START;
            } else if (i == LED_CH_MAX_NUM - 1) {
                flags = LED_REGOP_STOP;
            }
            if ((error = LED_WriteReg(device_id, LED_REG_CTRL1 + i, current << LED_REG_CTRL_CUR_OFFSET, flags)) != LED_ERR_SUCCESS) {
                return error;
            }
        }
    }

    return LED_ERR_SUCCESS;
}


LED_Error_t LED_SetChannelBrightness(uint8_t device_id, const LED_Ch_t channel, uint8_t brightness)
{
    if (device_id >= LED_DeviceCount) {
        return LED_ERR_BAD_PARAMS;
    }
    if (channel >= LED_CH_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }

    if (LED_Devices[device_id].state == LED_DEV_STATE_UNKNOWN) {
        return LED_ERR_NO_INIT;
    }

    LED_Error_t error;
    if ((error = LED_WriteReg(device_id, LED_REG_PWM1 + channel, brightness, LED_REGOP_START | LED_REGOP_STOP)) != LED_ERR_SUCCESS) {
        return error;
    }
    return LED_ERR_SUCCESS;
}


LED_Error_t LED_SetChannelState(uint8_t device_id, const LED_Ch_t channel, const LED_ChState_t state)
{
    if (device_id >= LED_DeviceCount) {
        return LED_ERR_BAD_PARAMS;
    }
    if (channel >= LED_CH_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }
    if (state >= LED_CH_STATE_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }

    if (LED_Devices[device_id].state == LED_DEV_STATE_UNKNOWN) {
        return LED_ERR_NO_INIT;
    }

    uint8_t value = (LED_Devices[device_id].current << LED_REG_CTRL_CUR_OFFSET) | state;
    LED_Error_t error;
    if ((error = LED_WriteReg(device_id, LED_REG_CTRL1 + channel, value, LED_REGOP_START | LED_REGOP_STOP)) != LED_ERR_SUCCESS) {
        return error;
    }

    return LED_ERR_SUCCESS;
}


LED_Error_t LED_SetAllChannelsState(uint8_t device_id, const LED_ChState_t state)
{
    if (device_id >= LED_DeviceCount) {
        return LED_ERR_BAD_PARAMS;
    }
    if (state >= LED_CH_STATE_MAX_NUM) {
        return LED_ERR_BAD_PARAMS;
    }

    LED_Error_t error;
    if ((error = LED_WriteReg(device_id, LED_REG_GLOBAL_CTRL, !state, LED_REGOP_START | LED_REGOP_STOP)) != LED_ERR_SUCCESS) {
        return error;
    }
    return LED_ERR_SUCCESS;
}


LED_Error_t LED_SetColor(uint8_t device_id, uint8_t red, uint8_t green, uint8_t blue)
{
    if (device_id >= LED_DeviceCount) {
        return LED_ERR_BAD_PARAMS;
    }

    if (LED_Devices[device_id].state == LED_DEV_STATE_UNKNOWN) {
        return LED_ERR_NO_INIT;
    }

    LED_Error_t error;
    if ((error = LED_WriteReg(device_id, LED_REG_PWM1 + LED_CH_R, red, LED_REGOP_START)) != LED_ERR_SUCCESS) {
        return error;
    }
    if ((error = LED_WriteReg(device_id, LED_REG_PWM1 + LED_CH_G, green, 0)) != LED_ERR_SUCCESS) {
        return error;
    }
    if ((error = LED_WriteReg(device_id, LED_REG_PWM1 + LED_CH_B, blue, LED_REGOP_STOP)) != LED_ERR_SUCCESS) {
        return error;
    }
    return LED_ERR_SUCCESS;
}
