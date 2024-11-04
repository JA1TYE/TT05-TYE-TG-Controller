#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#include "tye_tg_driver.h"

void tye_tg_init(void){
    // SPI initialisation
    // SCK = 1MHz
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
}

void tye_tg_set_frequency(uint8_t ch,float frequency) {
    //Convert frequency to ingrement value
    uint32_t increment = frequency * 0x10000 / TG_FSAMPLE;
    
    //Write the increment value to the TG
    tye_tg_set_increment(ch,increment);
}

void tye_tg_set_increment(uint8_t ch,uint16_t increment) {
    //Write the increment value to the TG
    uint8_t data[3];
    //Prepare the data to be sent
    data[0] = ch & 0x03;
    data[1] = (increment >> 8) & 0xFF;
    data[2] = increment & 0xFF;
    //Send the data
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, data, 3);
    gpio_put(PIN_CS, 1);
}

void tye_tg_set_volume(uint8_t ch,uint8_t volume) {
    //Write the volume value to the TG
    uint8_t data[3];
    //Prepare the data to be sent
    data[0] = 0x04 | (ch & 0x03);
    data[1] = 0x00;
    data[2] = volume & 0xFF;
    //Send the data
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, data, 3);
    gpio_put(PIN_CS, 1);
}

void tye_tg_set_waveform(uint8_t ch,tg_wave_type_t waveform) {
    //Write the waveform value to the TG
    uint8_t data[3];
    //Prepare the data to be sent
    data[0] = 0x08 | (ch & 0x03);
    data[1] = 0x00;
    data[2] = waveform & 0x07;
    //Send the data
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, data, 3);
    gpio_put(PIN_CS, 1);
}

void tye_tg_set_wavetable(uint8_t addr, uint8_t in_data) {
    //Write the wavetable value to the TG
    uint8_t data[3];
    //Prepare the data to be sent
    data[0] = 0x20 | (addr & 0x1F);
    data[1] = 0x00;
    data[2] = in_data & 0x0F;
    //Send the data
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, data, 3);
    gpio_put(PIN_CS, 1);
}

void tye_tg_send_raw_command(uint8_t* data) {
    //Send the raw command to the TG
    gpio_put(PIN_CS, 0);
    spi_write_blocking(SPI_PORT, data, 3);
    gpio_put(PIN_CS, 1);
}
