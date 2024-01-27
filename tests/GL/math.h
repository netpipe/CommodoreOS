#ifndef MATH_H
#define MATH_H

#include <stdint.h>

// 3D Vector
typedef struct {
    int16_t x, y, z;
} Vector3;

// 3x3 Matrix
typedef struct {
    int16_t m[3][3];
} Matrix3x3;

// Precomputed sin and cos tables
extern int16_t sinTable[360];
extern int16_t cosTable[360];

// Initialize a 3D vector
Vector3 createVector3(int16_t x, int16_t y, int16_t z);

// Add two 3D vectors
Vector3 addVectors(Vector3 a, Vector3 b);

// Subtract two 3D vectors
Vector3 subtractVectors(Vector3 a, Vector3 b);

// Scalar multiplication of a 3D vector
Vector3 multiplyVectorScalar(Vector3 v, int16_t scalar);

// Multiply two 3x3 matrices
Matrix3x3 multiplyMatrices(Matrix3x3 a, Matrix3x3 b);

// Rotate a 3D vector around the X-axis
Vector3 rotateVectorX(Vector3 v, int16_t angle);

// Rotate a 3D vector around the Y-axis
Vector3 rotateVectorY(Vector3 v, int16_t angle);

#endif

