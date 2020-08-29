#ifndef WIN_HEADER_H
#define WIN_HEADER_H

#include <windows.h>
#include <stdint.h>
#include <cmath>
#include <string>

#define internal static 
#define local_persist static 
#define global_variable static

#define Pi32 3.14159265359f

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

struct win32_window_dimension
{
	int Width;
	int Height;
};

struct win32_offscreen_buffer
{
	// Little Indian architecture so in memory color is stored as BB GG RR XX
	BITMAPINFO Info; // color and dimension information about DIB
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
};

#endif 
