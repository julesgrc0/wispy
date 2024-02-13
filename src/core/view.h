#pragma once
#include "../stdafx.h"

#pragma pack(push, 1)
typedef struct w_collision {
  struct {
    bool left : 1;
    bool right : 1;
    bool top : 1;
    bool bottom : 1;
  };
  uint8_t all;
} w_collision;
#pragma pack(pop)

#define smooth_float(current, target, speed)                                   \
  current = (current < target) ? fmin(current + speed, target)                 \
                               : fmax(current - speed, target);

Vector2 center_camera_on_object(Camera2D *camera, Rectangle box);
Vector2 center_object_on_camera(Rectangle box, Camera2D *camera);
Rectangle get_camera_view(Camera2D *camera);

void smooth_vec(Vector2 *position, Vector2 target, float move);
void smooth_rect(Rectangle *box, Rectangle target, float move);

Vector2 get_mouse_block_center(Camera2D *camera);
Vector2 vec_block_round(Vector2 vec);
