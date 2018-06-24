#include <cstdio>

#include "gambatte.h"

#include "input.h"

gambatte::GB gameboy;
Input input;

void run_one_frame() {
    size_t samples = 35112;
    long unsigned int audioBuffer[35112 + 2064];
    gameboy.runFor(0, 0, &audioBuffer[0], samples);
}

void wait(int seconds) {
    for (int i = 0; i < 60 * seconds; ++i) {
        run_one_frame();
    }
}

void press(unsigned key, int seconds = 1) {
    input.press(key);
    wait(seconds);
}

void go_to_file_load() {
    press(SELECT | UP);
    press(DOWN);
    press(A);
    press(0);
    press(A);
    press(0);
}

void load_first_entry() {
    press(UP, 2); // scroll to top
    press(0);
    press(A, 10); // wait or song load
    press(0);
}

bool sound_has_been_enabled;

void play_song() {
    sound_has_been_enabled = false;
    puts("start");
    input.press(START);
    wait(10);
}

void stop() {
    puts("stop");
}

void on_ff_write(char p, char data) {
    if (p < 0x10 || p >= 0x40) {
        return; // not sound
    }
    switch (p) {
        case 0x26: // sound enable
            if (data) {
                sound_has_been_enabled = true;
            } else {
                if (sound_has_been_enabled) {
                    stop();
                }
            }
            break;
        default:
            printf("%x=%x\n", p, data & 0xff);
    }
}

void on_lcd_interrupt() {
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: lsdpack <lsdj.gb>");
        return 1;
    }
    gameboy.setInputGetter(&input);
    gameboy.setWriteHandler(on_ff_write);
    gameboy.setLcdHandler(on_lcd_interrupt);
    gameboy.load(argv[1]);

    go_to_file_load();
    load_first_entry();
    play_song();
}
