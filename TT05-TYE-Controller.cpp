#include <stdio.h>
#include <cstdlib>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/spi.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#include "tt_driver.h"
#include "tye_tg_driver.h"
#include "tye_seq.h"

uint8_t play_seq = 0;
uint32_t wait_timer = 0;

tg_wave_type_t get_random_wf(void){
    uint8_t wf = rand() % 7;
    if(wf >= TG_NOISE)wf++;
    return (tg_wave_type_t)wf;
}

bool timer_handler(repeating_timer_t *rt){
    if(play_seq == 0){//Start playing score
        tye_tg_set_waveform(0,get_random_wf());
        tye_tg_set_waveform(1,get_random_wf());
        tye_tg_set_waveform(2,get_random_wf());
        tye_tg_set_waveform(3,TG_NOISE);
        tye_seq_play(true);
        play_seq = 1;
    }
    else if(play_seq == 1){//Playing score
        bool ret = tye_seq_play(false);
        if(ret){//End of score
            printf("Play done\n");
            play_seq = 2;
        }
    }
    else if(play_seq == 2){//Start 1000ms timer
        wait_timer = 1000;
        play_seq = 3;
    }
    else if(play_seq == 3){
        wait_timer--;
        if(wait_timer == 0){
            play_seq = 0;
        }
    }
    return true;
}

int main()
{
    //Set PLL Freq to 125MHz = 1500MHz/(6 * 2)
    pll_init(pll_sys, 1, 1500 * MHZ, 6, 2);

    //Setup sysclk to use PLL
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                    125 * MHZ,
                    125 * MHZ);

    // CLK peri is clocked from clk_sys so need to change clk_peri's freq
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    125 * MHZ,
                    125 * MHZ);

    stdio_init_all();

    sleep_ms(1000);

    tt_project_init(256);
    tt_project_set_clock((125 * MHZ) / 3.0);
    //Reset the project
    tt_project_set_reset(true);

    tye_tg_init();

    //Release project reset
    tt_project_set_reset(false);

    //Initialize wavetable(First half:saw, Second half:triangle)
    int8_t wavetable[32] = {
        -7,-6,-5,-4,-3,-2,-1,0,
        1,2,3,4,5,6,7,-8
        -7,-5,-3,-1,1,3,5,7,
        7,5,3,1,-1,-3,-5,-7
    }; 

    // set timer handler
    struct repeating_timer timer0;
    add_repeating_timer_us(1000, timer_handler, NULL, &timer0);

    while (true) {
        //Do nothing
    }
    
}
