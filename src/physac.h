#pragma once
/**********************************************************************************************
 *
 *   Physac v1.1 - 2D Physics library for videogames
 *
 *   DESCRIPTION:
 *
 *   Physac is a small 2D physics library written in pure C. The engine uses a fixed time-step thread loop
 *   to simluate physics. A physics step contains the following phases: get collision information,
 *   apply dynamics, collision solving and position correction. It uses a very simple struct for physic
 *   bodies with a position vector to be used in any 3D rendering API.
 *
 *   LICENSE: zlib/libpng
 *
 *   Copyright (c) 2016-2020 Victor Fisac (github: @victorfisac)
 *
 *   This software is provided "as-is", without any express or implied warranty. In no event
 *   will the authors be held liable for any damages arising from the use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose, including commercial
 *   applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not claim that you
 *     wrote the original software. If you use this software in a product, an acknowledgment
 *     in the product documentation would be appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *     as being the original software.
 *
 *     3. This notice may not be removed or altered from any source distribution.
 *
 *
 *    CHANGES:
 *
 *    The physac code has been modified for the wispy-c game.
 *
 **********************************************************************************************/

#include "stdafx.h"

#ifdef _DEBUG
#define PHYSAC_DEBUG
#endif

// #define PHYSAC_THREADS
#define PHYSACDEF static

#define PHYSAC_MAX_BODIES 64
#define PHYSAC_MAX_MANIFOLDS 4096
#define PHYSAC_MAX_VERTICES 24
#define PHYSAC_CIRCLE_VERTICES 24

#define PHYSAC_COLLISION_ITERATIONS 100
#define PHYSAC_PENETRATION_ALLOWANCE 0.05f
#define PHYSAC_PENETRATION_CORRECTION 0.4f

#define PHYSAC_PI 3.14159265358979323846
#define PHYSAC_DEG2RAD (PHYSAC_PI / 180.0f)

#define PHYSAC_MALLOC(size) malloc(size)
#define PHYSAC_FREE(ptr) free(ptr)

typedef enum PhysicsShapeType
{
    PHYSICS_CIRCLE,
    PHYSICS_POLYGON
} PhysicsShapeType;
typedef struct PhysicsBodyData *PhysicsBody;

typedef struct Mat2
{
    float m00;
    float m01;
    float m10;
    float m11;
} Mat2;

typedef struct PolygonData
{
    unsigned int vertexCount;
    Vector2 positions[PHYSAC_MAX_VERTICES];
    Vector2 normals[PHYSAC_MAX_VERTICES];
} PolygonData;

typedef struct PhysicsShape
{
    PhysicsShapeType type;
    PhysicsBody body;
    float radius;
    Mat2 transform;
    PolygonData vertexData;
} PhysicsShape;

typedef struct PhysicsBodyData
{
    unsigned int id;
    bool enabled;
    Vector2 position;
    Vector2 velocity;
    Vector2 force;
    float angularVelocity;
    float torque;
    float orient;
    float inertia;
    float inverseInertia;
    float mass;
    float inverseMass;
    float staticFriction;
    float dynamicFriction;
    float restitution;
    bool useGravity;
    bool isGrounded;
    bool freezeOrient;
    PhysicsShape shape;
} PhysicsBodyData;

typedef struct PhysicsManifoldData
{
    unsigned int id;
    PhysicsBody bodyA;
    PhysicsBody bodyB;
    float penetration;
    Vector2 normal;
    Vector2 contacts[2];
    unsigned int contactsCount;
    float restitution;
    float dynamicFriction;
    float staticFriction;
} PhysicsManifoldData, *PhysicsManifold;

PHYSACDEF void InitPhysics(void);
PHYSACDEF void RunPhysicsStep(void);
PHYSACDEF void SetPhysicsTimeStep(double delta);
PHYSACDEF bool IsPhysicsEnabled(void);
PHYSACDEF void SetPhysicsGravity(float x, float y);
PHYSACDEF PhysicsBody CreatePhysicsBodyCircle(Vector2 pos, float radius, float density);
PHYSACDEF PhysicsBody CreatePhysicsBodyRectangle(Vector2 pos, float width, float height, float density);
PHYSACDEF PhysicsBody CreatePhysicsBodyPolygon(Vector2 pos, float radius, int sides, float density);
PHYSACDEF void PhysicsAddForce(PhysicsBody body, Vector2 force);
PHYSACDEF void PhysicsAddTorque(PhysicsBody body, float amount);
PHYSACDEF void PhysicsShatter(PhysicsBody body, Vector2 position, float force);
PHYSACDEF int GetPhysicsBodiesCount(void);
PHYSACDEF PhysicsBody GetPhysicsBody(int index);
PHYSACDEF int GetPhysicsShapeType(int index);
PHYSACDEF int GetPhysicsShapeVerticesCount(int index);
PHYSACDEF Vector2 GetPhysicsShapeVertex(PhysicsBody body, int vertex);
PHYSACDEF void SetPhysicsBodyRotation(PhysicsBody body, float radians);
PHYSACDEF void DestroyPhysicsBody(PhysicsBody body);
PHYSACDEF void ClosePhysics(void);

#define PHYSAC_FLT_MAX 3.402823466e+38f
#define PHYSAC_EPSILON 0.000001f
#define PHYSAC_K 1.0f / 3.0f
#define PHYSAC_VECTOR_ZERO \
    (Vector2) { 0.0f, 0.0f }

#if defined(_WIN32) && defined(PHYSAC_NO_THREADS)
static HANDLE physicsThreadId = INVALID_HANDLE_VALUE;
#endif

static unsigned int usedMemory = 0;
static bool physicsThreadEnabled = false;
static double baseTime = 0.0;
static double startTime = 0.0;
static double deltaTime = 1.0 / 60.0 / 10.0 * 1000;
static double currentTime = 0.0;
static uint64_t frequency = 0;

static double accumulator = 0.0;
static unsigned int stepsCount = 0;
static Vector2 gravityForce = {0.0f, 9.81f};
static PhysicsBody bodies[PHYSAC_MAX_BODIES];
static unsigned int physicsBodiesCount = 0;
static PhysicsManifold contacts[PHYSAC_MAX_MANIFOLDS];
static unsigned int physicsManifoldsCount = 0;

static int FindAvailableBodyIndex();
static PolygonData CreateRandomPolygon(float radius, int sides);
static PolygonData CreateRectanglePolygon(Vector2 pos, Vector2 size);

#if defined(_WIN32)
static DWORD WINAPI PhysicsLoop(LPVOID arg);
#else
static void *PhysicsLoop(void *arg);
#endif

static void PhysicsStep(void);
static int FindAvailableManifoldIndex();
static PhysicsManifold CreatePhysicsManifold(PhysicsBody a, PhysicsBody b);
static void DestroyPhysicsManifold(PhysicsManifold manifold);
static void SolvePhysicsManifold(PhysicsManifold manifold);
static void SolveCircleToCircle(PhysicsManifold manifold);
static void SolveCircleToPolygon(PhysicsManifold manifold);
static void SolvePolygonToCircle(PhysicsManifold manifold);
static void SolveDifferentShapes(PhysicsManifold manifold, PhysicsBody bodyA, PhysicsBody bodyB);
static void SolvePolygonToPolygon(PhysicsManifold manifold);
static void IntegratePhysicsForces(PhysicsBody body);
static void InitializePhysicsManifolds(PhysicsManifold manifold);
static void IntegratePhysicsImpulses(PhysicsManifold manifold);
static void IntegratePhysicsVelocity(PhysicsBody body);
static void CorrectPhysicsPositions(PhysicsManifold manifold);
static float FindAxisLeastPenetration(int *faceIndex, PhysicsShape shapeA, PhysicsShape shapeB);
static void FindIncidentFace(Vector2 *v0, Vector2 *v1, PhysicsShape ref, PhysicsShape inc, int index);
static int Clip(Vector2 normal, float clip, Vector2 *faceA, Vector2 *faceB);
static bool BiasGreaterThan(float valueA, float valueB);
static Vector2 TriangleBarycenter(Vector2 v1, Vector2 v2, Vector2 v3);

