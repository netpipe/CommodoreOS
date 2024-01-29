#include <stdlib.h>
#include <conio.h>
#include <mouse.h>

// Function to check and report errors
void CheckError(const char* message, unsigned char errorCode) {
    if (errorCode != MOUSE_ERR_OK) {
        cprintf("%s: Error %u\n", message, errorCode);
        exit(EXIT_FAILURE);
    }
}
int i;

    struct mouse_box full_box, small_box;
    unsigned char width, height;
    int Done=0;
    
int main(void) {
    const char* mouse_name = mouse_stddrv;//"1351";  // mou Assuming using a 1351 mouse

       // DoWarning ();
       // mouse_name = mouse_stddrv;
        
    // Initialize conio screen
    clrscr();
     cursor (0);
       // mouse_load_driver(&mouse_static_stddrv, mouse_name);

    // Load and check the mouse driver
     CheckError ("mouse_load_driver",                mouse_load_driver(&mouse_def_callbacks, mouse_name));
    //CheckError ("mouse_install",                mouse_install (&mouse_def_callbacks,mouse_static_stddrv));
    // Show mouse cursor
    mouse_show();
    mouse_getbox (&full_box);
        screensize (&width, &height);

    while (!kbhit()) {  // Continue until a key is pressed
        // Get mouse position and button states
        struct mouse_info info;
        mouse_info(&info);

        // Clear the previous output
        gotoxy(0, 0);
        cclear(40);

        // Display the mouse coordinates and button state
        //gotoxy(0, 2);
        cprintf (" X  = %3d\r\n", info.pos.x);
        cprintf (" Y  = %3d\r\n", info.pos.y);
        cprintf (" B1 = %c\r\n", (info.buttons & MOUSE_BTN_LEFT) ? 'v'  : '^');
        cprintf (" B2 = %c", (info.buttons & MOUSE_BTN_RIGHT) ? 'v'  : '^');
        // You can add more interactive features here

        // Small delay to reduce flickering
        for (i = 0; i < 500; i++);
    }
    
    // Hide mouse cursor and unload the driver
    mouse_hide();
  //  mouse_unload_drv();
    CheckError ("mouse_unload", mouse_unload ());
    
    return EXIT_SUCCESS;
}

