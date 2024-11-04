#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/structs/clocks.h"

#include "tt_driver.h"

//TT configuration port definetions
#define PIN_HK_CSB 1
#define PIN_HK_SCK 2
#define PIN_HK_SDI 3
#define PIN_HK_SDO 4
#define PIN_CTRL_ENA 6
#define PIN_CTRL_RES 7
#define PIN_CTRL_INC 8

//TT project clock/reset definetion
#define PIN_CLK 0
#define PIN_RST 3

void tt_project_init(uint16_t tt_project_number) {
    //Initialize GPIO for TT configuration port
    gpio_set_function(PIN_HK_CSB, GPIO_FUNC_SIO);
    gpio_set_function(PIN_HK_SCK,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_HK_SDI,  GPIO_FUNC_SIO);
    gpio_set_function(PIN_HK_SDO, GPIO_FUNC_SIO);
    gpio_set_function(PIN_CTRL_ENA, GPIO_FUNC_SIO);
    gpio_set_function(PIN_CTRL_RES,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CTRL_INC,  GPIO_FUNC_SIO);

    //Initialize GPIO0 for project clock
    gpio_set_function(PIN_CLK, GPIO_FUNC_PWM);

    //Set direction of Configuration port pins
    gpio_set_dir(PIN_HK_CSB, GPIO_OUT);
    gpio_put(PIN_HK_CSB, 1);

    gpio_set_dir(PIN_HK_SCK, GPIO_OUT);
    gpio_put(PIN_HK_CSB, 0);
    
    gpio_set_dir(PIN_HK_SDI, GPIO_OUT);
    gpio_put(PIN_HK_SDI, 0);

    gpio_set_dir(PIN_HK_SDO, GPIO_IN);
    
    gpio_set_dir(PIN_CTRL_ENA, GPIO_OUT);
    gpio_put(PIN_CTRL_ENA, 0);

    gpio_set_dir(PIN_CTRL_RES, GPIO_OUT);
    gpio_put(PIN_CTRL_RES, 1);
    
    gpio_set_dir(PIN_CTRL_INC, GPIO_OUT);
    gpio_put(PIN_CTRL_INC, 0);

    //Select the project
    //To access TT/HH configuration port, CSB must be low
    gpio_put(PIN_HK_CSB, 0);
    //Reset the counter first
    gpio_put(PIN_CTRL_RES, 0);
    sleep_us(1);
    gpio_put(PIN_CTRL_RES, 1);
    sleep_us(1);
    //Send CTRL_INC pulses "tt_project_number" times
    for(uint16_t i = 0;i < tt_project_number;i++) {
        gpio_put(PIN_CTRL_INC, 1);
        sleep_us(1);
        gpio_put(PIN_CTRL_INC, 0);
        sleep_us(1);
    }
    //Enable the project
    gpio_put(PIN_CTRL_ENA, 1);

    //Release the configuration port
    gpio_put(PIN_HK_CSB, 1);

}

void tt_project_set_clock(uint32_t clock_frequency) {
    //GPIO0 is used for the project clock
    pwm_config config = pwm_get_default_config();
    uint32_t master_clk = clock_get_hz(clk_peri);
    //calculate and round the divider
    uint32_t divider = (master_clk + clock_frequency/2) / clock_frequency;
    pwm_config_set_wrap(&config, divider - 1);
    pwm_init(0,&config,true);
    pwm_set_chan_level(0, PWM_CHAN_A, divider/2);
}

void tt_project_set_reset(bool reset) {
    gpio_put(PIN_RST, !reset);
}