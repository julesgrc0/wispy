#pragma once
#include "../../stdafx.h"

#if defined(WISPY_WINDOWS) || defined(WISPY_LINUX)
#define camera_x(camera) (camera->matrix[12])
#define camera_y(camera) (camera->matrix[13])
#endif

typedef struct w_camera {
#if defined(WISPY_ANDROID)
  Vector2 position;
#else
  float *matrix;
#endif
  Vector2 target_position;
} w_camera;

w_camera *create_camera(float x, float y);
void destroy_camera(w_camera *camera);

void set_camera_center(w_camera *camera, Vector2 center);
Vector2 get_camera_center(w_camera *camera);
Vector2 get_camera_object_center(w_camera *camera, Rectangle object);

Rectangle get_camera_view(w_camera *camera);
Rectangle get_camera_view_with_gap(w_camera *camera);


#if defined(WISPY_ANDROID)
Rectangle get_rectangle_camera(Rectangle rect, w_camera *camera);
Vector2 get_vector_camera(Vector2 vec, w_camera *camera);
#else
void add_camera_vec(w_camera *camera, Vector2 vec);
void set_camera_vec(w_camera *camera, Vector2 vec);
Vector2 get_camera_vec(w_camera *camera);

void begin_camera(w_camera *camera);
void end_camera();
#endif