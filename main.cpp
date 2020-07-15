#include <windows.h>
#include "header.h"
#include "drawing.h"
#include <vector>
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
softwareRendererUpdateCalculateFrame(game_offscreen_buffer *Buffer, Model *model)
{
	color testColor = { 0.0f, 0.168f, 0.176f, 0.184f };
	color testColor2 = { 0.0f, 1.0f, 1.0f, 1.0f };

	DrawRectangle(Buffer, 0, 0, 1280, 720, testColor);

	for (unsigned int i = 0; i < model->faces.size(); i++)
	{
		vector3i v0 = model->faces[i];
		
		vector3f v1 = model->vertices[v0.x - 1];
		vector3f v2 = model->vertices[v0.y - 1];
		vector3f v3 = model->vertices[v0.z - 1];

		int x0{}, y0{}, x1{}, y1{};

		x0 = (v1.x ) * 719 + 700 / 2;
		y0 = (v1.y ) * 719;

		x1 = (v2.x ) * 719 + 700 / 2;
		y1 = (v2.y ) * 719;

		DrawLine(Buffer, x0, y0, x1, y1, testColor2);

		x0 = (v2.x ) * 719 + 700 / 2;
		y0 = (v2.y) * 719;

		x1 = (v3.x ) * 719 + 700 / 2;
		y1 = (v3.y ) * 719;

		DrawLine(Buffer, x0, y0, x1, y1, testColor2);

		x0 = (v3.x ) * 719 + 700 / 2;
		y0 = (v3.y ) * 719;

		x1 = (v1.x ) * 719 + 700 / 2;
		y1 = (v1.y ) * 719;

		DrawLine(Buffer, x0, y0, x1, y1, testColor2);

	}

	//DrawLine(Buffer, 0, 0, 1279, 719, testColor2);
	//DrawLine(Buffer, 1279, 0, 0, 719, testColor2);
	//DrawLine(Buffer, 1280/2, 0, 1280/2, 719, testColor2);
	//DrawLine(Buffer, 0, 720/2, 1279, 720/2, testColor2);
}

