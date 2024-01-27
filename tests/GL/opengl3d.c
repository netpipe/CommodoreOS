#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCREEN_SIZE SCREEN_WIDTH * SCREEN_HEIGHT

// Memory-mapped registers
#define VIC_CTRL (*(volatile unsigned char*)0xD011)
#define VIC_RASTER (*(volatile unsigned char*)0xD012)
#define VIC_SCREEN_POS (*(volatile unsigned int*)0xD018)

// Constants for trigonometric calculations
#define SIN_TABLE_SIZE 359
#define DEG_TO_RAD_FACTOR 0.0174533
//#define FIXED_POINT_SHIFT 8
// Constants
#define FIXED_POINT_SHIFT 16
#define PI 3.14159265358979323846
typedef int16_t fixed_point_t;

// Precompute sine values for angles 0 to 359 degrees
int16_t sinTable[SIN_TABLE_SIZE];

// Convert degrees to fixed-point format
int16_t degreesToFixedPoint(int degrees) {
    return degrees << FIXED_POINT_SHIFT;
}
int i;





// Fixed-point conversion
fixed_point_t int_to_fixed(int n) {
    return n << FIXED_POINT_SHIFT;
}

// Sine function (Taylor series approximation)
fixed_point_t fixed_sin(fixed_point_t x) {
    fixed_point_t result = x;
    fixed_point_t term = x;
    fixed_point_t sign = int_to_fixed(-1);

    for ( i = 3; i <= 10; i += 2) {
        term = (term * x * x) >> FIXED_POINT_SHIFT;
        term /= int_to_fixed(i * (i - 1));
        term *= sign;
        result += term;
        sign = -sign;
    }

    return result;
}

// Cosine function (Taylor series approximation)
fixed_point_t fixed_cos(fixed_point_t x) {
    fixed_point_t result = int_to_fixed(1);
    fixed_point_t term = int_to_fixed(1);
    fixed_point_t sign = int_to_fixed(-1);

    for (i = 2; i <= 10; i += 2) {
        term = (term * x * x) >> FIXED_POINT_SHIFT;
        term /= int_to_fixed(i * (i - 1));
        term *= sign;
        result += term;
        sign = -sign;
    }

    return result;
}

void initSinTable() {
    for (i = 0; i < SIN_TABLE_SIZE; ++i) {
    int test=fixed_sin(i * DEG_TO_RAD_FACTOR);
        sinTable[i] = degreesToFixedPoint( test * 100);
    }
}
// Triangle strip vertices in 3D space
#define NUM_VERTICES 10
typedef struct {
    int16_t x, y, z;
} Vertex;

Vertex vertices[NUM_VERTICES];

// Rotation matrices for X and Y axes
int16_t rotationMatrixX[3][3] = {
    {1, 0, 0},
    {0, 85, -50},
    {0, 50, 85}
};

int16_t rotationMatrixY[3][3] = {
    {85, 0, 50},
    {0, 1, 0},
    {-50, 0, 85}
};

// Precomputed sin values for optimization
int16_t sinTable[SIN_TABLE_SIZE];

void initGraphics() {
    // Set up VIC-II for graphics mode
    VIC_CTRL = 0x3B; // Multicolor bitmap mode

    // Precompute sin values for optimization
    for (int i = 0; i < SIN_TABLE_SIZE; ++i) {
        sinTable[i] = 50 * fixed_sin(i * DEG_TO_RAD_FACTOR);
    }
}

void clearScreen() {
    clrscr();
}

void drawPixel(int x, int y, unsigned char color) {
    unsigned int screenPos = SCREEN_WIDTH * y + x;
    *((unsigned char*)0x0400 + screenPos) = color;
}

void rotateVertex(Vertex* vertex, int16_t matrix[3][3]) {
    int16_t newX = vertex->x * matrix[0][0] + vertex->y * matrix[0][1] + vertex->z * matrix[0][2];
    int16_t newY = vertex->x * matrix[1][0] + vertex->y * matrix[1][1] + vertex->z * matrix[1][2];
    int16_t newZ = vertex->x * matrix[2][0] + vertex->y * matrix[2][1] + vertex->z * matrix[2][2];

    vertex->x = newX;
    vertex->y = newY;
    vertex->z = newZ;
}

void initTriangleStrip() {
    // Initialize the triangle strip in 3D space
    for (int i = 0; i < NUM_VERTICES; ++i) {
        vertices[i].x = sinTable[i % SIN_TABLE_SIZE];
        vertices[i].y = i * 10;
        vertices[i].z = 300;
    }
}

void drawTriangleStrip() {
    for (int i = 0; i < NUM_VERTICES; ++i) {
        rotateVertex(&vertices[i], rotationMatrixX);
        rotateVertex(&vertices[i], rotationMatrixY);
    }

    // Draw the rotating triangle strip
    for (int i = 0; i < NUM_VERTICES - 2; ++i) {
        int screenX1 = SCREEN_WIDTH / 2 + vertices[i].x / (vertices[i].z / 100);
        int screenY1 = SCREEN_HEIGHT / 2 - vertices[i].y / (vertices[i].z / 100);

        int screenX2 = SCREEN_WIDTH / 2 + vertices[i + 1].x / (vertices[i + 1].z / 100);
        int screenY2 = SCREEN_HEIGHT / 2 - vertices[i + 1].y / (vertices[i + 1].z / 100);

        int screenX3 = SCREEN_WIDTH / 2 + vertices[i + 2].x / (vertices[i + 2].z / 100);
        int screenY3 = SCREEN_HEIGHT / 2 - vertices[i + 2].y / (vertices[i + 2].z / 100);

        drawPixel(screenX1, screenY1, i + 1);
        drawPixel(screenX2, screenY2, i + 2);
        drawPixel(screenX3, screenY3, i + 3);
    }
}

void renderFrame() {
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
}

int main() {
    initGraphics();
    initTriangleStrip();

    while (1) {
        clearScreen();
        drawTriangleStrip();
        renderFrame();
    }

    return 0;
}
