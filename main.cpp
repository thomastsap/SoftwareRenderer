#include <windows.h>
#include "header.h"

int softwareRendererRunning = 1;
win32_offscreen_buffer GlobalBackbuffer = {};


internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	// Resize GlobalBackbuffer 

	if (Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = Width;
	Buffer->Height = Height;

	int BytesPerPixel = 4;
	Buffer->BytesPerPixel = BytesPerPixel;

	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width; // width of the new DIB
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height; // treat bitmap as top-down (origin upper-left corner)
	Buffer->Info.bmiHeader.biPlanes = 1; // must be one
	Buffer->Info.bmiHeader.biBitCount = 32; // bits per pixel
	Buffer->Info.bmiHeader.biCompression = BI_RGB; // uncompressed format

	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Width * BytesPerPixel; // size of row in bytes

	// TODO(casey): Probably clear this to black
}

internal void
Win32DisplayCalculatedFrame(win32_offscreen_buffer *Buffer,
	HDC DeviceContext, int WindowWidth, int WindowHeight)
{
	// NOTE(casey): For prototyping purposes, we're going to always blit
	// 1-to-1 pixels to make sure we don't introduce artifacts with
	// stretching while we are learning to code the renderer!
	StretchDIBits(DeviceContext,
		0, 0, Buffer->Width, Buffer->Height,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory, // frame bits
		&Buffer->Info,
		DIB_RGB_COLORS, SRCCOPY);
}

internal win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;

	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return(Result);
}

LRESULT CALLBACK softwareRendererWindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;

	switch (uMsg)
	{
		case WM_CLOSE:
		{
			softwareRendererRunning = 0;
		} break;
		case WM_DESTROY:
		{
			softwareRendererRunning = 0;
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(hwnd, &Paint);
			win32_window_dimension Dimension = Win32GetWindowDimension(hwnd);
			Win32DisplayCalculatedFrame(&GlobalBackbuffer, DeviceContext,
				Dimension.Width, Dimension.Height);
			EndPaint(hwnd, &Paint);
		} break;
		default:
		{
			Result = DefWindowProcA(hwnd, uMsg, wParam, lParam);
		} break;
	}

	return(Result);
}

void softwareRendererPendingMessages()
{
	MSG Message;
	while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		switch (Message.message)
		{
			case WM_QUIT:
			{
				softwareRendererRunning = 0;
			} break;

			default:
			{
				// some messages will bypass the queue and thus softwareRendererWindowProcedure will be called
				TranslateMessage(&Message);
				DispatchMessageA(&Message);
			} break;
		}
	}
}

internal void
DrawPixel(game_offscreen_buffer *Buffer, int32 XPosition, int32 YPosition, uint32 Color)
{
	uint32 *Pixel = (uint32*)Buffer->Memory + YPosition * Buffer->Width + XPosition;
	(*Pixel) = Color;
}


internal void 
softwareRendererUpdateCalculateFrame(game_offscreen_buffer *Buffer)
{
	DrawPixel(Buffer, 9, 9, 0x00FFFFFF);
	DrawPixel(Buffer, 11, 11, 0x00FFFFFF);
	DrawPixel(Buffer, 12, 12, 0x00FFFFFF);
	DrawPixel(Buffer, 13, 13, 0x00FFFFFF);
}

int CALLBACK
WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode) 
{
	WNDCLASSA windowSoftwareRendererClass = {};
	//windowSoftwareRendererClass.style = CS_HREDRAW | CS_VREDRAW;
	windowSoftwareRendererClass.lpfnWndProc = softwareRendererWindowProcedure;
	// handle to the applications
	windowSoftwareRendererClass.hInstance = Instance;
	windowSoftwareRendererClass.lpszClassName = "SOFTWARE_RENDERER_CLASS";

	Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

	if (RegisterClassA(&windowSoftwareRendererClass))
	{
		HWND softwareRenderWindowHandle = CreateWindowExA(
			0,
			windowSoftwareRendererClass.lpszClassName,
			"Software Renderer",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			0, 0, 1280, 720,
			0, 0, Instance, 0);

		if (softwareRenderWindowHandle)
		{
			while (softwareRendererRunning)
			{
				softwareRendererPendingMessages();

				game_offscreen_buffer Buffer = {};
				Buffer.Memory = GlobalBackbuffer.Memory;
				Buffer.Width = GlobalBackbuffer.Width;
				Buffer.Height = GlobalBackbuffer.Height;
				Buffer.Pitch = GlobalBackbuffer.Pitch;
				Buffer.BytesPerPixel = GlobalBackbuffer.BytesPerPixel;

				softwareRendererUpdateCalculateFrame(&Buffer);

				// Before rendering the frame see if the window was resized
				win32_window_dimension Dimension = Win32GetWindowDimension(softwareRenderWindowHandle);
				HDC DeviceContext = GetDC(softwareRenderWindowHandle);
				Win32DisplayCalculatedFrame(&GlobalBackbuffer, DeviceContext, Dimension.Width, Dimension.Height);
				ReleaseDC(softwareRenderWindowHandle, DeviceContext);
			}
		}
		else
		{
			OutputDebugStringA("CreateWindowExA: failed\n");
		}
	}
	else
	{
		OutputDebugStringA("RegisterClassA: failed\n");
	}
	OutputDebugStringA("asdasdsad\n");
	return 0;
}