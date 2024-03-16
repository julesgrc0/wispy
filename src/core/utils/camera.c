#include "camera.h"

w_camera *create_camera(float x, float y) {
  w_camera *camera = malloc(sizeof(w_camera));
  if (camera == NULL) {
    LOG("failed to allocate memory for camera");
    return NULL;
  }
  memset(camera, 0, sizeof(w_camera));
#if defined(WISPY_ANDROID)
  camera->position = VEC(x, y);
  camera->target_position = VEC(x, y);
#else
  camera->matrix = malloc(sizeof(float) * 16);
  if (camera->matrix == NULL) {
    LOG("failed to allocate memory for camera matrix");
    free(camera);
    return NULL;
  }

  Matrix m = MatrixTranslate(-x, -y, 0);
  camera->matrix[0] = m.m0;
  camera->matrix[1] = m.m1;
  camera->matrix[2] = m.m2;
  camera->matrix[3] = m.m3;
  camera->matrix[4] = m.m4;
  camera->matrix[5] = m.m5;
  camera->matrix[6] = m.m6;
  camera->matrix[7] = m.m7;
  camera->matrix[8] = m.m8;
  camera->matrix[9] = m.m9;
  camera->matrix[10] = m.m10;
  camera->matrix[11] = m.m11;
  camera->matrix[12] = m.m12;
  camera->matrix[13] = m.m13;
  camera->matrix[14] = m.m14;
  camera->matrix[15] = m.m15;

  camera->target_position = VEC(x, y);
#endif
  return camera;
}
void destroy_camera(w_camera *camera) {
  if (camera == NULL) {
    LOG("camera (null) already destroyed");
    return;
  }
  if (camera->matrix != NULL) {
    free(camera->matrix);
  }
  free(camera);
}

Vector2 get_camera_center(w_camera *camera) {
#if defined(WISPY_ANDROID)
  return VEC(camera->position.x + RENDER_W / 2,
             camera->position.y + RENDER_H / 2);
#else
  return VEC(-camera_x(camera) + RENDER_W / 2,
             -camera_y(camera) + RENDER_H / 2);
#endif
}
void set_camera_center(w_camera *camera, Vector2 center) {
#if defined(WISPY_ANDROID)
  camera->position = VEC(center.x - RENDER_W / 2, center.y - RENDER_H / 2);
#else
  camera_x(camera) = -center.x + RENDER_W / 2;
  camera_y(camera) = -center.y + RENDER_H / 2;
#endif
}

Vector2 get_camera_object_center(w_camera *camera, Rectangle object) {
#if defined(WISPY_ANDROID)
  return VEC(camera->position.x + object.x + (RENDER_W - object.width) / 2,
             camera->position.y + object.y + (RENDER_H - object.height) / 2);
#else
  return VEC(-camera_x(camera) + object.x + (RENDER_W - object.width) / 2,
             -camera_y(camera) + object.y + (RENDER_H - object.height) / 2);
#endif
}
Rectangle get_camera_view(w_camera *camera) {
#if defined(WISPY_ANDROID)
  return RECT(camera->position.x, camera->position.y, RENDER_W, RENDER_H);
#else
  return RECT(-camera_x(camera), -camera_y(camera), RENDER_W, RENDER_H);
#endif
}
Rectangle get_camera_view_with_gap(w_camera *camera) {
#if defined(WISPY_ANDROID)
  return RECT(camera->position.x - RENDER_CUBE_GAP * CUBE_W,
              camera->position.y - RENDER_CUBE_GAP * CUBE_H,
              RENDER_W + RENDER_CUBE_GAP * CUBE_W * 2,
              RENDER_H + RENDER_CUBE_GAP * CUBE_H * 2);
#else
  return RECT(-camera_x(camera) - RENDER_CUBE_GAP * CUBE_W,
              -camera_y(camera) - RENDER_CUBE_GAP * CUBE_H,
              RENDER_W + RENDER_CUBE_GAP * CUBE_W * 2,
              RENDER_H + RENDER_CUBE_GAP * CUBE_H * 2);
#endif
}

#if defined(WISPY_ANDROID)
Rectangle get_rect_to_camera(Rectangle rect, w_camera *camera) {
  return RECT(rect.x - camera->position.x, rect.y - camera->position.y,
              rect.width, rect.height);
}
#else
void add_camera_vec(w_camera *camera, Vector2 vec) {
  camera_x(camera) -= vec.x;
  camera_y(camera) -= vec.y;
}
void set_camera_vec(w_camera *camera, Vector2 vec) {
  camera_x(camera) = -vec.x;
  camera_y(camera) = -vec.y;
}
Vector2 get_camera_vec(w_camera *camera) {
  return VEC(-camera_x(camera), -camera_y(camera));
}

void begin_camera(w_camera *camera) {
  rlDrawRenderBatchActive();
  rlLoadIdentity();
  rlMultMatrixf(camera->matrix);
}
void end_camera() { EndMode2D(); }
#endif