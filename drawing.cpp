#include "drawing.h"

drawingResult
DrawPixel(game_offscreen_buffer *Buffer, int32 XPosition, int32 YPosition, color Color)
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
	(*Pixel) = (RoundReal32ToUInt32(Color.A * 255.0f) << 24) +
		(RoundReal32ToUInt32(Color.R * 255.0f) << 16) +
		(RoundReal32ToUInt32(Color.G * 255.0f) << 8) +
		(RoundReal32ToUInt32(Color.B * 255.0f) << 0);

	return DRAWING_SUCCESS;
}

drawingResult
DrawLine(game_offscreen_buffer *Buffer, int32 x0, int32 y0, int32 x1, int32 y1, color Color)
{
	if (!Buffer ||
		x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0 ||
		x0 > Buffer->Width || x1 > Buffer->Width ||
		y0 > Buffer->Height || y1 > Buffer->Height)
	{
		return DRAWING_ERROR_OUT_OF_BOUNDS;
	}
	//x1--;
	// test
	uint32 colorInt = ((RoundReal32ToUInt32(Color.A * 255.0f) << 24) |
		(RoundReal32ToUInt32(Color.R * 255.0f) << 16) |
		(RoundReal32ToUInt32(Color.G * 255.0f) << 8) |
		(RoundReal32ToUInt32(Color.B * 255.0f) << 0));

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
			(*Pixel) = colorInt;
			//DrawPixel(Buffer, y, x, Color);
		}
		else
		{
			//if (y >= Buffer->Height) 
			//	y = Buffer->Height - 1;
			uint32 *Pixel = (uint32*)Buffer->Memory + y * Buffer->Width + x;
			(*Pixel) = colorInt;
			//DrawPixel(Buffer, x, y, Color);
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

drawingResult DrawRectangle(game_offscreen_buffer * Buffer, int32 minX, int32 minY, int32 maxX, int32 maxY, color Color)
{
	
	if (!Buffer ||
		minX < 0 || minY < 0 || maxX < 0 || maxY < 0 ||
		minX > Buffer->Width || maxX > Buffer->Width ||
		minY > Buffer->Height || maxY > Buffer->Height)
	{
		return DRAWING_ERROR_OUT_OF_BOUNDS;
	}
	
	uint32 colorInt = ((RoundReal32ToUInt32(Color.A * 255.0f) << 24) |
		(RoundReal32ToUInt32(Color.R * 255.0f) << 16) |
		(RoundReal32ToUInt32(Color.G * 255.0f) << 8) |
		(RoundReal32ToUInt32(Color.B * 255.0f) << 0));


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
			*Pixel++ = colorInt;
		}

		Row += Buffer->Pitch;
	}
	return drawingResult();
}
