import sys
import mido

incr_array = [
    0x000D,0x000E,0x000F,0x0010,0x0011,0x0012,0x0013,0x0014,
    0x0015,0x0016,0x0017,0x0019,0x001A,0x001C,0x001E,0x001F,
    0x0021,0x0023,0x0025,0x0027,0x002A,0x002C,0x002F,0x0032,
    0x0035,0x0038,0x003B,0x003F,0x0042,0x0046,0x004A,0x004F,
    0x0054,0x0059,0x005E,0x0063,0x0069,0x0070,0x0076,0x007D,
    0x0085,0x008D,0x0095,0x009E,0x00A7,0x00B1,0x00BC,0x00C7,
    0x00D3,0x00DF,0x00EC,0x00FB,0x0109,0x0119,0x012A,0x013C,
    0x014E,0x0162,0x0177,0x018E,0x01A5,0x01BE,0x01D9,0x01F5,
    0x0213,0x0232,0x0254,0x0277,0x029D,0x02C5,0x02EF,0x031B,
    0x034B,0x037D,0x03B2,0x03EA,0x0426,0x0465,0x04A8,0x04EF,
    0x053A,0x0589,0x05DE,0x0637,0x0696,0x06FA,0x0764,0x07D4,
    0x084C,0x08CA,0x0950,0x09DD,0x0A74,0x0B13,0x0BBB,0x0C6E,
    0x0D2B,0x0DF3,0x0EC8,0x0FA9,0x1097,0x1194,0x129F,0x13BB,
    0x14E7,0x1625,0x1776,0x18DC,0x1A56,0x1BE7,0x1D90,0x1F52,
    0x212E,0x2328,0x253F,0x2776,0x29CE,0x2C4B,0x2EED,0x31B7,
    0x34AC,0x37CE,0x3B1F,0x3EA3,0x425D,0x464F,0x4A7D,0x4EEB,
]

track2channel = {2:0, 3:1, 4:2, 11:3}

def main():
    if len(sys.argv) != 2:
        print("Usage: {} <midi file>".format(sys.argv[0]))
        sys.exit(1)
    mid = mido.MidiFile(sys.argv[1])
    max_track_len = 0
    score_array = ["","","",""]
    for i, track in enumerate(mid.tracks):
        count = 0
        command_count = 0
        is_first_note_on = True
        if i in [2,3,4,11]: #2,3,4,12
            #print("Track {}: {}".format(i, track.name))
            for msg in track:
                #debug print
                #if msg.type == "note_on" or msg.type == "note_off":
                #    print(msg)
                count += msg.time
                if msg.type == "note_on":
                    if is_first_note_on == True:
                        if count > 240:
                            rest_time = count - 240
                            high_byte = rest_time >> 8
                            low_byte = rest_time & 0xff
                            
                            score_array[track2channel[i]] += "0xff,0x{:02x},0x{:02x},\n".format(high_byte, low_byte)
                            command_count += 3
                            #print("0xff,0x{:02x},0x{:02x},".format(high_byte, low_byte))
                        is_first_note_on = False
                    else:
                        #generate wait message
                        if msg.time > 0 :
                            rest_time = msg.time
                            high_byte = rest_time >> 8
                            low_byte = rest_time & 0xff
                            
                            score_array[track2channel[i]] += "0xff,0x{:02x},0x{:02x},\n".format(high_byte, low_byte)
                            command_count += 3
                    #print("0xff,0x{:02x},0x{:02x},".format(high_byte, low_byte))    
                    #generate note-on message
                    #set incr
                    channel = track2channel[i]
                    incl_addr = channel & 0x03
                    incr_high_byte = incr_array[msg.note] >> 8
                    incr_low_byte = incr_array[msg.note] & 0xff
                    
                    score_array[track2channel[i]] += "0x{:02x},0x{:02x},0x{:02x},\n".format(incl_addr, incr_high_byte, incr_low_byte)
                    command_count += 3
                    #print("0x{:02x},0x{:02x},0x{:02x},".format(incl_addr, incr_high_byte, incr_low_byte))
                    
                    #set volume
                    channel = track2channel[i]
                    vol_addr = (channel & 0x03) | 0x04
                    vol_high_byte = 0x00
                    vol_low_byte = 0xff
                    
                    score_array[track2channel[i]] += "0x{:02x},0x{:02x},0x{:02x},\n".format(vol_addr, vol_high_byte, vol_low_byte)
                    command_count += 3
                    #print("0x{:02x},0x{:02x},0x{:02x},".format(vol_addr, vol_high_byte, vol_low_byte))

                if msg.type == "note_off":
                    #generate wait message
                    rest_time = msg.time
                    high_byte = rest_time >> 8
                    low_byte = rest_time & 0xff
                    
                    score_array[track2channel[i]] += "0xff,0x{:02x},0x{:02x},\n".format(high_byte, low_byte)
                    command_count += 3
                    #print("0xff,0x{:02x},0x{:02x},".format(high_byte, low_byte))
                    
                    #generate note-off message
                    #set volume
                    channel = track2channel[i]
                    vol_addr = (channel & 0x03) | 0x04
                    vol_high_byte = 0x00
                    vol_low_byte = 0x00
                    score_array[track2channel[i]] += "0x{:02x},0x{:02x},0x{:02x},\n".format(vol_addr, vol_high_byte, vol_low_byte)
                    command_count += 3                    
                    #print("0x{:02x},0x{:02x},0x{:02x},".format(vol_addr, vol_high_byte, vol_low_byte))
            #generate end of track message
            score_array[track2channel[i]] += "0xfe,0x00,0x00,\n"
            command_count += 3      
            #print("0xfe,0x00,0x00,")
            max_track_len = max(max_track_len, command_count)
    print("uint8_t score_data[4][{}] = {{".format(max_track_len))
    for i in range(4):
        print("{")
        score_array[i].removesuffix(",")
        print(score_array[i], end="")
        print("},")
    print("};")

if __name__ == "__main__":
    main()