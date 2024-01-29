#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <peekpoke.h>
#include <string.h>
#include <c64.h>
#include <time.h>

bool m_game_is_running = true;
bool m_music_should_play = true;
clock_t m_last_note_endtime = 0;
char m_note_index = 0;

typedef struct _soundData {
    int freq;
    char duration;
} SoundData;

SoundData music[] = {
    {6577,  25},
    { 7382, 25},
    { 6577, 25},
    { 6577, 25},
    { 6577, 12},
    { 7382, 12},
    { 8286, 75},
    { 6577, 25},
    { 7382, 25},
    { 4927, 25},
    { 5530, 6 },
    { 6207, 6 },
    { 6577, 25},
    { 6207, 12},
    { 4927, 25},
    { -1,   -1}
};

void setup(void)
{
    // Clear all SID registers
    memset(&SID, 0, 24);

    // Set up voice 1
    SID.v1.ad = 9; // Attack/Decay
    SID.v1.sr = 0; // Sustain/Release
    SID.amp = 15; // Set volume to max

    clrscr();
    printf("Q to quit\n");
    printf("P to pause\n");
    printf("R to resume\n");
    printf("All other characters print to screen.\n\n");
}

void input(void)
{
    unsigned char c;

    if (kbhit()) {
        c = cgetc();

        switch (c) {
        case 'q':
            m_game_is_running = false;
            break;
        case 'p':
            m_music_should_play = false;
            SID.v1.ctrl = 16;
            break;
        case 'r':
            m_music_should_play = true;
            break;
        default:
            // Display last key pressed to show CPU is not
            // blocked while music is playing
            printf("%c", c);
        }
    }
}

void update(void)
{
    // Play music
    if (m_music_should_play && clock() > m_last_note_endtime) {
        SID.v1.ctrl = 16; // Release voice 1 (stop note)
        m_note_index++; // Advance note pointer
        if (music[m_note_index].freq == -1) {
            m_note_index = 0; // Loop music to beginning
        }
        SID.v1.freq = music[m_note_index].freq; // Set voice 1 to note
        SID.v1.ctrl = 17; // Gate voice 1 (start note)
        // Store clock time this note should stop playing
        m_last_note_endtime = clock() + music[m_note_index].duration;
    }
}

void render(void)
{
    // Nothing to do
}

void shutdown(void)
{
    // Nothing to do
}

int main(void)
{
    setup();
    while (m_game_is_running) {
        input();
        update();
        render();
    }
    shutdown();
    return EXIT_SUCCESS;
}
