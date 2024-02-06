#include "view.h"

Rectangle get_center_box_from_camera(Camera2D camera, Rectangle box)
{
	return (Rectangle){
		.x = camera.target.x + ((RENDER_W * (1.f / camera.zoom) - box.width) / 2.f),
		.y = camera.target.y + ((RENDER_H * (1.f / camera.zoom) - box.height) / 2.f),
		.width = box.width,
		.height = box.height};
}

Rectangle get_view_from_camera(Camera2D camera)
{
	return (Rectangle){
		.x = camera.target.x, +((RENDER_W * (1.f / camera.zoom)) / 2.f), .y = camera.target.y, +((RENDER_H * (1.f / camera.zoom)) / 2.f), .width = RENDER_W * (1.f / camera.zoom), .height = RENDER_H * (1.f / camera.zoom)};
}