// object should be located in the project file, that is in the same folder as main function
internal Model *
loadObjModel(LPWSTR modelName)
{
	if (modelName)
	{
		Model *model = (Model *)VirtualAlloc(0, sizeof(Model), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (model)
		{
			FILE * fp;
			errno_t err;
			err = _wfopen_s(&fp, modelName, L"r");
			int getfaceFormat = 0;
			enum FaceFormat 
			{
				UNDEFINED_FORMAT,
				SUPPORTED_FORMAT, // f %d/%d/%d %d/%d/%d %d/%d/%d
			} faceFormat = UNDEFINED_FORMAT;

			if (!err)
			{
				vector3f v3f;
				vector3i face;
				real32 minX{ FLT_MAX }, maxX{ -FLT_MAX }, minY{ FLT_MAX }, maxY{ -FLT_MAX }, minZ{ FLT_MAX }, maxZ{ -FLT_MAX };
				char characterRead[5]{};
				while (true)
				{
					// NOTE: fscanf ignores empty lines
					if (fscanf_s(fp, "%s", characterRead, sizeof(characterRead)) > 0)
					{
						if (strcmp(characterRead, "v") == 0)
						{
							fscanf_s(fp, " %f %f %f\n", &v3f.x, &v3f.y, &v3f.z);
							model->vertices.push_back(v3f);
							if (v3f.x < minX)
								minX = v3f.x;
							if (v3f.y < minY)
								minY = v3f.y;
							if (v3f.z < minZ)
								minZ = v3f.z;
							if (v3f.x > maxX)
								maxX = v3f.x;
							if (v3f.y > maxY)
								maxY = v3f.y;
							if (v3f.z > maxZ)
								maxZ = v3f.z;
						}
						else if (strcmp(characterRead, "f") == 0)
						{
							if (!getfaceFormat)
							{
								fpos_t filePosition = 0; // store file position in order to restore it after i specified face format
								int numberOfCharactersRead = 0;
								numberOfCharactersRead = fscanf_s(fp, " %d/%*[^ ] %d/%*[^ ] %d/%*[^\n]\n", &face.x, &face.y, &face.z);
								if (numberOfCharactersRead == 3)
								{
									model->faces.push_back(face);
									faceFormat = SUPPORTED_FORMAT;
								}
								getfaceFormat = 1;
							}
							if (faceFormat == UNDEFINED_FORMAT)
								return model;
							else if(faceFormat == SUPPORTED_FORMAT)
								fscanf_s(fp, " %d/%*[^ ] %d/%*[^ ] %d/%*[^\n]\n", &face.x, &face.y, &face.z);
								
							model->faces.push_back(face);
						}
						else
							fscanf_s(fp, "%*[^\n]\n");
					}
					else
					{
						if (strcmp(characterRead, "") == 0)
						{ 
							fscanf_s(fp, "%*[^\n]\n");
						}
						else
							break;
					}
						

				}
				fclose(fp);

				real32 r_x = maxX - minX;
				real32 r_y = maxY - minY;
				real32 r_z = maxZ - minZ;
				real32 slope_x = 1 / r_x;
				real32 slope_y = 1 / r_y;
				real32 slope_z = 1 / r_z;

				real32 max = maxX - minX;
				if ((maxY - minY) > max)
					max = (maxY - minY);
				//if ((maxZ - minZ) > max)
				//	max = (maxZ - minZ);
				
				for (int i = 0; i < model->vertices.size(); i++)
				{
					model->vertices[i].x = float(model->vertices[i].x - minX) / max; // *slope_x;
					model->vertices[i].y = (model->vertices[i].y - minY) / max; // *slope_y;
					//model->vertices[i].z = (model->vertices[i].z - minZ) / max; // *slope_z;
				}

				return model;
			}
		}
	}
	return NULL;
}


int CALLBACK
WinMain(HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode) 
{
	// get command line arguments
	LPWSTR *szArglist;
	int nArgs;
	int i;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (szArglist == NULL)
	{
		OutputDebugString("CommandLineToArgvW failed\n");
		return 0;
	}
	else
	{
		for (i = 0; i < nArgs; i++)
		{
			OutputDebugStringW(szArglist[i]);
		}
	}

	Model *model = loadObjModel(szArglist[1]);

	

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
		OutputDebugString("QueryPerformanceCounter: failed\n");
	}
	int64 systemCountsPerSecond = systemCountsPerSecondLargeInteger.QuadPart;

	WNDCLASSW windowSoftwareRendererClass = {};
	windowSoftwareRendererClass.style = CS_HREDRAW | CS_VREDRAW;
	windowSoftwareRendererClass.lpfnWndProc = softwareRendererWindowProcedure;
	// handle to the applications
	windowSoftwareRendererClass.hInstance = Instance;
	windowSoftwareRendererClass.lpszClassName = L"SOFTWARE_RENDERER_CLASS";

	Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

	if (RegisterClassW(&windowSoftwareRendererClass))
	{
		HWND softwareRenderWindowHandle = CreateWindowExW(
			0,
			windowSoftwareRendererClass.lpszClassName,
			L"Software Renderer",
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
				
				softwareRendererUpdateCalculateFrame(&Buffer, model);				

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
				OutputDebugString(BufferMessage);
				SetWindowTextW(softwareRenderWindowHandle, (LPCWSTR)BufferMessage);

				ReleaseDC(softwareRenderWindowHandle, DeviceContext);
			}
		}
		else
		{
			OutputDebugString("CreateWindowExA: failed\n");
		}
	}
	else
	{
		OutputDebugString("RegisterClassA: failed\n");
	}
	OutputDebugString("asdasdsad\n");
	return 0;
}