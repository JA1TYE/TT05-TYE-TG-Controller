#ifndef _TYE_SEQ_H_
#define _TYE_SEQ_H_

#include <cstdint>
#include "hardware/clocks.h"
#include "hardware/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

bool tye_seq_play(bool reset);
bool tye_seq_parse(uint8_t ch);

//Definition of commands
#define SEQ_WAIT 0xff
#define SEQ_END 0xfe

#ifdef __cplusplus
}
#endif

#endif
