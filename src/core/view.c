#include "view.h"

Vector2 center_camera_on_object(Camera2D *camera, Rectangle box) {

  return (Vector2){.x = box.x - (RENDER_W - box.width) / 2.f,
                   .y = box.y - (RENDER_W - box.width) / 2.f};
}
Vector2 center_object_on_camera(Rectangle box, Camera2D *camera) {
  return (Vector2){.x = camera->target.x + (RENDER_W - box.width) / 2,
                   .y = camera->target.y + (RENDER_H - box.height) / 2};
}

Rectangle get_camera_view(Camera2D *camera) {
  return (Rectangle){.x = camera->target.x,
                     .y = camera->target.y,
                     .width = RENDER_W,
                     .height = RENDER_H};
}