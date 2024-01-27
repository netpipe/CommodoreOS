#include <conio.h>
#include <tgi.h>
#include <stdint.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200


//this was not working yet

typedef struct {
    double x, y, z;
} Vector3;

typedef struct {
    Vector3 origin;
    Vector3 direction;
} Ray;

typedef struct {
    Vector3 center;
    double radius;
} Sphere;

typedef struct {
    uint8_t r, g, b;
} Color;

typedef struct {
    Sphere sphere;
    Color color;
} SceneObject;

typedef struct {
    SceneObject* objects;
    size_t numObjects;
} Scene;

void clearScreen() {
    tgi_clear();
}

void drawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  //  tgi_setcolor(tgi_get_nearest_color(r, g, b));
    tgi_setpixel(x, y);
}
int x,y;
void drawScene(Scene* scene) {
    for ( y = 0; y < SCREEN_HEIGHT; y++) {
        for ( x = 0; x < SCREEN_WIDTH; x++) {
            // Ray generation
            Ray ray;
            ray.origin.x = 0;
            ray.origin.y = 0;
            ray.origin.z = 0;

            ray.direction.x = (double)(x - SCREEN_WIDTH / 2) / SCREEN_WIDTH;
            ray.direction.y = (double)(y - SCREEN_HEIGHT / 2) / SCREEN_HEIGHT;
            ray.direction.z = 1;

            Color pixelColor = traceRay(ray, scene);
            drawPixel(x, y, pixelColor.r, pixelColor.g, pixelColor.b);
        }
    }
}

Color traceRay(Ray ray, Scene* scene) {
    Color backgroundColor = {0, 0, 0};
    Color objectColor = backgroundColor;

    double closestHit = INFINITY;
    for (size_t i = 0; i < scene->numObjects; i++) {
        double hit = intersect(ray, &scene->objects[i].sphere);
        if (hit < closestHit) {
            closestHit = hit;
            objectColor = scene->objects[i].color;
        }
    }

    return objectColor;
}

double intersect(Ray ray, Sphere* sphere) {
    Vector3 oc;
    oc.x = ray.origin.x - sphere->center.x;
    oc.y = ray.origin.y - sphere->center.y;
    oc.z = ray.origin.z - sphere->center.z;

    double a = dot(ray.direction, ray.direction);
    double b = 2.0 * dot(oc, ray.direction);
    double c = dot(oc, oc) - sphere->radius * sphere->radius;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return INFINITY;
    }

    return (-b - sqrt(discriminant)) / (2.0 * a);
}

double dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

int main() {
    SceneObject objects[] = {
        {{0, 0, 5}, 1.0, {255, 0, 0}},    // Red sphere
        {{0, -1, 7}, 1.0, {0, 255, 0}},   // Green sphere
        {{0, -5001, 0}, 5000.0, {0, 0, 255}}  // Blue ground
    };

    Scene scene;
    scene.objects = objects;
    scene.numObjects = sizeof(objects) / sizeof(SceneObject);

    tgi_install(&tgi_static_stddrv);

    if (tgi_init() != 0) {
        printf("Graphics initialization failed.\n");
        return 1;
    }

    clearScreen();
    drawScene(&scene);

    while (!kbhit()) {
        // Wait for key press
    }

    tgi_done();

    return 0;
}

