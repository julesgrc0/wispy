#pragma once
#include "../../stdafx.h"
#include "camera.h"

#define smooth_float(current, target, speed)                                   \
  current = ((float)current < (float)target)                                   \
                ? fminf((float)current + (float)speed, (float)target)          \
                : fmaxf((float)current - (float)speed, (float)target);

void smooth_vec(Vector2 *position, Vector2 target, float move);
void smooth_rect(Rectangle *box, Rectangle target, float move);
void smooth_camera(w_camera *camera, float move);

Vector2 vec_block_round(Vector2 vec);
