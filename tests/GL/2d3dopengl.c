#include <conio.h>
#include <stdint.h>
#include <math.h>

// Memory-mapped registers
#define VIC_CTRL (*(volatile unsigned char*)0xD011)
#define VIC_RASTER (*(volatile unsigned char*)0xD012)
#define VIC_SCREEN_POS (*(volatile unsigned int*)0xD018)

// Constants
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

// Precomputed sin and cos tables
int16_t sinTable[360];
int16_t cosTable[360];

// OpenGL-like drawing states
typedef struct {
    int16_t x, y, z;
    unsigned char color;
} Vertex3D;

typedef struct {
    int16_t x, y;
    unsigned char color;
} Vertex2D;

Vertex3D vertexBuffer3D[50];  // Assuming a maximum of 50 vertices for simplicity
Vertex2D vertexBuffer2D[50];  // Assuming a maximum of 50 vertices for simplicity
int vertexCount3D = 0;
int vertexCount2D = 0;

// Initialize graphics mode
void initGraphics() {
    VIC_CTRL = 0x3B; // Multicolor bitmap mode
}

// Clear the screen
void clearScreen() {
    clrscr();
}

// Draw a pixel
void drawPixel(int x, int y, unsigned char color) {
    unsigned int screenPos = SCREEN_WIDTH * y + x;
    *((unsigned char*)0x0400 + screenPos) = color;
}

// Precompute sin and cos tables
void initSinCosTables() {
    for (int i = 0; i < 360; ++i) {
        sinTable[i] = (int16_t)(sin(i * 0.0174533) * 100);
        cosTable[i] = (int16_t)(cos(i * 0.0174533) * 100);
    }
}

// Set 3D vertex data in the buffer
void glVertex3i(int x, int y, int z) {
    if (vertexCount3D < sizeof(vertexBuffer3D) / sizeof(Vertex3D)) {
        vertexBuffer3D[vertexCount3D].x = x;
        vertexBuffer3D[vertexCount3D].y = y;
        vertexBuffer3D[vertexCount3D].z = z;
        vertexBuffer3D[vertexCount3D].color = vertexCount3D + 1;  // Different color for each vertex
        ++vertexCount3D;
    }
}

// Set 2D vertex data in the buffer
void glVertex2i(int x, int y) {
    if (vertexCount2D < sizeof(vertexBuffer2D) / sizeof(Vertex2D)) {
        vertexBuffer2D[vertexCount2D].x = x;
        vertexBuffer2D[vertexCount2D].y = y;
        vertexBuffer2D[vertexCount2D].color = vertexCount2D + 1;  // Different color for each vertex
        ++vertexCount2D;
    }
}

// Begin drawing
void glBegin(int mode) {
    if (mode == GL_TRIANGLE_STRIP) {
        vertexCount3D = 0;
        vertexCount2D = 0;
    }
}

// End drawing and render the triangle strip
void glEnd() {
    for (int i = 0; i < vertexCount3D - 2; ++i) {
        int x1 = SCREEN_WIDTH / 2 + vertexBuffer3D[i].x / (vertexBuffer3D[i].z / 100);
        int y1 = SCREEN_HEIGHT / 2 - vertexBuffer3D[i].y / (vertexBuffer3D[i].z / 100);
        int x2 = SCREEN_WIDTH / 2 + vertexBuffer3D[i + 1].x / (vertexBuffer3D[i + 1].z / 100);
        int y2 = SCREEN_HEIGHT / 2 - vertexBuffer3D[i + 1].y / (vertexBuffer3D[i + 1].z / 100);
        int x3 = SCREEN_WIDTH / 2 + vertexBuffer3D[i + 2].x / (vertexBuffer3D[i + 2].z / 100);
        int y3 = SCREEN_HEIGHT / 2 - vertexBuffer3D[i + 2].y / (vertexBuffer3D[i + 2].z / 100);

        drawPixel(x1, y1, vertexBuffer3D[i].color);
        drawPixel(x2, y2, vertexBuffer3D[i + 1].color);
        drawPixel(x3, y3, vertexBuffer3D[i + 2].color);
    }

    for (int i = 0; i < vertexCount2D - 2; ++i) {
        int x1 = SCREEN_WIDTH / 2 + vertexBuffer2D[i].x;
        int y1 = SCREEN_HEIGHT / 2 - vertexBuffer2D[i].y;
        int x2 = SCREEN_WIDTH / 2 + vertexBuffer2D[i + 1].x;
        int y2 = SCREEN_HEIGHT / 2 - vertexBuffer2D[i + 1].y;
        int x3 = SCREEN_WIDTH / 2 + vertexBuffer2D[i + 2].x;
        int y3 = SCREEN_HEIGHT / 2 - vertexBuffer2D[i + 2].y;

        drawPixel(x1, y1, vertexBuffer2D[i].color);
        drawPixel(x2, y2, vertexBuffer2D[i + 1].color);
        drawPixel(x3, y3, vertexBuffer2D[i + 2].color);
    }
}

// Rotate a 3D point around the X-axis
void rotatePointX(int16_t* x, int16_t* y, int16_t* z, int16_t angle) {
    int16_t tempY = *y;
    *y = (tempY * cosTable[angle] - *z * sinTable[angle]) / 100;
    *z = (tempY * sinTable[angle] + *z * cosTable[angle]) / 100;
}

// Rotate a 3D point around the Y-axis
void rotatePointY(int16_t* x, int16_t* y, int16_t* z, int16_t angle) {
    int16_t tempX = *x;
    *x = (tempX * cosTable[angle] + *z * sinTable[angle]) / 100;
    *z = (-tempX * sinTable[angle] + *z * cosTable[angle]) / 100;
}

// Main function
int main() {
    initGraphics();
    clearScreen();
    initSinCosTables();

    int frame = 0;

    while (1) {
        // Draw 3D triangle strip
        glBegin(GL_TRIANGLE_STRIP);

        // Add 3D vertices to the buffer (rotating points for animation)
        for (int i = 0; i < 50; ++i) {
            int16_t x = i * 10;
            int16_t y = 50 + sinTable[(i + frame) % 360];
            int16_t z = 300;
            rotatePointX(&x, &y, &z, frame);
            rotatePointY(&x, &y, &z, frame);
            glVertex3i(x, y, z);
        }

        glEnd();

        // Draw 2D triangle strip
        glBegin(GL_TRIANGLE_STRIP);

        // Add 2D vertices to the buffer (rotating points for animation)
        for (int i = 0; i < 50; ++i) {
            int16_t x = i * 10;
            int16_t y = 50 + sinTable[(i + frame) % 360];
            rotatePointX(&x, &y, &z, frame);
            rotatePointY(&x, &y, &z, frame);
            glVertex2i(x, y);
        }

        glEnd();

        // Render the frame
        VIC_RASTER = 0x00; // Set raster line to the top

        // Wait for the vertical blank to start
        while (VIC_RASTER < 0x30);

        // Wait for the vertical blank to end
        while (VIC_RASTER >= 0x30);

        // Set the screen position for double buffering
        VIC_SCREEN_POS = 0x400;

        // Wait for the vertical blank to start
        while (VIC_RASTER < 0x30);

        // Wait for the vertical blank to end
        while (VIC_RASTER >= 0x30);

        // Set the screen position for double buffering
        VIC_SCREEN_POS = 0x400;

        ++frame;
    }

    return 0;
}
