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

Vector2 get_collision_resolution(w_collision *bc, Rectangle self_rect,
                                 Rectangle other_rect) {
  Vector2 resolution = VEC_ZERO;
  float overlapX = 0.0f, overlapY = 0.0f;

  bc->left = self_rect.x < other_rect.x + other_rect.width &&
             self_rect.x + self_rect.width > other_rect.x;
  bc->right = self_rect.x + self_rect.width > other_rect.x &&
              self_rect.x < other_rect.x;
  bc->top = self_rect.y < other_rect.y + other_rect.height &&
            self_rect.y + self_rect.height > other_rect.y;
  bc->bottom = self_rect.y + self_rect.height > other_rect.y &&
               self_rect.y < other_rect.y;

  if (bc->left && !bc->right) {
    overlapX = other_rect.x + other_rect.width - self_rect.x;
  } else if (!bc->left && bc->right) {
    overlapX = other_rect.x - (self_rect.x + self_rect.width);
  }
  if (bc->top && !bc->bottom) {
    overlapY = other_rect.y + other_rect.height - self_rect.y;
  } else if (!bc->top && bc->bottom) {
    overlapY = other_rect.y - (self_rect.y + self_rect.height);
  }

  if (fabsf(overlapX) < fabsf(overlapY)) {
    resolution.x = overlapX;
  } else {
    resolution.y = overlapY;
  }

  return resolution;
}
