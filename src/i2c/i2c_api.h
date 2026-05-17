#ifndef I2C_API_H
#define I2C_API_H


/** @brief Enumeration of supported I2C bus frequency modes. */
typedef enum
{
    I2C_FREQ_MAN = (0U),
    I2C_FREQ_50_KHZ = (1U),
    I2C_FREQ_100_KHZ = (2U),
    I2C_FREQ_400_KHZ = (3U),
    I2C_FREQ_1000_KHZ = (4U),
    I2C_FREQ_MAX_NUM = (5U)
} I2C_Freq_t;


/** @brief Enumeration of available I2C channels (ports) on the microcontroller. */
typedef enum
{
    I2C_CH0 = (0U),
    I2C_CH1 = (1U),
    I2C_CH2 = (2U),
    I2C_CH3 = (3U),
    I2C_CH_MAX_NUM = (4U)
} I2C_Ch_t;


/** @brief Enumeration defining the direction of I2C communication. */
typedef enum
{
    I2C_DIR_WRITE = (0U),
    I2C_DIR_READ = (1U),
    I2C_DIR_MAX_NUM = (2U)
} I2C_Dir_t;


/**
 * @brief Enumeration of possible error codes returned by I2C communication functions.
 *
 * @var I2C_ERR_SUCCESS - No error occurred. The operation completed successfully.
 * @var I2C_ERR_BAD_PARAMS - Invalid or out-of-range parameters were passed to the function.
 * @var I2C_ERR_TIME_OUT - Timeout occurred during the I2C transaction.
 * @var I2C_ERR_NACK - NACK (Not Acknowledged) was received from the slave device.
 * @var I2C_ERR_BUS_ERR - General I2C bus error occurred
 * @var I2C_ERR_DEVICE_ERR - Device-specific error was detected
 * @var I2C_ERR_CRITICAL - Critical system-level failure occurred.
 */
typedef enum
{
    I2C_ERR_SUCCESS = (0U),
    I2C_ERR_BAD_PARAMS = (1U),
    I2C_ERR_TIME_OUT = (2U),
    I2C_ERR_NACK = (3U),
    I2C_ERR_BUS_ERR = (4U),
    I2C_ERR_DEVICE_ERR = (5U),
    I2C_ERR_CRITICAL = (6U)
} I2C_Error_t;


/**
 * @brief Initializes the specified I2C channel with the given frequency.
 * @param i2c_ch I2C channel index.
 * @param i2c_freq I2C bus frequency configuration index.
 * @return I2C_Error_t Status of the operation.
 */
I2C_Error_t I2C_Init(const I2C_Ch_t i2c_ch, const I2C_Freq_t i2c_freq);


/**
 * @brief Generates a START condition and transmits the slave address.
 *
 * @description This function initiates the communication and sends the 7-bit address
 * followed by the R/W bit based on the specified direction.
 *
 * @param i2c_ch I2C channel index.
 * @param i2c_addr 7-bit I2C slave address.
 * @param direction Transfer direction (Read or Write).
 * @return I2C_Error_t Status of the operation.
 */
I2C_Error_t I2C_Start(const I2C_Ch_t i2c_ch, const BYTE i2c_addr, const I2C_Dir_t direction);


/**
 * @brief Generates a STOP condition to terminate the current transaction.
 * @param i2c_ch I2C channel index
 * @return I2C_Error_t Status of the operation.
 */
I2C_Error_t I2C_Stop(const I2C_Ch_t i2c_ch);


/**
 * @brief Reads data from the I2C slave device.
 * @param i2c_ch I2C channel index
 * @param data_bytes Pointer to buffer where received data will be stored
 * @param length Number of bytes to read (must be > 0)
 * @return I2C_Error_t Status of the operation.
 */
I2C_Error_t I2C_Read(const I2C_Ch_t i2c_ch, uint8_t* data_bytes, const uint16_t length);


/**
 * @brief Writes data to the I2C slave device.
 * @param i2c_ch I2C channel index
 * @param data_bytes Pointer to buffer containing data to send
 * @param length Number of bytes to write (must be > 0)
 * @return I2C_Error_t Status of the operation.
 */
I2C_Error_t I2C_Write(const I2C_Ch_t i2c_ch, const uint8_t* data_bytes, const uint16_t length);

#endif /* I2C_API_H */