static void InitTimer(void);
static uint64_t GetTimeCount(void);
static double GetCurrentTimePhysac(void);

static Vector2 MathCross(float value, Vector2 vector);
static float MathCrossVector2(Vector2 v1, Vector2 v2);
static float MathLenSqr(Vector2 vector);
static float MathDot(Vector2 v1, Vector2 v2);
static inline float DistSqr(Vector2 v1, Vector2 v2);
static void MathNormalize(Vector2 *vector);

static Mat2 Mat2Radians(float radians);
static void Mat2Set(Mat2 *matrix, float radians);
static inline Mat2 Mat2Transpose(Mat2 matrix);
static inline Vector2 Mat2MultiplyVector2(Mat2 matrix, Vector2 vector);

PHYSACDEF void InitPhysics(void)
{

#if defined(_WIN32) && defined(PHYSAC_THREADS)
    physicsThreadId = CreateThread(NULL, 0, &PhysicsLoop, NULL, 0, NULL);
#endif

    InitTimer();

#if defined(PHYSAC_DEBUG)
    printf("[PHYSAC] physics module initialized successfully\n");
#endif

    accumulator = 0.0;
}

PHYSACDEF bool IsPhysicsEnabled(void)
{
    return physicsThreadEnabled;
}

PHYSACDEF void SetPhysicsGravity(float x, float y)
{
    gravityForce.x = x;
    gravityForce.y = y;
}

PHYSACDEF PhysicsBody CreatePhysicsBodyCircle(Vector2 pos, float radius, float density)
{
    PhysicsBody newBody = (PhysicsBody)PHYSAC_MALLOC(sizeof(PhysicsBodyData));
    usedMemory += sizeof(PhysicsBodyData);

    int newId = FindAvailableBodyIndex();
    if (newId != -1)
    {

        newBody->id = newId;
        newBody->enabled = true;
        newBody->position = pos;
        newBody->velocity = PHYSAC_VECTOR_ZERO;
        newBody->force = PHYSAC_VECTOR_ZERO;
        newBody->angularVelocity = 0.0f;
        newBody->torque = 0.0f;
        newBody->orient = 0.0f;
        newBody->shape.type = PHYSICS_CIRCLE;
        newBody->shape.body = newBody;
        newBody->shape.radius = radius;
        newBody->shape.transform = Mat2Radians(0.0f);
        newBody->shape.vertexData = (PolygonData){0};

        newBody->mass = PHYSAC_PI * radius * radius * density;
        newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f / newBody->mass : 0.0f);
        newBody->inertia = newBody->mass * radius * radius;
        newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f / newBody->inertia : 0.0f);
        newBody->staticFriction = 0.4f;
        newBody->dynamicFriction = 0.2f;
        newBody->restitution = 0.0f;
        newBody->useGravity = true;
        newBody->isGrounded = false;
        newBody->freezeOrient = false;

        bodies[physicsBodiesCount] = newBody;
        physicsBodiesCount++;

#if defined(PHYSAC_DEBUG)
        printf("[PHYSAC] created polygon physics body id %i\n", newBody->id);
#endif
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] new physics body creation failed because there is any available id to use\n");
#endif

    return newBody;
}

PHYSACDEF PhysicsBody CreatePhysicsBodyRectangle(Vector2 pos, float width, float height, float density)
{
    PhysicsBody newBody = (PhysicsBody)PHYSAC_MALLOC(sizeof(PhysicsBodyData));
    usedMemory += sizeof(PhysicsBodyData);

    int newId = FindAvailableBodyIndex();
    if (newId != -1)
    {

        newBody->id = newId;
        newBody->enabled = true;
        newBody->position = pos;
        newBody->velocity = (Vector2){0.0f};
        newBody->force = (Vector2){0.0f};
        newBody->angularVelocity = 0.0f;
        newBody->torque = 0.0f;
        newBody->orient = 0.0f;
        newBody->shape.type = PHYSICS_POLYGON;
        newBody->shape.body = newBody;
        newBody->shape.radius = 0.0f;
        newBody->shape.transform = Mat2Radians(0.0f);
        newBody->shape.vertexData = CreateRectanglePolygon(pos, (Vector2){width, height});

        Vector2 center = {0.0f, 0.0f};
        float area = 0.0f;
        float inertia = 0.0f;

        for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++)
        {

            Vector2 p1 = newBody->shape.vertexData.positions[i];
            int nextIndex = (((i + 1) < newBody->shape.vertexData.vertexCount) ? (i + 1) : 0);
            Vector2 p2 = newBody->shape.vertexData.positions[nextIndex];

            float D = MathCrossVector2(p1, p2);
            float triangleArea = D / 2;

            area += triangleArea;

            center.x += triangleArea * PHYSAC_K * (p1.x + p2.x);
            center.y += triangleArea * PHYSAC_K * (p1.y + p2.y);

            float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
            float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
            inertia += (0.25f * PHYSAC_K * D) * (intx2 + inty2);
        }

        center.x *= 1.0f / area;
        center.y *= 1.0f / area;

        for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++)
        {
            newBody->shape.vertexData.positions[i].x -= center.x;
            newBody->shape.vertexData.positions[i].y -= center.y;
        }

        newBody->mass = density * area;
        newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f / newBody->mass : 0.0f);
        newBody->inertia = density * inertia;
        newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f / newBody->inertia : 0.0f);
        newBody->staticFriction = 0.4f;
        newBody->dynamicFriction = 0.2f;
        newBody->restitution = 0.0f;
        newBody->useGravity = true;
        newBody->isGrounded = false;
        newBody->freezeOrient = false;

        bodies[physicsBodiesCount] = newBody;
        physicsBodiesCount++;

#if defined(PHYSAC_DEBUG)
        printf("[PHYSAC] created polygon physics body id %i\n", newBody->id);
#endif
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] new physics body creation failed because there is any available id to use\n");
#endif

    return newBody;
}

