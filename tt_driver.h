#ifndef _TT_DRIVER_H_
#define _TT_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

void tt_project_init(uint16_t tt_project_number);
void tt_project_set_clock(uint32_t clock_frequency);
void tt_project_set_reset(bool reset);

#ifdef __cplusplus
}
#endif

#endif