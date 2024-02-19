#include <tgi.h>

void initializeGraphics(void) {
    tgi_install(tgi_static_stddrv);
    tgi_init();
   // while (tgi_busy()) {}
    tgi_clear();
}

void drawHorizontalLine2(int x0, int y, int x1) {
int temp;
int x;
    if (x0 > x1) {
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    for ( x = x0; x <= x1; x++) {
        tgi_setpixel(x, y);
    }
}

void drawHorizontalLine(int x0, int y, int x1) {
int temp;
    // Ensure x0 is less than x1 for consistency
    if (x0 > x1) {
        temp = x0;
        x0 = x1;
        x1 = temp;
    }

    // Use tgi_line() to draw the line, which is faster than setting individual pixels
    tgi_line(x0, y, x1, y);
}

void fillFlatBottomTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
 int dx1,dy1;
int dx2,dy2;
int y;
int xs;
int xe;
    dx1 = x1 - x0, dy1 = y1 - y0;
     dx2 = x2 - x0, dy2 = y2 - y0;
    
    for ( y = y0; y <= y1; y++) {
         xs = x0 + dx1 * (y - y0) / dy1;
         xe = x0 + dx2 * (y - y0) / dy2;
        drawHorizontalLine(xs, y, xe);
    }
}

void fillFlatTopTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
int dx1,dy1;
int dx2,dy2;
int y;
int xs;
int xe;
     dx1 = x2 - x0, dy1 = y2 - y0;
     dx2 = x2 - x1, dy2 = y2 - y1;
    
    for ( y = y0; y <= y2; y++) {
         xs = x0 + dx1 * (y - y0) / dy1;
         xe = x1 + dx2 * (y - y1) / dy2;
        drawHorizontalLine(xs, y, xe);
    }
}

void drawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
    // Sort vertices by y
int temp;
int x3;
    if (y0 > y1) { temp; temp = y0; y0 = y1; y1 = temp; temp = x0; x0 = x1; x1 = temp; }
    if (y1 > y2) { temp; temp = y1; y1 = y2; y2 = temp; temp = x1; x1 = x2; x2 = temp; }
    if (y0 > y1) { temp; temp = y0; y0 = y1; y1 = temp; temp = x0; x0 = x1; x1 = temp; }

    // Check for trivial case of bottom-flat triangle
    if (y1 == y2) {
        fillFlatBottomTriangle(x0, y0, x1, y1, x2, y2);
    } else if (y0 == y1) {
        fillFlatTopTriangle(x0, y0, x1, y1, x2, y2);
    } else {
        // General case: split the triangle in a top-flat and bottom-flat
         x3 = x0 + ((y1 - y0) * (x2 - x0)) / (y2 - y0);
        fillFlatBottomTriangle(x0, y0, x1, y1, x3, y1);
        fillFlatTopTriangle(x1, y1, x3, y1, x2, y2);
    }
}


int main(void) {
    initializeGraphics();

    // Example usage
    drawFilledTriangle(100, 50, 10, 100, 150, 150);

    // Keep the graphics visible
   // while (!tgi_kbhit()) {}

  //  tgi_done();
    return 0;
}