PHYSACDEF PhysicsBody CreatePhysicsBodyPolygon(Vector2 pos, float radius, int sides, float density)
{
    PhysicsBody newBody = (PhysicsBody)PHYSAC_MALLOC(sizeof(PhysicsBodyData));
    usedMemory += sizeof(PhysicsBodyData);

    int newId = FindAvailableBodyIndex();
    if (newId != -1)
    {

        newBody->id = newId;
        newBody->enabled = true;
        newBody->position = pos;
        newBody->velocity = PHYSAC_VECTOR_ZERO;
        newBody->force = PHYSAC_VECTOR_ZERO;
        newBody->angularVelocity = 0.0f;
        newBody->torque = 0.0f;
        newBody->orient = 0.0f;
        newBody->shape.type = PHYSICS_POLYGON;
        newBody->shape.body = newBody;
        newBody->shape.transform = Mat2Radians(0.0f);
        newBody->shape.vertexData = CreateRandomPolygon(radius, sides);

        Vector2 center = {0.0f, 0.0f};
        float area = 0.0f;
        float inertia = 0.0f;

        for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++)
        {

            Vector2 position1 = newBody->shape.vertexData.positions[i];
            int nextIndex = (((i + 1) < newBody->shape.vertexData.vertexCount) ? (i + 1) : 0);
            Vector2 position2 = newBody->shape.vertexData.positions[nextIndex];

            float cross = MathCrossVector2(position1, position2);
            float triangleArea = cross / 2;

            area += triangleArea;

            center.x += triangleArea * PHYSAC_K * (position1.x + position2.x);
            center.y += triangleArea * PHYSAC_K * (position1.y + position2.y);

            float intx2 = position1.x * position1.x + position2.x * position1.x + position2.x * position2.x;
            float inty2 = position1.y * position1.y + position2.y * position1.y + position2.y * position2.y;
            inertia += (0.25f * PHYSAC_K * cross) * (intx2 + inty2);
        }

        center.x *= 1.0f / area;
        center.y *= 1.0f / area;

        for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++)
        {
            newBody->shape.vertexData.positions[i].x -= center.x;
            newBody->shape.vertexData.positions[i].y -= center.y;
        }

        newBody->mass = density * area;
        newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f / newBody->mass : 0.0f);
        newBody->inertia = density * inertia;
        newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f / newBody->inertia : 0.0f);
        newBody->staticFriction = 0.4f;
        newBody->dynamicFriction = 0.2f;
        newBody->restitution = 0.0f;
        newBody->useGravity = true;
        newBody->isGrounded = false;
        newBody->freezeOrient = false;

        bodies[physicsBodiesCount] = newBody;
        physicsBodiesCount++;

#if defined(PHYSAC_DEBUG)
        printf("[PHYSAC] created polygon physics body id %i\n", newBody->id);
#endif
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] new physics body creation failed because there is any available id to use\n");
#endif

    return newBody;
}

PHYSACDEF void PhysicsAddForce(PhysicsBody body, Vector2 force)
{
    if (body != NULL)
        body->force = Vector2Add(body->force, force);
}

PHYSACDEF void PhysicsAddTorque(PhysicsBody body, float amount)
{
    if (body != NULL)
        body->torque += amount;
}

PHYSACDEF void PhysicsShatter(PhysicsBody body, Vector2 position, float force)
{
    if (body != NULL)
    {
        if (body->shape.type == PHYSICS_POLYGON)
        {
            PolygonData vertexData = body->shape.vertexData;
            bool collision = false;

            for (int i = 0; i < vertexData.vertexCount; i++)
            {
                Vector2 positionA = body->position;
                Vector2 positionB = Mat2MultiplyVector2(body->shape.transform, Vector2Add(body->position, vertexData.positions[i]));
                int nextIndex = (((i + 1) < vertexData.vertexCount) ? (i + 1) : 0);
                Vector2 positionC = Mat2MultiplyVector2(body->shape.transform, Vector2Add(body->position, vertexData.positions[nextIndex]));

                float alpha = ((positionB.y - positionC.y) * (position.x - positionC.x) + (positionC.x - positionB.x) * (position.y - positionC.y)) /
                              ((positionB.y - positionC.y) * (positionA.x - positionC.x) + (positionC.x - positionB.x) * (positionA.y - positionC.y));

                float beta = ((positionC.y - positionA.y) * (position.x - positionC.x) + (positionA.x - positionC.x) * (position.y - positionC.y)) /
                             ((positionB.y - positionC.y) * (positionA.x - positionC.x) + (positionC.x - positionB.x) * (positionA.y - positionC.y));

                float gamma = 1.0f - alpha - beta;

                if ((alpha > 0.0f) && (beta > 0.0f) && (gamma > 0.0f))
                {
                    collision = true;
                    break;
                }
            }

            if (collision)
            {
                int count = vertexData.vertexCount;
                Vector2 bodyPos = body->position;
                Vector2 *vertices = (Vector2 *)PHYSAC_MALLOC(sizeof(Vector2) * count);
                Mat2 trans = body->shape.transform;

                for (int i = 0; i < count; i++)
                    vertices[i] = vertexData.positions[i];

                DestroyPhysicsBody(body);

                for (int i = 0; i < count; i++)
                {
                    int nextIndex = (((i + 1) < count) ? (i + 1) : 0);
                    Vector2 center = TriangleBarycenter(vertices[i], vertices[nextIndex], PHYSAC_VECTOR_ZERO);
                    center = Vector2Add(bodyPos, center);
                    Vector2 offset = Vector2Subtract(center, bodyPos);

                    PhysicsBody newBody = CreatePhysicsBodyPolygon(center, 10, 3, 10);

                    PolygonData newData = {0};
                    newData.vertexCount = 3;

                    newData.positions[0] = Vector2Subtract(vertices[i], offset);
                    newData.positions[1] = Vector2Subtract(vertices[nextIndex], offset);
                    newData.positions[2] = Vector2Subtract(position, center);

                    newData.positions[0].x *= 0.95f;
                    newData.positions[0].y *= 0.95f;
                    newData.positions[1].x *= 0.95f;
                    newData.positions[1].y *= 0.95f;
                    newData.positions[2].x *= 0.95f;
                    newData.positions[2].y *= 0.95f;

                    for (int j = 0; j < newData.vertexCount; j++)
                    {
                        int nextVertex = (((j + 1) < newData.vertexCount) ? (j + 1) : 0);
                        Vector2 face = Vector2Subtract(newData.positions[nextVertex], newData.positions[j]);

                        newData.normals[j] = (Vector2){face.y, -face.x};
                        MathNormalize(&newData.normals[j]);
                    }

                    newBody->shape.vertexData = newData;
                    newBody->shape.transform = trans;

                    center = PHYSAC_VECTOR_ZERO;
                    float area = 0.0f;
                    float inertia = 0.0f;

                    for (int j = 0; j < newBody->shape.vertexData.vertexCount; j++)
                    {

                        Vector2 p1 = newBody->shape.vertexData.positions[j];
                        int nextVertex = (((j + 1) < newBody->shape.vertexData.vertexCount) ? (j + 1) : 0);
                        Vector2 p2 = newBody->shape.vertexData.positions[nextVertex];

                        float D = MathCrossVector2(p1, p2);
                        float triangleArea = D / 2;

                        area += triangleArea;

                        center.x += triangleArea * PHYSAC_K * (p1.x + p2.x);
                        center.y += triangleArea * PHYSAC_K * (p1.y + p2.y);

                        float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
                        float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
                        inertia += (0.25f * PHYSAC_K * D) * (intx2 + inty2);
                    }

                    center.x *= 1.0f / area;
                    center.y *= 1.0f / area;

                    newBody->mass = area;
                    newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f / newBody->mass : 0.0f);
                    newBody->inertia = inertia;
                    newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f / newBody->inertia : 0.0f);

                    Vector2 pointA = newBody->position;
                    Vector2 pointB = Vector2Subtract(newData.positions[1], newData.positions[0]);
                    pointB.x /= 2.0f;
                    pointB.y /= 2.0f;
                    Vector2 forceDirection = Vector2Subtract(Vector2Add(pointA, Vector2Add(newData.positions[0], pointB)), newBody->position);
                    MathNormalize(&forceDirection);
                    forceDirection.x *= force;
                    forceDirection.y *= force;

                    PhysicsAddForce(newBody, forceDirection);
                }

                PHYSAC_FREE(vertices);
            }
        }
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] error when trying to shatter a null reference physics body");
#endif
}

PHYSACDEF int GetPhysicsBodiesCount(void)
{
    return physicsBodiesCount;
}

