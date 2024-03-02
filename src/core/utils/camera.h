#pragma once
#include "../../stdafx.h"

#define camera_x(camera) (camera->matrix[12])
#define camera_y(camera) (camera->matrix[13])

typedef struct w_camera {
  float *matrix;
  Vector2 target_position;
} w_camera;

w_camera *create_camera(float x, float y);
void destroy_camera(w_camera *camera);

void add_camera_vec(w_camera *camera, Vector2 vec);
void set_camera_vec(w_camera *camera, Vector2 vec);
void set_camera_center(w_camera *camera, Vector2 vec);

Vector2 get_camera_vec(w_camera *camera);
Vector2 get_camera_center(w_camera *camera);

Rectangle get_camera_view(w_camera *camera);
Rectangle get_camera_view_with_gap(w_camera *camera);

void begin_camera(w_camera *camera);
void end_camera();
