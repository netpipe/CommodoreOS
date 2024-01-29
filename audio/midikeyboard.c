#include <stdio.h>
#include <conio.h>
#include <peekpoke.h>

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHANNEL  0  // MIDI channel 1

void sendMidiMessage(int status, int note, int velocity) {
    // Send MIDI message (simplified for demonstration)
    // In a real setup, you'd send this data to a MIDI interface
    printf("%02X %02X %02X\n", status | MIDI_CHANNEL, note, velocity);
}

int main() {
    clrscr();  // Clear the screen

    // Main loop
    while (1) {
        // Check for keyboard input
        if (kbhit()) {
            char key = cgetc();
            int note;

            // Map keyboard keys to MIDI notes (simplified)
            switch (key) {
                case 'a': note = 60; break; // C4
                case 's': note = 62; break; // D4
                case 'd': note = 64; break; // E4
                // Add more key-to-note mappings here
                default: continue; // Ignore other keys
            }

            // Send MIDI note-on message
            sendMidiMessage(MIDI_NOTE_ON, note, 127); // Velocity 127 for max volume
        }
    }

    return 0;
}

