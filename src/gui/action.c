#include "action.h"

w_guiaction *create_action(w_guicontext *ctx, Vector2 position, float size,
                           Texture texture) {
  w_guiaction *action = malloc(sizeof(w_guiaction));
  if (action == NULL)
    return NULL;
  memset(action, 0, sizeof(w_guiaction));
  action->ctx = ctx;
  action->position = Vector2SubtractValue(position, size);
  action->size = size;
  action->icon = texture;

  return action;
}

bool update_action(w_guiaction *act) {
#ifdef __ANDROID__
  bool clicked = check_collision_touch(act->position, act->size);
#else
    bool clicked = IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
                   CheckCollisionPointCircle(GetMousePosition(), act->position,
                                             act->size);
#endif

  DrawCircle(act->position.x, act->position.y, act->size,
             Fade(clicked ? BLACK : WHITE, 0.3f));
  DrawCircle(act->position.x, act->position.y, act->size * 0.9f,
             Fade(clicked ? BLACK : WHITE, 0.3f));
  DrawTexturePro(
      act->icon, (Rectangle){0, 0, act->icon.width, act->icon.height},
      (Rectangle){act->position.x - act->size / 2,
                  act->position.y - act->size / 2, act->size, act->size},
      (Vector2){0, 0}, 0, BLACK);

  return clicked;
}

void destroy_action(w_guiaction *act) {
  if (act) {
    free(act);
  }
}