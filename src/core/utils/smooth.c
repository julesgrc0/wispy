#include "smooth.h"

void smooth_vec(Vector2 *position, Vector2 target, float move) {
  smooth_float(position->x, target.x, move);
  smooth_float(position->y, target.y, move);
}

void smooth_rect(Rectangle *box, Rectangle target, float move) {
  smooth_float(box->x, target.x, move);
  smooth_float(box->y, target.y, move);
}
void smooth_camera(w_camera *camera, Vector2 target, float move) {
  smooth_float(camera_x(camera), -target.x, move);
  smooth_float(camera_y(camera), -target.y, move);
}
Vector2 vec_block_round(Vector2 vec) {
  return VEC(roundf(vec.x / (float)CUBE_W) * (float)CUBE_W,
             roundf(vec.y / (float)CUBE_H) * (float)CUBE_H);
}