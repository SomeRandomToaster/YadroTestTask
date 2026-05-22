#include "i2c/i2c_api.h"
#include "led_control/led_control.h"

#define LED_ADDR1 0x78
#define LED_ADDR2 0x7A

int main(void)
{
    uint8_t id1, id2;
    LED_GetDeviceId(&id1, I2C_CH0, LED_ADDR1);
    LED_GetDeviceId(&id2, I2C_CH0, LED_ADDR2);

    LED_InitBus(I2C_CH0, I2C_FREQ_400_KHZ);
    LED_InitDevice(id1, LED_CUR_IMAX, LED_PWM_3_KHZ);
    LED_InitDevice(id2, LED_CUR_IMAX, LED_PWM_3_KHZ);

    LED_SetChannelState(id1, LED_CH4, LED_CH_STATE_ON);
    LED_SetChannelBrightness(id1, LED_CH4, 255);

    LED_SetChannelState(id2, LED_CH2, LED_CH_STATE_ON);
    LED_SetChannelBrightness(id2, LED_CH2, 255);

    while (1)
    {
        ;
    }
}
