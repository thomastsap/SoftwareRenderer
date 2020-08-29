#ifndef DRAWING_H
#define DRAWING_H

#include "win_header.h"
#include "helper_functions.h"
#include <vector>
#include "geometry.h"

struct game_offscreen_buffer
{
	// Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
};

enum drawingResult
{
	DRAWING_SUCCESS,
	DRAWING_ERROR_OUT_OF_BOUNDS
};

struct color
{
	real32 A;
	real32 R;
	real32 G;
	real32 B;
};

struct Model
{
	std::vector<vec3f> vertices;
	int numberOfVertices;
	std::vector<vec3i> faces;
	int numberOfFaces;
};

uint32 rgbaToUint32(color colorRGBA);

drawingResult
DrawPixel(game_offscreen_buffer *Buffer, int32 XPosition, int32 YPosition, uint32 Color);

drawingResult
DrawLine(game_offscreen_buffer *Buffer, int32 x0, int32 y0, int32 x1, int32 y1, color colorRGBA);

drawingResult
DrawRectangle(game_offscreen_buffer *Buffer, int32 minX, int32 minY, int32 maxX, int32 maxY, color colorRGBA);

#endif
