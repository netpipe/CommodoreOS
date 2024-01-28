#include <stdint.h>
#include <stdio.h>

// Fixed-point arithmetic definitions
typedef int32_t fixed;
#define FIXED_SHIFT 16
#define FIXED_ONE (1 << FIXED_SHIFT)
#define INT_TO_FIXED(x) ((x) << FIXED_SHIFT)
#define FIXED_TO_INT(x) ((x) >> FIXED_SHIFT)
#define FIXED_MULTIPLY(a, b) ((a * b) >> FIXED_SHIFT)
#define FIXED_DIVIDE(a, b) ((a << FIXED_SHIFT) / b)

// Basic 2D vertex structure
typedef struct {
    fixed x, y;
} Vertex;

// Triangle structure (assuming triangle strip)
typedef struct {
    Vertex vertices[3];
} Triangle;

// Global variable to store triangles (simple approach)
#define MAX_TRIANGLES 10
Triangle triangles[MAX_TRIANGLES];
int triangleCount = 0;

// Start defining a triangle strip
void glBegin() {
    triangleCount = 0;
}

// End defining a triangle strip (rendering)
void glEnd() {
    // Simple rendering (printing coordinates)
    for (int i = 0; i < triangleCount; ++i) {
        printf("Triangle %d:\n", i);
        for (int j = 0; j < 3; ++j) {
            printf("  Vertex %d: (%d, %d)\n", j, 
                   FIXED_TO_INT(triangles[i].vertices[j].x), 
                   FIXED_TO_INT(triangles[i].vertices[j].y));
        }
    }
}

// Add a vertex to the triangle strip
void glVertex(fixed x, fixed y) {
    if (triangleCount < MAX_TRIANGLES) {
        int vertexIndex = triangleCount % 3;
        triangles[triangleCount / 3].vertices[vertexIndex] = (Vertex){x, y};
        if (vertexIndex == 2) {
            triangleCount += 3;
        }
    }
}

// Rotate a vertex around the origin
Vertex rotateVertex(Vertex v, fixed angle) {
    fixed s = sin(angle);
    fixed c = cos(angle);

    // Rotate and return
    return (Vertex){
        FIXED_MULTIPLY(v.x, c) - FIXED_MULTIPLY(v.y, s),
        FIXED_MULTIPLY(v.x, s) + FIXED_MULTIPLY(v.y, c)
    };
}

// Main function
int main() {
    glBegin();
    glVertex(INT_TO_FIXED(0), INT_TO_FIXED(0));
    glVertex(INT_TO_FIXED(10), INT_TO_FIXED(0));
    glVertex(INT_TO_FIXED(5), INT_TO_FIXED(10));
    glEnd();

    return 0;
}

