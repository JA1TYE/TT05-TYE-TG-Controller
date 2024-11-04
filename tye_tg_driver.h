#ifndef _TYE_TG_DRIVER_H_
#define _TYE_TG_DRIVER_H_

#include <cstdint>
#include "hardware/clocks.h"
#include "hardware/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

// SPI Defines
#define SPI_PORT spi1
#define PIN_MISO 8
#define PIN_CS   9
#define PIN_SCK  10
#define PIN_MOSI 11

typedef enum{
    TG_SQR_50 = 0,
    TG_SQR_12_5 = 1,
    TG_SQR_25 = 2,
    TG_NOISE = 3,
    TG_WT_NORMAL = 4,
    TG_WT_FIRST_HALF = 5,
    TG_WT_SECOND_HALF = 6,
    TG_WT_SHUFFLE = 7
}tg_wave_type_t;

#define TG_FSAMPLE (125 * MHZ / (3.0 * 1024)) 

void tye_tg_init(void);
void tye_tg_set_frequency(uint8_t ch,float frequency);
void tye_tg_set_increment(uint8_t ch,uint16_t increment);
void tye_tg_set_volume(uint8_t ch,uint8_t volume);
void tye_tg_set_waveform(uint8_t ch,tg_wave_type_t waveform);
void tye_tg_set_wavetable(uint8_t addr, uint8_t data);
void tye_tg_send_raw_command(uint8_t* data);

#ifdef __cplusplus
}
#endif

#endif
