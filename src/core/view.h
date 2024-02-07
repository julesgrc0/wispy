#pragma once
#include "../stdafx.h"

#define smooth_camera(camera, player, speed)                                   \
  camera = (camera < player) ? fmin(camera + speed, player)                    \
                             : fmax(camera - speed, player);

Vector2 center_camera_on_object(Camera2D *camera, Rectangle box);
Vector2 center_object_on_camera(Rectangle box, Camera2D *camera);
Rectangle get_camera_view(Camera2D *camera);
