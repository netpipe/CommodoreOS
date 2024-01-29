#include <stdio.h>
#include <conio.h>

// Define a process structure
typedef struct {
    void (*func)(void); // Function to execute
    char name[20];      // Process name
} Process;

// Define two sample processes
void process1(void) {
    while (1) {
        cprintf("Process 1\n");
        // Add some delay or work here
    }
}

void process2(void) {
    while (1) {
        cprintf("Process 2\n");
        // Add some delay or work here
    }
}

int main() {
    Process processes[] = {
        {process1, "Process 1"},
        {process2, "Process 2"}
    };

    // Initialize screen
    clrscr();
    cputs("Microkernel for C64\n");

    // Main loop for process scheduling
    while (1) {
        for (int i = 0; i < sizeof(processes) / sizeof(processes[0]); ++i) {
            cprintf("Running %s\n", processes[i].name);
            processes[i].func();
        }
    }

    return 0;
}