PHYSACDEF PhysicsBody GetPhysicsBody(int index)
{
    if (index < physicsBodiesCount)
    {
        if (bodies[index] == NULL)
        {
#if defined(PHYSAC_DEBUG)
            printf("[PHYSAC] error when trying to get a null reference physics body");
#endif
        }
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] physics body index is out of bounds");
#endif

    return bodies[index];
}

PHYSACDEF int GetPhysicsShapeType(int index)
{
    int result = -1;

    if (index < physicsBodiesCount)
    {
        if (bodies[index] != NULL)
            result = bodies[index]->shape.type;

#if defined(PHYSAC_DEBUG)
        else
            printf("[PHYSAC] error when trying to get a null reference physics body");
#endif
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] physics body index is out of bounds");
#endif

    return result;
}

PHYSACDEF int GetPhysicsShapeVerticesCount(int index)
{
    int result = 0;

    if (index < physicsBodiesCount)
    {
        if (bodies[index] != NULL)
        {
            switch (bodies[index]->shape.type)
            {
            case PHYSICS_CIRCLE:
                result = PHYSAC_CIRCLE_VERTICES;
                break;
            case PHYSICS_POLYGON:
                result = bodies[index]->shape.vertexData.vertexCount;
                break;
            default:
                break;
            }
        }
#if defined(PHYSAC_DEBUG)
        else
            printf("[PHYSAC] error when trying to get a null reference physics body");
#endif
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] physics body index is out of bounds");
#endif

    return result;
}

PHYSACDEF Vector2 GetPhysicsShapeVertex(PhysicsBody body, int vertex)
{
    Vector2 position = {0.0f, 0.0f};

    if (body != NULL)
    {
        switch (body->shape.type)
        {
        case PHYSICS_CIRCLE:
        {
            position.x = body->position.x + cosf(360.0f / PHYSAC_CIRCLE_VERTICES * vertex * PHYSAC_DEG2RAD) * body->shape.radius;
            position.y = body->position.y + sinf(360.0f / PHYSAC_CIRCLE_VERTICES * vertex * PHYSAC_DEG2RAD) * body->shape.radius;
        }
        break;
        case PHYSICS_POLYGON:
        {
            PolygonData vertexData = body->shape.vertexData;
            position = Vector2Add(body->position, Mat2MultiplyVector2(body->shape.transform, vertexData.positions[vertex]));
        }
        break;
        default:
            break;
        }
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] error when trying to get a null reference physics body");
#endif

    return position;
}

PHYSACDEF void SetPhysicsBodyRotation(PhysicsBody body, float radians)
{
    if (body != NULL)
    {
        body->orient = radians;

        if (body->shape.type == PHYSICS_POLYGON)
            body->shape.transform = Mat2Radians(radians);
    }
}

PHYSACDEF void DestroyPhysicsBody(PhysicsBody body)
{
    if (body != NULL)
    {
        int id = body->id;
        int index = -1;

        for (int i = 0; i < physicsBodiesCount; i++)
        {
            if (bodies[i]->id == id)
            {
                index = i;
                break;
            }
        }

        if (index == -1)
        {
#if defined(PHYSAC_DEBUG)
            printf("[PHYSAC] Not possible to find body id %i in pointers array\n", id);
#endif
            return;
        }

        PHYSAC_FREE(body);
        usedMemory -= sizeof(PhysicsBodyData);
        bodies[index] = NULL;

        for (int i = index; i < physicsBodiesCount; i++)
        {
            if ((i + 1) < physicsBodiesCount)
                bodies[i] = bodies[i + 1];
        }

        physicsBodiesCount--;

#if defined(PHYSAC_DEBUG)
        printf("[PHYSAC] destroyed physics body id %i\n", id);
#endif
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] error trying to destroy a null referenced body\n");
#endif
}

PHYSACDEF void ClosePhysics(void)
{

    physicsThreadEnabled = false;

#if defined(_WIN32) && defined(PHYSAC_THREADS)
    if (physicsThreadId != physicsThreadId)
        WaitForSingleObject(physicsThreadId, INFINITE);
#endif

    for (int i = physicsManifoldsCount - 1; i >= 0; i--)
        DestroyPhysicsManifold(contacts[i]);

    for (int i = physicsBodiesCount - 1; i >= 0; i--)
        DestroyPhysicsBody(bodies[i]);

#if defined(PHYSAC_DEBUG)
    if (physicsBodiesCount > 0 || usedMemory != 0)
        printf("[PHYSAC] physics module closed with %i still allocated bodies [MEMORY: %i bytes]\n", physicsBodiesCount, usedMemory);
    else if (physicsManifoldsCount > 0 || usedMemory != 0)
        printf("[PHYSAC] physics module closed with %i still allocated manifolds [MEMORY: %i bytes]\n", physicsManifoldsCount, usedMemory);
    else
        printf("[PHYSAC] physics module closed successfully\n");
#endif
}

static int FindAvailableBodyIndex()
{
    int index = -1;
    for (int i = 0; i < PHYSAC_MAX_BODIES; i++)
    {
        int currentId = i;

        for (int k = 0; k < physicsBodiesCount; k++)
        {
            if (bodies[k]->id == currentId)
            {
                currentId++;
                break;
            }
        }

        if (currentId == i)
        {
            index = i;
            break;
        }
    }

    return index;
}

static PolygonData CreateRandomPolygon(float radius, int sides)
{
    PolygonData data = {0};
    data.vertexCount = sides;

    for (int i = 0; i < data.vertexCount; i++)
    {
        data.positions[i].x = cosf(360.0f / sides * i * PHYSAC_DEG2RAD) * radius;
        data.positions[i].y = sinf(360.0f / sides * i * PHYSAC_DEG2RAD) * radius;
    }

    for (int i = 0; i < data.vertexCount; i++)
    {
        int nextIndex = (((i + 1) < sides) ? (i + 1) : 0);
        Vector2 face = Vector2Subtract(data.positions[nextIndex], data.positions[i]);

        data.normals[i] = (Vector2){face.y, -face.x};
        MathNormalize(&data.normals[i]);
    }

    return data;
}

static PolygonData CreateRectanglePolygon(Vector2 pos, Vector2 size)
{
    PolygonData data = {0};
    data.vertexCount = 4;

    data.positions[0] = (Vector2){pos.x + size.x / 2, pos.y - size.y / 2};
    data.positions[1] = (Vector2){pos.x + size.x / 2, pos.y + size.y / 2};
    data.positions[2] = (Vector2){pos.x - size.x / 2, pos.y + size.y / 2};
    data.positions[3] = (Vector2){pos.x - size.x / 2, pos.y - size.y / 2};

    for (int i = 0; i < data.vertexCount; i++)
    {
        int nextIndex = (((i + 1) < data.vertexCount) ? (i + 1) : 0);
        Vector2 face = Vector2Subtract(data.positions[nextIndex], data.positions[i]);

        data.normals[i] = (Vector2){face.y, -face.x};
        MathNormalize(&data.normals[i]);
    }

    return data;
}

#if defined(_WIN32)
static DWORD WINAPI PhysicsLoop(LPVOID arg)
#else
static void *PhysicsLoop(void *arg)
#endif
{
#if defined(PHYSAC_DEBUG)
    printf("[PHYSAC] physics thread created successfully\n");
#endif

    physicsThreadEnabled = true;

    while (physicsThreadEnabled)
    {
        RunPhysicsStep();
    }

    return NULL;
}

