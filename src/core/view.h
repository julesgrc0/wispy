#pragma once
#include "../stdafx.h"

#define smooth_float(current, target, speed)                                   \
  current = (current < target) ? fmin(current + speed, target)                 \
                               : fmax(current - speed, target);

Vector2 center_camera_on_object(Camera2D *camera, Rectangle box);
Vector2 center_object_on_camera(Rectangle box, Camera2D *camera);
Rectangle get_camera_view(Camera2D *camera);
void smooth_vec(Vector2 *position, Vector2 target, float move);
void smooth_rect(Rectangle *box, Rectangle target, float move);
