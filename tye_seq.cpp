#include <stdio.h>

#include "freq_table.h"
#include "tye_tg_driver.h"
#include "tye_seq.h"
#include "score.h"

uint32_t current_data_ptr[4] = {0,0,0,0};
uint16_t current_wait_time[4] = {0,0,0,0};
bool current_wait_flag[4] = {false,false,false,false};
uint8_t end_flag = 0;

bool tye_seq_play(bool reset){
    bool parse_ret;
    if(reset){
        for(int i = 0; i < 4; i++){
            current_data_ptr[i] = 0;
            current_wait_time[i] = 0;
        }
        end_flag = 0;
    }
    for(int i = 0; i < 4; i++){
        parse_ret = tye_seq_parse(i);
        if(parse_ret){
            end_flag |= 1 << i;
        }
    }
    if(end_flag == 0x0f){
        return true;
    }
    else{
        return false;
    }
}

bool tye_seq_parse(uint8_t ch){
    while(true){
        uint8_t cmd_type = score_data[ch][current_data_ptr[ch]];
        uint16_t cmd_data = score_data[ch][current_data_ptr[ch] + 1];
        cmd_data = cmd_data << 8 | score_data[ch][current_data_ptr[ch] + 2];


        if(cmd_type == SEQ_WAIT){
            if(current_wait_flag[ch] == false){
                current_wait_time[ch] = cmd_data;
                current_wait_flag[ch] = true;
                current_wait_time[ch]--;
                return false; 
            }
            else{
                if(current_wait_time[ch] == 0){
                    current_wait_flag[ch] = false;
                    current_data_ptr[ch] += 3;
                }
                else{
                    current_wait_time[ch]--;
                    return false;
                }
            }    

        }
        else if (cmd_type == SEQ_END){
            return true;
        }
        else{
            tye_tg_send_raw_command(&(score_data[ch][current_data_ptr[ch]]));
            current_data_ptr[ch] += 3;
        }
    }
}