static void PhysicsStep(void)
{

    stepsCount++;

    for (int i = physicsManifoldsCount - 1; i >= 0; i--)
    {
        PhysicsManifold manifold = contacts[i];

        if (manifold != NULL)
            DestroyPhysicsManifold(manifold);
    }

    for (int i = 0; i < physicsBodiesCount; i++)
    {
        PhysicsBody body = bodies[i];
        body->isGrounded = false;
    }

    for (int i = 0; i < physicsBodiesCount; i++)
    {
        PhysicsBody bodyA = bodies[i];

        if (bodyA != NULL)
        {
            for (int j = i + 1; j < physicsBodiesCount; j++)
            {
                PhysicsBody bodyB = bodies[j];

                if (bodyB != NULL)
                {
                    if ((bodyA->inverseMass == 0) && (bodyB->inverseMass == 0))
                        continue;

                    PhysicsManifold manifold = CreatePhysicsManifold(bodyA, bodyB);
                    SolvePhysicsManifold(manifold);

                    if (manifold->contactsCount > 0)
                    {

                        PhysicsManifold newManifold = CreatePhysicsManifold(bodyA, bodyB);
                        newManifold->penetration = manifold->penetration;
                        newManifold->normal = manifold->normal;
                        newManifold->contacts[0] = manifold->contacts[0];
                        newManifold->contacts[1] = manifold->contacts[1];
                        newManifold->contactsCount = manifold->contactsCount;
                        newManifold->restitution = manifold->restitution;
                        newManifold->dynamicFriction = manifold->dynamicFriction;
                        newManifold->staticFriction = manifold->staticFriction;
                    }
                }
            }
        }
    }

    for (int i = 0; i < physicsBodiesCount; i++)
    {
        PhysicsBody body = bodies[i];

        if (body != NULL)
            IntegratePhysicsForces(body);
    }

    for (int i = 0; i < physicsManifoldsCount; i++)
    {
        PhysicsManifold manifold = contacts[i];

        if (manifold != NULL)
            InitializePhysicsManifolds(manifold);
    }

    for (int i = 0; i < PHYSAC_COLLISION_ITERATIONS; i++)
    {
        for (int j = 0; j < physicsManifoldsCount; j++)
        {
            PhysicsManifold manifold = contacts[i];

            if (manifold != NULL)
                IntegratePhysicsImpulses(manifold);
        }
    }

    for (int i = 0; i < physicsBodiesCount; i++)
    {
        PhysicsBody body = bodies[i];

        if (body != NULL)
            IntegratePhysicsVelocity(body);
    }

    for (int i = 0; i < physicsManifoldsCount; i++)
    {
        PhysicsManifold manifold = contacts[i];

        if (manifold != NULL)
            CorrectPhysicsPositions(manifold);
    }

    for (int i = 0; i < physicsBodiesCount; i++)
    {
        PhysicsBody body = bodies[i];

        if (body != NULL)
        {
            body->force = PHYSAC_VECTOR_ZERO;
            body->torque = 0.0f;
        }
    }
}

PHYSACDEF void RunPhysicsStep(void)
{

    currentTime = GetCurrentTimePhysac();

    const double delta = currentTime - startTime;

    accumulator += delta;

    while (accumulator >= deltaTime)
    {
        PhysicsStep();
        accumulator -= deltaTime;
    }

    startTime = currentTime;
}

PHYSACDEF void SetPhysicsTimeStep(double delta)
{
    deltaTime = delta;
}

static int FindAvailableManifoldIndex()
{
    int index = -1;
    for (int i = 0; i < PHYSAC_MAX_MANIFOLDS; i++)
    {
        int currentId = i;

        for (int k = 0; k < physicsManifoldsCount; k++)
        {
            if (contacts[k]->id == currentId)
            {
                currentId++;
                break;
            }
        }

        if (currentId == i)
        {
            index = i;
            break;
        }
    }

    return index;
}

static PhysicsManifold CreatePhysicsManifold(PhysicsBody a, PhysicsBody b)
{
    PhysicsManifold newManifold = (PhysicsManifold)PHYSAC_MALLOC(sizeof(PhysicsManifoldData));
    usedMemory += sizeof(PhysicsManifoldData);

    int newId = FindAvailableManifoldIndex();
    if (newId != -1)
    {

        newManifold->id = newId;
        newManifold->bodyA = a;
        newManifold->bodyB = b;
        newManifold->penetration = 0;
        newManifold->normal = PHYSAC_VECTOR_ZERO;
        newManifold->contacts[0] = PHYSAC_VECTOR_ZERO;
        newManifold->contacts[1] = PHYSAC_VECTOR_ZERO;
        newManifold->contactsCount = 0;
        newManifold->restitution = 0.0f;
        newManifold->dynamicFriction = 0.0f;
        newManifold->staticFriction = 0.0f;

        contacts[physicsManifoldsCount] = newManifold;
        physicsManifoldsCount++;
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] new physics manifold creation failed because there is any available id to use\n");
#endif

    return newManifold;
}

static void DestroyPhysicsManifold(PhysicsManifold manifold)
{
    if (manifold != NULL)
    {
        int id = manifold->id;
        int index = -1;

        for (int i = 0; i < physicsManifoldsCount; i++)
        {
            if (contacts[i]->id == id)
            {
                index = i;
                break;
            }
        }

        if (index == -1)
        {
#if defined(PHYSAC_DEBUG)
            printf("[PHYSAC] Not possible to manifold id %i in pointers array\n", id);
#endif
            return;
        }

        PHYSAC_FREE(manifold);
        usedMemory -= sizeof(PhysicsManifoldData);
        contacts[index] = NULL;

        for (int i = index; i < physicsManifoldsCount; i++)
        {
            if ((i + 1) < physicsManifoldsCount)
                contacts[i] = contacts[i + 1];
        }

        physicsManifoldsCount--;
    }
#if defined(PHYSAC_DEBUG)
    else
        printf("[PHYSAC] error trying to destroy a null referenced manifold\n");
#endif
}

static void SolvePhysicsManifold(PhysicsManifold manifold)
{
    switch (manifold->bodyA->shape.type)
    {
    case PHYSICS_CIRCLE:
    {
        switch (manifold->bodyB->shape.type)
        {
        case PHYSICS_CIRCLE:
            SolveCircleToCircle(manifold);
            break;
        case PHYSICS_POLYGON:
            SolveCircleToPolygon(manifold);
            break;
        default:
            break;
        }
    }
    break;
    case PHYSICS_POLYGON:
    {
        switch (manifold->bodyB->shape.type)
        {
        case PHYSICS_CIRCLE:
            SolvePolygonToCircle(manifold);
            break;
        case PHYSICS_POLYGON:
            SolvePolygonToPolygon(manifold);
            break;
        default:
            break;
        }
    }
    break;
    default:
        break;
    }

    if (!manifold->bodyB->isGrounded)
        manifold->bodyB->isGrounded = (manifold->normal.y < 0);
}

static void SolveCircleToCircle(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    Vector2 normal = Vector2Subtract(bodyB->position, bodyA->position);

    float distSqr = MathLenSqr(normal);
    float radius = bodyA->shape.radius + bodyB->shape.radius;

    if (distSqr >= radius * radius)
    {
        manifold->contactsCount = 0;
        return;
    }

    float distance = sqrtf(distSqr);
    manifold->contactsCount = 1;

    if (distance == 0.0f)
    {
        manifold->penetration = bodyA->shape.radius;
        manifold->normal = (Vector2){1.0f, 0.0f};
        manifold->contacts[0] = bodyA->position;
    }
    else
    {
        manifold->penetration = radius - distance;
        manifold->normal = (Vector2){normal.x / distance, normal.y / distance};
        manifold->contacts[0] = (Vector2){manifold->normal.x * bodyA->shape.radius + bodyA->position.x, manifold->normal.y * bodyA->shape.radius + bodyA->position.y};
    }

    if (!bodyA->isGrounded)
        bodyA->isGrounded = (manifold->normal.y < 0);
}

