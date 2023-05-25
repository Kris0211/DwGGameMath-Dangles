#include "dwgSimpleGraphics.h"

#define NUM 4
#define DAMP 0.97f

const Vector3 color(1.f, 0.5f, 0.f);

// entry point for the app (using WinMain, so no console appears, just the rendering window)
int WinMain()
{
    // init window and rendering with given width, height, and title of the window
    if (!dwgInitApp(1600, 900, "dangles.gif"))
        return 1;

    // Initial state
    Vector3 pos[NUM] = {};
    Vector3 vel[NUM] = {};
    float dist[NUM - 1] = {};
    float dtLeft = 0.f;
    const Vector3 g = {0.f, 0.f, -150.f};
    const float dtStep = 1.f / 60.f;

    for (int i = 0; i < NUM; ++i) {
        pos[i] = Vector3(0.f, 2.f - static_cast<float>(i), 0.f);
        vel[i] = Vector3(0.f);
    }

    for (int i = 0; i < NUM - 1; ++i)
        dist[i] = length(pos[i] - pos[i + 1]);


    // main game loop, each iteration is a single frame
    while (!dwgShouldClose())
    {
        const double globalTime = dwgGlobalTime();	// global time - time since the start of the app

        dtLeft += dwgDeltaTime();
        while (dtLeft > dtStep) {
            dtLeft -= dtStep;
            float dt = dtStep;

            // propose pos & vel
            Vector3 newPos[NUM] = {};
            for (int i = 0; i < NUM; ++i) {
                vel[i] = (vel[i] + g * dt) * DAMP;
                newPos[i] = pos[i] + vel[i] * dt;
            }

            // constraints
            newPos[0] = Vector3(0.f, sinf(5.f * static_cast<float>(globalTime)) * 2.f, 2.f);
            for (int i = 0; i < NUM - 1; ++i) {
                // distance
                const Vector3 diff = normalize(newPos[i + 1] - newPos[i]) * dist[i];
                newPos[i + 1] = newPos[i] + diff;

                // collision
                {
                    // create collider
                    Vector3 colPos = {0.f, 0.5f, 0.f};
                    const float radius = dist[0];
                    const float cRadius = 1.0f;
                    //dwgDebugSphere(pos[i], Vector3(1.0f), Vector3(0.0f, 0.5f, 1.f));

                    // calculate constraints
                    const Vector3 cDiff = newPos[i + 1] - colPos;
                    const float cDist = length(cDiff);
                    const float inCol = radius + cRadius - cDist;
                    if (inCol > 0.0f) {
                        const Vector3 resolve = normalize(cDiff) * inCol;
                        newPos[i + 1] += resolve;
                    }
                }
            }
            dwgDebugSphere(Vector3(0.f),
                           Vector3(static_cast<float>(DWG_PI) * 0.5f),
                           Vector3(0.0f, 0.5f, 1.f));
            // update pos & vel
            for (int i = 0; i < NUM; ++i) {
                vel[i] = (newPos[i] - pos[i]) / dt;
                pos[i] = newPos[i];

                //draw call
                dwgDebugSphere(pos[i], Vector3(0.5f), color);
            }
        }

        // prepare camera
        const Point3 eye = { 5.0f, 5.0f, 1.0f }; // eye position
        const Point3 at = { 0.0f, 0.0f, 0.0f }; // what we're looking at
        const Vector3 up = { 0.0f, 0.0f, 1.0f }; // where's the up direction for camera
        const Matrix4 camera = Matrix4::lookAt(eye, at, up); // create camera view matrix
        const float fov = 120.0f; // field of view - angle of view of the camera (for perspective)

        // draw scene
        dwgRender(camera, fov);
    }

    // release rendering and window app
    dwgReleaseApp();

    return 0;
}
