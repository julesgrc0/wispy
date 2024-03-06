#include "smooth.h"

void smooth_vec(Vector2 *position, Vector2 target, float move) {
  smooth_float(position->x, target.x, move);
  smooth_float(position->y, target.y, move);
}

void smooth_rect(Rectangle *box, Rectangle target, float move) {
  smooth_float(box->x, target.x, move);
  smooth_float(box->y, target.y, move);
}
void smooth_camera(w_camera *camera, float move) {
#if defined(WISPY_ANDROID)
  smooth_float(camera->position.x, camera->target_position.x, move);
  smooth_float(camera->position.y, camera->target_position.y, move);
#else
  smooth_float(camera_x(camera), camera->target_position.x, move);
  smooth_float(camera_y(camera), camera->target_position.y, move);
#endif
}
Vector2 vec_block_round(Vector2 vec) {
  return VEC(roundf(vec.x / (float)CUBE_W) * (float)CUBE_W,
             roundf(vec.y / (float)CUBE_H) * (float)CUBE_H);
}