static void SolveCircleToPolygon(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    SolveDifferentShapes(manifold, bodyA, bodyB);
}

static void SolvePolygonToCircle(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    SolveDifferentShapes(manifold, bodyB, bodyA);

    manifold->normal.x *= -1.0f;
    manifold->normal.y *= -1.0f;
}

static void SolveDifferentShapes(PhysicsManifold manifold, PhysicsBody bodyA, PhysicsBody bodyB)
{
    manifold->contactsCount = 0;

    Vector2 center = bodyA->position;
    center = Mat2MultiplyVector2(Mat2Transpose(bodyB->shape.transform), Vector2Subtract(center, bodyB->position));

    float separation = -PHYSAC_FLT_MAX;
    int faceNormal = 0;
    PolygonData vertexData = bodyB->shape.vertexData;

    for (int i = 0; i < vertexData.vertexCount; i++)
    {
        float currentSeparation = MathDot(vertexData.normals[i], Vector2Subtract(center, vertexData.positions[i]));

        if (currentSeparation > bodyA->shape.radius)
            return;

        if (currentSeparation > separation)
        {
            separation = currentSeparation;
            faceNormal = i;
        }
    }

    Vector2 v1 = vertexData.positions[faceNormal];
    int nextIndex = (((faceNormal + 1) < vertexData.vertexCount) ? (faceNormal + 1) : 0);
    Vector2 v2 = vertexData.positions[nextIndex];

    if (separation < PHYSAC_EPSILON)
    {
        manifold->contactsCount = 1;
        Vector2 normal = Mat2MultiplyVector2(bodyB->shape.transform, vertexData.normals[faceNormal]);
        manifold->normal = (Vector2){-normal.x, -normal.y};
        manifold->contacts[0] = (Vector2){manifold->normal.x * bodyA->shape.radius + bodyA->position.x, manifold->normal.y * bodyA->shape.radius + bodyA->position.y};
        manifold->penetration = bodyA->shape.radius;
        return;
    }

    float dot1 = MathDot(Vector2Subtract(center, v1), Vector2Subtract(v2, v1));
    float dot2 = MathDot(Vector2Subtract(center, v2), Vector2Subtract(v1, v2));
    manifold->penetration = bodyA->shape.radius - separation;

    if (dot1 <= 0.0f)
    {
        if (DistSqr(center, v1) > bodyA->shape.radius * bodyA->shape.radius)
            return;

        manifold->contactsCount = 1;
        Vector2 normal = Vector2Subtract(v1, center);
        normal = Mat2MultiplyVector2(bodyB->shape.transform, normal);
        MathNormalize(&normal);
        manifold->normal = normal;
        v1 = Mat2MultiplyVector2(bodyB->shape.transform, v1);
        v1 = Vector2Add(v1, bodyB->position);
        manifold->contacts[0] = v1;
    }
    else if (dot2 <= 0.0f)
    {
        if (DistSqr(center, v2) > bodyA->shape.radius * bodyA->shape.radius)
            return;

        manifold->contactsCount = 1;
        Vector2 normal = Vector2Subtract(v2, center);
        v2 = Mat2MultiplyVector2(bodyB->shape.transform, v2);
        v2 = Vector2Add(v2, bodyB->position);
        manifold->contacts[0] = v2;
        normal = Mat2MultiplyVector2(bodyB->shape.transform, normal);
        MathNormalize(&normal);
        manifold->normal = normal;
    }
    else
    {
        Vector2 normal = vertexData.normals[faceNormal];

        if (MathDot(Vector2Subtract(center, v1), normal) > bodyA->shape.radius)
            return;

        normal = Mat2MultiplyVector2(bodyB->shape.transform, normal);
        manifold->normal = (Vector2){-normal.x, -normal.y};
        manifold->contacts[0] = (Vector2){manifold->normal.x * bodyA->shape.radius + bodyA->position.x, manifold->normal.y * bodyA->shape.radius + bodyA->position.y};
        manifold->contactsCount = 1;
    }
}

static void SolvePolygonToPolygon(PhysicsManifold manifold)
{
    if ((manifold->bodyA == NULL) || (manifold->bodyB == NULL))
        return;

    PhysicsShape bodyA = manifold->bodyA->shape;
    PhysicsShape bodyB = manifold->bodyB->shape;
    manifold->contactsCount = 0;

    int faceA = 0;
    float penetrationA = FindAxisLeastPenetration(&faceA, bodyA, bodyB);

    if (penetrationA >= 0.0f)
        return;

    int faceB = 0;
    float penetrationB = FindAxisLeastPenetration(&faceB, bodyB, bodyA);

    if (penetrationB >= 0.0f)
        return;

    int referenceIndex = 0;
    bool flip = false;

    PhysicsShape refPoly;
    PhysicsShape incPoly;

    if (BiasGreaterThan(penetrationA, penetrationB))
    {
        refPoly = bodyA;
        incPoly = bodyB;
        referenceIndex = faceA;
    }
    else
    {
        refPoly = bodyB;
        incPoly = bodyA;
        referenceIndex = faceB;
        flip = true;
    }

    Vector2 incidentFace[2];
    FindIncidentFace(&incidentFace[0], &incidentFace[1], refPoly, incPoly, referenceIndex);

    PolygonData refData = refPoly.vertexData;
    Vector2 v1 = refData.positions[referenceIndex];
    referenceIndex = (((referenceIndex + 1) < refData.vertexCount) ? (referenceIndex + 1) : 0);
    Vector2 v2 = refData.positions[referenceIndex];

    v1 = Mat2MultiplyVector2(refPoly.transform, v1);
    v1 = Vector2Add(v1, refPoly.body->position);
    v2 = Mat2MultiplyVector2(refPoly.transform, v2);
    v2 = Vector2Add(v2, refPoly.body->position);

    Vector2 sidePlaneNormal = Vector2Subtract(v2, v1);
    MathNormalize(&sidePlaneNormal);

    Vector2 refFaceNormal = {sidePlaneNormal.y, -sidePlaneNormal.x};
    float refC = MathDot(refFaceNormal, v1);
    float negSide = MathDot(sidePlaneNormal, v1) * -1;
    float posSide = MathDot(sidePlaneNormal, v2);

    if (Clip((Vector2){-sidePlaneNormal.x, -sidePlaneNormal.y}, negSide, &incidentFace[0], &incidentFace[1]) < 2)
        return;

    if (Clip(sidePlaneNormal, posSide, &incidentFace[0], &incidentFace[1]) < 2)
        return;

    manifold->normal = (flip ? (Vector2){-refFaceNormal.x, -refFaceNormal.y} : refFaceNormal);

    int currentPoint = 0;
    float separation = MathDot(refFaceNormal, incidentFace[0]) - refC;

    if (separation <= 0.0f)
    {
        manifold->contacts[currentPoint] = incidentFace[0];
        manifold->penetration = -separation;
        currentPoint++;
    }
    else
        manifold->penetration = 0.0f;

    separation = MathDot(refFaceNormal, incidentFace[1]) - refC;

    if (separation <= 0.0f)
    {
        manifold->contacts[currentPoint] = incidentFace[1];
        manifold->penetration += -separation;
        currentPoint++;

        manifold->penetration /= currentPoint;
    }

    manifold->contactsCount = currentPoint;
}

