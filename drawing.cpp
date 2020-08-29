#include "drawing.h"
#include "helper_functions.h"


uint32 rgbaToUint32(color colorRGBA)
{
	return	(RoundReal32ToUInt32(colorRGBA.A * 255.0f) << 24) +
			(RoundReal32ToUInt32(colorRGBA.R * 255.0f) << 16) +
			(RoundReal32ToUInt32(colorRGBA.G * 255.0f) << 8) +
			(RoundReal32ToUInt32(colorRGBA.B * 255.0f) << 0);
}

drawingResult
DrawPixel(game_offscreen_buffer *Buffer, int32 XPosition, int32 YPosition, uint32 Color)
{
	if (!Buffer ||
		(YPosition > Buffer->Height) ||
		(XPosition > Buffer->Width) ||
		YPosition < 0 ||
		XPosition < 0)
	{
		return DRAWING_ERROR_OUT_OF_BOUNDS;
	}

	uint32 *Pixel = (uint32*)Buffer->Memory + YPosition * Buffer->Width + XPosition;
	(*Pixel) = Color;

	return DRAWING_SUCCESS;
}

drawingResult
DrawLine(game_offscreen_buffer *Buffer, int32 x0, int32 y0, int32 x1, int32 y1, color colorRGBA)
{
	if (!Buffer ||
		x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0 ||
		x0 > Buffer->Width || x1 > Buffer->Width ||
		y0 > Buffer->Height || y1 > Buffer->Height)
	{
		return DRAWING_ERROR_OUT_OF_BOUNDS;
	}

	uint32 color = rgbaToUint32(colorRGBA);

	// Bresenham's Draw Lines Algorithm
	bool steep = false;
	if (abs(x0 - x1) < abs(y0 - y1))
	{
		swap(&x0, &y0);
		swap(&x1, &y1);
		steep = true;
	}
	if (x0 > x1)
	{
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	int32 dx = x1 - x0;
	int32 dy = y1 - y0;
	int derror = abs(dy) * 2;
	int error = 0;
	int32 y = y0;
	for (int32 x = x0; x <= x1; x++)
	{
		if (steep)
		{
			uint32 *Pixel = (uint32*)Buffer->Memory + x * Buffer->Width + y;
			(*Pixel) = color;
		}
		else
		{
			uint32 *Pixel = (uint32*)Buffer->Memory + y * Buffer->Width + x;
			(*Pixel) = color;
		}
		error += derror;
		if (error > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error -= dx * 2;
		}
	}


	return DRAWING_SUCCESS;
}

drawingResult DrawRectangle(game_offscreen_buffer * Buffer, int32 minX, int32 minY, int32 maxX, int32 maxY, color colorRGBA)
{
	
	if (!Buffer ||
		minX < 0 || minY < 0 || maxX < 0 || maxY < 0 ||
		minX > Buffer->Width || maxX > Buffer->Width ||
		minY > Buffer->Height || maxY > Buffer->Height)
	{
		return DRAWING_ERROR_OUT_OF_BOUNDS;
	}
	
	uint32 color = rgbaToUint32(colorRGBA);


	uint8 *Row = ((uint8 *)Buffer->Memory +
		minX * Buffer->BytesPerPixel +
		minY * Buffer->Pitch);
	for (int Y = minY;
		Y < maxY;
		++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for (int X = minX;
			X < maxX;
			++X)
		{
			*Pixel++ = color;
		}

		Row += Buffer->Pitch;
	}
	return drawingResult();
}
