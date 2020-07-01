#include <windows.h>
#include "header.h"
#include "drawing.h"
#pragma comment(lib, "winmm.lib")

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
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;		// width of the new DIB
	Buffer->Info.bmiHeader.biHeight = Buffer->Height;	// treat bitmap as bottom up (origin lower-right corner)
	Buffer->Info.bmiHeader.biPlanes = 1;				// must be one
	Buffer->Info.bmiHeader.biBitCount = 32;				// bits per pixel
	Buffer->Info.bmiHeader.biCompression = BI_RGB;		// uncompressed format

	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Width * BytesPerPixel; // size of row in bytes

}

internal void
Win32DisplayCalculatedFrame(win32_offscreen_buffer *Buffer,
	HDC DeviceContext, int WindowWidth, int WindowHeight)
{
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
softwareRendererUpdateCalculateFrame(game_offscreen_buffer *Buffer)
{
	color testColor = { 0.0f, 0.0f, 0.0f, 0.5f };
	color testColor2 = { 0.0f, 1.0f, 1.0f, 1.0f };

	DrawRectangle(Buffer, 0, 0, 1280, 720, testColor);

	DrawLine(Buffer, 0, 0, 1279, 719, testColor2);
	DrawLine(Buffer, 1279, 0, 0, 719, testColor2);
	DrawLine(Buffer, 1280/2, 0, 1280/2, 719, testColor2);
	DrawLine(Buffer, 0, 720/2, 1279, 720/2, testColor2);

}

int CALLBACK
WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode) 
{

	
	/*
	UINT DesiredTimeResolution = 1; // in milliseconds
	bool32 SleepIsGranular = (timeBeginPeriod(DesiredTimeResolution) == TIMERR_NOERROR);
	if (!SleepIsGranular)
	{
		OutputDebugStringA("timeBeginPeriod: failed\n");
	}
	*/

	//counting framerate 
	LARGE_INTEGER systemCountsPerSecondLargeInteger = {};
	BOOL querySuccessful = QueryPerformanceFrequency(&systemCountsPerSecondLargeInteger);
	if (!querySuccessful)
	{
		OutputDebugStringA("QueryPerformanceCounter: failed\n");
	}
	int64 systemCountsPerSecond = systemCountsPerSecondLargeInteger.QuadPart;

	WNDCLASSA windowSoftwareRendererClass = {};
	windowSoftwareRendererClass.style = CS_HREDRAW | CS_VREDRAW;
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
			100, 100, 1296, 759,
			0, 0, Instance, 0);

		if (softwareRenderWindowHandle)
		{
			
			while (softwareRendererRunning)
			{
				// get number of cycles and number of counts at the start of the frame
				uint64 cyclesAtStartOfFrame = __rdtsc();
				LARGE_INTEGER countsAtStartOfFrame;
				QueryPerformanceCounter(&countsAtStartOfFrame);
				
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

				// get number of cycles and number of counts at the end of the frame
				uint64 cyclesAtEndOfFrame = __rdtsc();
				LARGE_INTEGER countsAtEndOfFrame;
				QueryPerformanceCounter(&countsAtEndOfFrame);

				
				uint64 cyclesPerFrame = cyclesAtEndOfFrame - cyclesAtStartOfFrame;
				uint64 CountsPerFrame = countsAtEndOfFrame.QuadPart - countsAtStartOfFrame.QuadPart;

				real64 FramesPerSecond = (real64)systemCountsPerSecond / (real64)CountsPerFrame; // FPS = (system_counts / sec) / (counts / frame) = frames / sec
				real64 MCyclesPerFrame = (real64)cyclesPerFrame / (1000.0f * 1000.0f);

				char BufferMessage[256];
				sprintf_s(BufferMessage, "Software Renderer %.02f FPS, %.02f MCPF \n", FramesPerSecond, MCyclesPerFrame);
				OutputDebugStringA(BufferMessage);
				SetWindowText(softwareRenderWindowHandle, BufferMessage);

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