static void IntegratePhysicsForces(PhysicsBody body)
{
    if ((body == NULL) || (body->inverseMass == 0.0f) || !body->enabled)
        return;

    body->velocity.x += (body->force.x * body->inverseMass) * (deltaTime / 2.0);
    body->velocity.y += (body->force.y * body->inverseMass) * (deltaTime / 2.0);

    if (body->useGravity)
    {
        body->velocity.x += gravityForce.x * (deltaTime / 1000 / 2.0);
        body->velocity.y += gravityForce.y * (deltaTime / 1000 / 2.0);
    }

    if (!body->freezeOrient)
        body->angularVelocity += body->torque * body->inverseInertia * (deltaTime / 2.0);
}

static void InitializePhysicsManifolds(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    manifold->restitution = sqrtf(bodyA->restitution * bodyB->restitution);
    manifold->staticFriction = sqrtf(bodyA->staticFriction * bodyB->staticFriction);
    manifold->dynamicFriction = sqrtf(bodyA->dynamicFriction * bodyB->dynamicFriction);

    for (int i = 0; i < manifold->contactsCount; i++)
    {

        Vector2 radiusA = Vector2Subtract(manifold->contacts[i], bodyA->position);
        Vector2 radiusB = Vector2Subtract(manifold->contacts[i], bodyB->position);

        Vector2 crossA = MathCross(bodyA->angularVelocity, radiusA);
        Vector2 crossB = MathCross(bodyB->angularVelocity, radiusB);

        Vector2 radiusV = {0.0f, 0.0f};
        radiusV.x = bodyB->velocity.x + crossB.x - bodyA->velocity.x - crossA.x;
        radiusV.y = bodyB->velocity.y + crossB.y - bodyA->velocity.y - crossA.y;

        if (MathLenSqr(radiusV) < (MathLenSqr((Vector2){gravityForce.x * deltaTime / 1000, gravityForce.y * deltaTime / 1000}) + PHYSAC_EPSILON))
            manifold->restitution = 0;
    }
}

static void IntegratePhysicsImpulses(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    if (fabs(bodyA->inverseMass + bodyB->inverseMass) <= PHYSAC_EPSILON)
    {
        bodyA->velocity = PHYSAC_VECTOR_ZERO;
        bodyB->velocity = PHYSAC_VECTOR_ZERO;
        return;
    }

    for (int i = 0; i < manifold->contactsCount; i++)
    {

        Vector2 radiusA = Vector2Subtract(manifold->contacts[i], bodyA->position);
        Vector2 radiusB = Vector2Subtract(manifold->contacts[i], bodyB->position);

        Vector2 radiusV = {0.0f, 0.0f};
        radiusV.x = bodyB->velocity.x + MathCross(bodyB->angularVelocity, radiusB).x - bodyA->velocity.x - MathCross(bodyA->angularVelocity, radiusA).x;
        radiusV.y = bodyB->velocity.y + MathCross(bodyB->angularVelocity, radiusB).y - bodyA->velocity.y - MathCross(bodyA->angularVelocity, radiusA).y;

        float contactVelocity = MathDot(radiusV, manifold->normal);

        if (contactVelocity > 0.0f)
            return;

        float raCrossN = MathCrossVector2(radiusA, manifold->normal);
        float rbCrossN = MathCrossVector2(radiusB, manifold->normal);

        float inverseMassSum = bodyA->inverseMass + bodyB->inverseMass + (raCrossN * raCrossN) * bodyA->inverseInertia + (rbCrossN * rbCrossN) * bodyB->inverseInertia;

        float impulse = -(1.0f + manifold->restitution) * contactVelocity;
        impulse /= inverseMassSum;
        impulse /= (float)manifold->contactsCount;

        Vector2 impulseV = {manifold->normal.x * impulse, manifold->normal.y * impulse};

        if (bodyA->enabled)
        {
            bodyA->velocity.x += bodyA->inverseMass * (-impulseV.x);
            bodyA->velocity.y += bodyA->inverseMass * (-impulseV.y);

            if (!bodyA->freezeOrient)
                bodyA->angularVelocity += bodyA->inverseInertia * MathCrossVector2(radiusA, (Vector2){-impulseV.x, -impulseV.y});
        }

        if (bodyB->enabled)
        {
            bodyB->velocity.x += bodyB->inverseMass * (impulseV.x);
            bodyB->velocity.y += bodyB->inverseMass * (impulseV.y);

            if (!bodyB->freezeOrient)
                bodyB->angularVelocity += bodyB->inverseInertia * MathCrossVector2(radiusB, impulseV);
        }

        radiusV.x = bodyB->velocity.x + MathCross(bodyB->angularVelocity, radiusB).x - bodyA->velocity.x - MathCross(bodyA->angularVelocity, radiusA).x;
        radiusV.y = bodyB->velocity.y + MathCross(bodyB->angularVelocity, radiusB).y - bodyA->velocity.y - MathCross(bodyA->angularVelocity, radiusA).y;

        Vector2 tangent = {radiusV.x - (manifold->normal.x * MathDot(radiusV, manifold->normal)), radiusV.y - (manifold->normal.y * MathDot(radiusV, manifold->normal))};
        MathNormalize(&tangent);

        float impulseTangent = -MathDot(radiusV, tangent);
        impulseTangent /= inverseMassSum;
        impulseTangent /= (float)manifold->contactsCount;

        float absImpulseTangent = fabs(impulseTangent);

        if (absImpulseTangent <= PHYSAC_EPSILON)
            return;

        Vector2 tangentImpulse = {0.0f, 0.0f};
        if (absImpulseTangent < impulse * manifold->staticFriction)
            tangentImpulse = (Vector2){tangent.x * impulseTangent, tangent.y * impulseTangent};
        else
            tangentImpulse = (Vector2){tangent.x * -impulse * manifold->dynamicFriction, tangent.y * -impulse * manifold->dynamicFriction};

        if (bodyA->enabled)
        {
            bodyA->velocity.x += bodyA->inverseMass * (-tangentImpulse.x);
            bodyA->velocity.y += bodyA->inverseMass * (-tangentImpulse.y);

            if (!bodyA->freezeOrient)
                bodyA->angularVelocity += bodyA->inverseInertia * MathCrossVector2(radiusA, (Vector2){-tangentImpulse.x, -tangentImpulse.y});
        }

        if (bodyB->enabled)
        {
            bodyB->velocity.x += bodyB->inverseMass * (tangentImpulse.x);
            bodyB->velocity.y += bodyB->inverseMass * (tangentImpulse.y);

            if (!bodyB->freezeOrient)
                bodyB->angularVelocity += bodyB->inverseInertia * MathCrossVector2(radiusB, tangentImpulse);
        }
    }
}

static void IntegratePhysicsVelocity(PhysicsBody body)
{
    if ((body == NULL) || !body->enabled)
        return;

    body->position.x += body->velocity.x * deltaTime;
    body->position.y += body->velocity.y * deltaTime;

    if (!body->freezeOrient)
        body->orient += body->angularVelocity * deltaTime;

    Mat2Set(&body->shape.transform, body->orient);

    IntegratePhysicsForces(body);
}

static void CorrectPhysicsPositions(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    Vector2 correction = {0.0f, 0.0f};
    correction.x = (max(manifold->penetration - PHYSAC_PENETRATION_ALLOWANCE, 0.0f) / (bodyA->inverseMass + bodyB->inverseMass)) * manifold->normal.x * PHYSAC_PENETRATION_CORRECTION;
    correction.y = (max(manifold->penetration - PHYSAC_PENETRATION_ALLOWANCE, 0.0f) / (bodyA->inverseMass + bodyB->inverseMass)) * manifold->normal.y * PHYSAC_PENETRATION_CORRECTION;

    if (bodyA->enabled)
    {
        bodyA->position.x -= correction.x * bodyA->inverseMass;
        bodyA->position.y -= correction.y * bodyA->inverseMass;
    }

    if (bodyB->enabled)
    {
        bodyB->position.x += correction.x * bodyB->inverseMass;
        bodyB->position.y += correction.y * bodyB->inverseMass;
    }
}

