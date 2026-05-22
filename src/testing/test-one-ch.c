#include "i2c/i2c_api.h"
#include "led_control/led_control.h"

#define LED_ADDR 0x78

int main(void)
{
    uint8_t id;
    LED_GetDeviceId(&id, I2C_CH0, LED_ADDR);
    LED_InitBus(I2C_CH0, I2C_FREQ_400_KHZ);
    LED_InitDevice(id, LED_CUR_IMAX, LED_PWM_3_KHZ);

    LED_SetChannelState(id, LED_CH4, LED_CH_STATE_ON);
    LED_SetChannelBrightness(id, LED_CH4, 255);

    while (1)
    {
        ;
    }
}
