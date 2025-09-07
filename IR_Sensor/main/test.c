#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define LSA0 ADC1_CHANNEL_0

void app_main(void)
{
    adc1_config_channel_atten(LSA0,ADC_ATTEN_DB_11);
    adc1_config_width(ADC_BITWIDTH_9);          //to get values in range 0-255
    gpio_set_direction(19, GPIO_MODE_OUTPUT);
    
    while(1){

        int reading = adc1_get_raw(LSA0);
        printf("Reading : %d" , reading);
        gpio_set_level(19,1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        gpio_set_level(19,0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("\n");
    }

}