static Vector2 GetSupport(PhysicsShape shape, Vector2 dir)
{
    float bestProjection = -PHYSAC_FLT_MAX;
    Vector2 bestVertex = {0.0f, 0.0f};
    PolygonData data = shape.vertexData;

    for (int i = 0; i < data.vertexCount; i++)
    {
        Vector2 vertex = data.positions[i];
        float projection = MathDot(vertex, dir);

        if (projection > bestProjection)
        {
            bestVertex = vertex;
            bestProjection = projection;
        }
    }

    return bestVertex;
}

static float FindAxisLeastPenetration(int *faceIndex, PhysicsShape shapeA, PhysicsShape shapeB)
{
    float bestDistance = -PHYSAC_FLT_MAX;
    int bestIndex = 0;

    PolygonData dataA = shapeA.vertexData;

    for (int i = 0; i < dataA.vertexCount; i++)
    {

        Vector2 normal = dataA.normals[i];
        Vector2 transNormal = Mat2MultiplyVector2(shapeA.transform, normal);

        Mat2 buT = Mat2Transpose(shapeB.transform);
        normal = Mat2MultiplyVector2(buT, transNormal);

        Vector2 support = GetSupport(shapeB, (Vector2){-normal.x, -normal.y});

        Vector2 vertex = dataA.positions[i];
        vertex = Mat2MultiplyVector2(shapeA.transform, vertex);
        vertex = Vector2Add(vertex, shapeA.body->position);
        vertex = Vector2Subtract(vertex, shapeB.body->position);
        vertex = Mat2MultiplyVector2(buT, vertex);

        float distance = MathDot(normal, Vector2Subtract(support, vertex));

        if (distance > bestDistance)
        {
            bestDistance = distance;
            bestIndex = i;
        }
    }

    *faceIndex = bestIndex;
    return bestDistance;
}

static void FindIncidentFace(Vector2 *v0, Vector2 *v1, PhysicsShape ref, PhysicsShape inc, int index)
{
    PolygonData refData = ref.vertexData;
    PolygonData incData = inc.vertexData;

    Vector2 referenceNormal = refData.normals[index];

    referenceNormal = Mat2MultiplyVector2(ref.transform, referenceNormal);
    referenceNormal = Mat2MultiplyVector2(Mat2Transpose(inc.transform), referenceNormal);

    int incidentFace = 0;
    float minDot = PHYSAC_FLT_MAX;

    for (int i = 0; i < incData.vertexCount; i++)
    {
        float dot = MathDot(referenceNormal, incData.normals[i]);

        if (dot < minDot)
        {
            minDot = dot;
            incidentFace = i;
        }
    }

    *v0 = Mat2MultiplyVector2(inc.transform, incData.positions[incidentFace]);
    *v0 = Vector2Add(*v0, inc.body->position);
    incidentFace = (((incidentFace + 1) < incData.vertexCount) ? (incidentFace + 1) : 0);
    *v1 = Mat2MultiplyVector2(inc.transform, incData.positions[incidentFace]);
    *v1 = Vector2Add(*v1, inc.body->position);
}

static int Clip(Vector2 normal, float clip, Vector2 *faceA, Vector2 *faceB)
{
    int sp = 0;
    Vector2 out[2] = {*faceA, *faceB};

    float distanceA = MathDot(normal, *faceA) - clip;
    float distanceB = MathDot(normal, *faceB) - clip;

    if (distanceA <= 0.0f)
        out[sp++] = *faceA;

    if (distanceB <= 0.0f)
        out[sp++] = *faceB;

    if ((distanceA * distanceB) < 0.0f)
    {

        float alpha = distanceA / (distanceA - distanceB);
        out[sp] = *faceA;
        Vector2 delta = Vector2Subtract(*faceB, *faceA);
        delta.x *= alpha;
        delta.y *= alpha;
        out[sp] = Vector2Add(out[sp], delta);
        sp++;
    }

    *faceA = out[0];
    *faceB = out[1];

    return sp;
}

static bool BiasGreaterThan(float valueA, float valueB)
{
    return (valueA >= (valueB * 0.95f + valueA * 0.01f));
}

static Vector2 TriangleBarycenter(Vector2 v1, Vector2 v2, Vector2 v3)
{
    Vector2 result = {0.0f, 0.0f};

    result.x = (v1.x + v2.x + v3.x) / 3;
    result.y = (v1.y + v2.y + v3.y) / 3;

    return result;
}

static void InitTimer(void)
{
    srand(time(NULL));

#if defined(_WIN32)
    QueryPerformanceFrequency((unsigned long long int *)&frequency);
#endif

#if defined(__linux__)
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) == 0)
        frequency = 1000000000;
#endif

#if defined(__APPLE__)
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    frequency = (timebase.denom * 1e9) / timebase.numer;
#endif

    baseTime = GetTimeCount();
    startTime = GetCurrentTimePhysac();
}

static uint64_t GetTimeCount(void)
{
    uint64_t value = 0;

#if defined(_WIN32)
    QueryPerformanceCounter((unsigned long long int *)&value);
#endif

#if defined(__linux__)
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    value = (uint64_t)now.tv_sec * (uint64_t)1000000000 + (uint64_t)now.tv_nsec;
#endif

#if defined(__APPLE__)
    value = mach_absolute_time();
#endif

    return value;
}

static double GetCurrentTimePhysac(void)
{
    return (double)(GetTimeCount() - baseTime) / frequency * 1000;
}

static inline Vector2 MathCross(float value, Vector2 vector)
{
    return (Vector2){-value * vector.y, value * vector.x};
}

static inline float MathCrossVector2(Vector2 v1, Vector2 v2)
{
    return (v1.x * v2.y - v1.y * v2.x);
}

static inline float MathLenSqr(Vector2 vector)
{
    return (vector.x * vector.x + vector.y * vector.y);
}

static inline float MathDot(Vector2 v1, Vector2 v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

static inline float DistSqr(Vector2 v1, Vector2 v2)
{
    Vector2 dir = Vector2Subtract(v1, v2);
    return MathDot(dir, dir);
}

static void MathNormalize(Vector2 *vector)
{
    float length, ilength;

    Vector2 aux = *vector;
    length = sqrtf(aux.x * aux.x + aux.y * aux.y);

    if (length == 0)
        length = 1.0f;

    ilength = 1.0f / length;

    vector->x *= ilength;
    vector->y *= ilength;
}

static Mat2 Mat2Radians(float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);

    return (Mat2){c, -s, s, c};
}

static void Mat2Set(Mat2 *matrix, float radians)
{
    float cos = cosf(radians);
    float sin = sinf(radians);

    matrix->m00 = cos;
    matrix->m01 = -sin;
    matrix->m10 = sin;
    matrix->m11 = cos;
}

static inline Mat2 Mat2Transpose(Mat2 matrix)
{
    return (Mat2){matrix.m00, matrix.m10, matrix.m01, matrix.m11};
}

static inline Vector2 Mat2MultiplyVector2(Mat2 matrix, Vector2 vector)
{
    return (Vector2){matrix.m00 * vector.x + matrix.m01 * vector.y, matrix.m10 * vector.x + matrix.m11 * vector.y};
}