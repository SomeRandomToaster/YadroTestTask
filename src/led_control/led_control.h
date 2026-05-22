#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "i2c/i2c_api.h"

#include <stdint.h>


/**
 * @brief Enumeration of possible error codes returned by LED driver functions.
 * @var LED_ERR_SUCCESS - No error occurred. The operation completed successfully.
 * @var LED_ERR_I2C_ERROR - I2C transport-level error. I2C error code is stored in LED_I2C_Errno.
 * @var LED_ERR_NO_INIT - Operation can't be performed on uninitialized device.
 */
typedef enum
{
    LED_ERR_SUCCESS = (0U),
    LED_ERR_BAD_PARAMS = (1U),
    LED_ERR_I2C_ERR = (2U),
    LED_ERR_NO_INIT = (3U)
} LED_Error_t;


/** @brief Enumeration of LED output channels.*/
typedef enum
{
    LED_CH1 = (0U),
    LED_CH2 = (1U),
    LED_CH3 = (2U),
    LED_CH4 = (3U),
    LED_CH_R = (0U),
    LED_CH_G = (1U),
    LED_CH_B = (2U),
    LED_CH_W = (3U),
    LED_CH_MAX_NUM = (4U)
} LED_Ch_t;


/** @brief Enumeration of LED output channel states.*/
typedef enum
{
    LED_CH_STATE_OFF = (0U),
    LED_CH_STATE_ON = (1U),
    LED_CH_STATE_MAX_NUM = (2U)
} LED_ChState_t;


/** @brief Enumeration of output current settings.*/
typedef enum
{
    LED_CUR_IMAX = (0U),
    LED_CUR_IMAX_2 = (1U),
    LED_CUR_IMAX_3 = (2U),
    LED_CUR_IMAX_4 = (3U),
    LED_CUR_MAX_NUM = (4U)
} LED_Current_t;


/** @brief Enumeration of PWM frequency settings.*/
typedef enum
{
    LED_PWM_3_KHZ = (0U),
    LED_PWM_22_KHZ = (1U),
    LED_PWM_MAX_NUM = (2U)
} LED_Freq_t;


/** @brief Last I2C operation status */
extern I2C_Error_t LED_I2C_Errno;


/**
 * @brief Adds the device to the internal table and stores its ID in specified location.
 *
 * @description This function stores the device ID for further interaction with the driver.
 * If a device with this (channel, address) pair already exists in the table, the function
 * will return ID of the existing device. Othewise it will add a new device to the table.
 * On error the function doesn't touch the provided location.'
 *
 * @param device_id Location to store the ID.
 * @param i2c_ch I2C channel number of the device.
 * @param i2c_addr I2C address of the device.
 * @return LED_Error_t Status of the operation.
 */
LED_Error_t LED_GetDeviceId(uint8_t* device_id, const I2C_Ch_t i2c_ch, uint8_t i2c_addr);


/**
 * @brief Initializes the bus if its frequency is supported by the device.
 *
 * @param i2c_ch I2C channel number.
 * @param i2c_freq Frequency of the I2C bus.
 * @return LED_Error_t Status of the operation.
 */
LED_Error_t LED_InitBus(const I2C_Ch_t i2c_ch, const I2C_Freq_t i2c_freq);


/**
 * @brief Resets the device, turns off all LEDs, sets output current and PWM frequency.
 *
 * @param device_id Device ID to work with.
 * @param current Output current.
 * @param pwm_freq PWM output frequency.
 * @return LED_Error_t Status of the operation.
 */
LED_Error_t LED_InitDevice(uint8_t device_id, const LED_Current_t current, const LED_Freq_t pwm_freq);


/**
 * @brief Sets one channel brightness.
 *
 * @param device_id Device ID to work with.
 * @param channel Channel number.
 * @param brightness Brightness value (0-255).
 * @return LED_Error_t Status of the operation.
 */
LED_Error_t LED_SetChannelBrightness(uint8_t device_id, const LED_Ch_t channel, uint8_t brightness);


/**
 * @brief Turns on/off one channel.
 *
 * @param device_id Device ID to work with.
 * @param channel Channel number.
 * @param state Channel state.
 * @return LED_Error_t Status of the operation.
 */
LED_Error_t LED_SetChannelState(uint8_t device_id, const LED_Ch_t channel, const LED_ChState_t state);


/**
 * @brief Turns on/off all channels.
 *
 * @param device_id Device ID to work with.
 * @param state All channels state.
 * @return LED_Error_t Status of the operation.
 */
LED_Error_t LED_SetAllChannelsState(uint8_t device_id, const LED_ChState_t state);


/**
 * @brief Sets LED color using D1-D3 channels as R, G and B.
 *
 * @param device_id Device ID to work with.
 * @param red Red channel value (0-255).
 * @param green Green channel value (0-255).
 * @param blue Blue channel value (0-255).
 * @return LED_Error_t Status of the operation.
 */
LED_Error_t LED_SetColor(uint8_t device_id, uint8_t red, uint8_t green, uint8_t blue);


#endif /* LED_CONTROL_H */
