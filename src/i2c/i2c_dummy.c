#include "i2c_api.h"

#include <stdint.h>

#define I2C_MAX_VALID_ADDR 0x7F

#define I2C_DUMMY_TXE_FLAG (1u << 7)
#define I2C_DUMMY_USART_BASE 0x40011000
#define I2C_DUMMY_USART_SR (*(volatile uint32_t *)(I2C_DUMMY_USART_BASE + 0x00))
#define I2C_DUMMY_USART_DR (*(volatile uint32_t *)(I2C_DUMMY_USART_BASE + 0x04))
#define I2C_DUMMY_MSG_SIZE 8

#define NUM_TO_HEX_DIGIT(x) ((x) > 9 ? (x) + 'A' - 10 : (x) + '0')
#define HEX_BASE 16


static uint8_t I2C_Dummy_Channel_Init_Status[I2C_CH_MAX_NUM] = {};


/**
 * @brief Writes a character to the QEMU-emulated USART1 interface.
 * @param c Character to be written.
 */
static void I2C_Dummy_Write_Char(char c)
{
    while (!(I2C_DUMMY_USART_SR & I2C_DUMMY_TXE_FLAG));
    I2C_DUMMY_USART_DR = c;
}


/**
 * @brief Writes a string to the QEMU-emulated USART1 interface.
 * @param str String to be written.
 */
static void I2C_Dummy_Write_Message(const char *str)
{
    while (*str)
    {
        I2C_Dummy_Write_Char(*str++);
    }
}


I2C_Error_t I2C_Init(const I2C_Ch_t i2c_ch, const I2C_Freq_t i2c_freq)
{
    if (i2c_ch >= I2C_CH_MAX_NUM) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (i2c_freq >= I2C_FREQ_MAX_NUM) {
        return I2C_ERR_BAD_PARAMS;
    }

    I2C_Dummy_Channel_Init_Status[i2c_ch] = 1;
    return I2C_ERR_SUCCESS;
}


I2C_Error_t I2C_Start(const I2C_Ch_t i2c_ch, const uint8_t i2c_addr, const I2C_Dir_t direction)
{
    char msg[I2C_DUMMY_MSG_SIZE] = "C     \n";

    if (i2c_ch >= I2C_CH_MAX_NUM) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (i2c_addr > I2C_MAX_VALID_ADDR) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (direction >= I2C_DIR_MAX_NUM) {
        return I2C_ERR_BAD_PARAMS;
    }

    if (!I2C_Dummy_Channel_Init_Status[i2c_ch]) {
        return I2C_ERR_BUS_ERR;
    }

    char direction_flag = (direction == I2C_DIR_READ) ? 'R' : 'W';

    uint8_t i = 1;
    msg[i++] = NUM_TO_HEX_DIGIT(i2c_ch);
    msg[i++] = 'S';
    msg[i++] = NUM_TO_HEX_DIGIT(i2c_addr / HEX_BASE);
    msg[i++] = NUM_TO_HEX_DIGIT(i2c_addr % HEX_BASE);
    msg[i++] = direction_flag;

    I2C_Dummy_Write_Message(msg);

    return I2C_ERR_SUCCESS;
}


I2C_Error_t I2C_Stop(const I2C_Ch_t i2c_ch)
{
    char msg[I2C_DUMMY_MSG_SIZE] = "C     \n";

    if (i2c_ch >= I2C_CH_MAX_NUM) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (!I2C_Dummy_Channel_Init_Status[i2c_ch]) {
        return I2C_ERR_BUS_ERR;
    }

    uint8_t i = 1;
    msg[i++] = NUM_TO_HEX_DIGIT(i2c_ch);
    msg[i++] = 'E';

    I2C_Dummy_Write_Message(msg);

    return I2C_ERR_SUCCESS;
}


I2C_Error_t I2C_Read(const I2C_Ch_t i2c_ch, uint8_t* data_bytes, const uint16_t length)
{
    if (i2c_ch >= I2C_CH_MAX_NUM) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (!data_bytes || !length) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (!I2C_Dummy_Channel_Init_Status[i2c_ch]) {
        return I2C_ERR_BUS_ERR;
    }
    return I2C_ERR_SUCCESS;
}


I2C_Error_t I2C_Write(const I2C_Ch_t i2c_ch, const uint8_t* data_bytes, const uint16_t length)
{
    char msg[I2C_DUMMY_MSG_SIZE] = "C     \n";

    if (i2c_ch >= I2C_CH_MAX_NUM) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (!data_bytes) {
        return I2C_ERR_BAD_PARAMS;
    }
    if (!I2C_Dummy_Channel_Init_Status[i2c_ch]) {
        return I2C_ERR_BUS_ERR;
    }

    for (uint16_t i = 0; i < length; i++) {
        uint8_t j = 1;
        msg[j++] = NUM_TO_HEX_DIGIT(i2c_ch);
        msg[j++] = 'W';
        msg[j++] = NUM_TO_HEX_DIGIT(data_bytes[i] / HEX_BASE);
        msg[j++] = NUM_TO_HEX_DIGIT(data_bytes[i] % HEX_BASE);

        I2C_Dummy_Write_Message(msg);
    }

    return I2C_ERR_SUCCESS;
}
