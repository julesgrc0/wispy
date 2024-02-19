#include "view.h"

Vector2 center_camera_on_object(Camera2D *camera, Rectangle box) {

  return (Vector2){.x = box.x - (RENDER_W - box.width) / 2.f,
                   .y = box.y - (RENDER_H - box.height) / 2.f};
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

void smooth_vec(Vector2 *position, Vector2 target, float move) {
  smooth_float(position->x, target.x, move);
  smooth_float(position->y, target.y, move);
}

void smooth_rect(Rectangle *box, Rectangle target, float move) {
  smooth_float(box->x, target.x, move);
  smooth_float(box->y, target.y, move);
}

Vector2 get_mouse_block_center(Camera2D *camera) {
  Vector2 mouse = Vector2Add(VEC(FORMAT_W(GetMouseX()), FORMAT_H(GetMouseY())),
                             camera->target);
  return Vector2Subtract(mouse, (Vector2){(CUBE_W / 2), (CUBE_H / 2)});
}

Vector2 vec_block_round(Vector2 vec) {
  return (Vector2){
      .x = round(vec.x / (float)CUBE_W) * (float)CUBE_W,
      .y = round(vec.y / (float)CUBE_H) * (float)CUBE_H,
  };
}
