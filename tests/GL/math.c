#include "math.h"
#include <math.h>

int16_t sinTable[360];
int16_t cosTable[360];

void initSinCosTables() {
    for (int i = 0; i < 360; ++i) {
        sinTable[i] = (int16_t)(sin(i * 0.0174533) * 100);
        cosTable[i] = (int16_t)(cos(i * 0.0174533) * 100);
    }
}

Vector3 createVector3(int16_t x, int16_t y, int16_t z) {
    Vector3 result = {x, y, z};
    return result;
}

Vector3 addVectors(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vector3 subtractVectors(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

Vector3 multiplyVectorScalar(Vector3 v, int16_t scalar) {
    Vector3 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    return result;
}

Matrix3x3 multiplyMatrices(Matrix3x3 a, Matrix3x3 b) {
    Matrix3x3 result;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j];
        }
    }
    return result;
}

Vector3 rotateVectorX(Vector3 v, int16_t angle) {
    Matrix3x3 rotationMatrix = {
        {1, 0, 0},
        {0, cosTable[angle], -sinTable[angle]},
        {0, sinTable[angle], cosTable[angle]}
    };

    return multiplyMatrices(rotationMatrix, createVector3(v.x, v.y, v.z));
}

Vector3 rotateVectorY(Vector3 v, int16_t angle) {
    Matrix3x3 rotationMatrix = {
        {cosTable[angle], 0, sinTable[angle]},
        {0, 1, 0},
        {-sinTable[angle], 0, cosTable[angle]}
    };

    return multiplyMatrices(rotationMatrix, createVector3(v.x, v.y, v.z));
}
