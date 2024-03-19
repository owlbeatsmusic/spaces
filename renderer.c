
// cl .\main.c user32.lib gdi32.lib winmm.lib

#include <windows.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "input.h"
#include "main.h"


int window_width  = 0;
int window_height = 0;

int font_width  = 16;
int font_height = 8;

typedef struct {
	char symbol;
} Cell;

int grid_width  = 124;
int grid_height = 44;

char string_grid[48*125];
Cell cell_grid[48*125];

static bool window_quit = false;

void insert_string_to_textbox_(wchar_t* text);

/* WINDOWs (& input) */

HWND window_handle;
HWND textbox_handle;

LRESULT CALLBACK WindowsProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) input_process_keycode((int)pKeyboard->vkCode);
		return CallNextHookEx(NULL, nCode, wParam, lParam);
		
	}
}


DWORD WINAPI win_main_thread_(void* data) {
	main_thread();
	return 0;
}

// this is the new main function called by windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {

	HANDLE main_thread_handle  = CreateThread(NULL, 0, win_main_thread_, NULL, 0, NULL);
	AllocConsole();
	freopen("CONOUT$", "w", stdout); // Redirect stdout to the console.

	static WNDCLASS window_class = { 0 };
	static const wchar_t window_class_name[] = L"My Window Class";
	window_class.lpszClassName = (PCSTR)window_class_name;
	window_class.lpfnWndProc = WindowsProcessMessage;
	window_class.hInstance = hInstance;

	typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALEXT)(int interval);

	PFNWGLSWAPINTERVALEXT wglSwapIntervalEXT;

	RegisterClass(&window_class);

	window_handle =
		CreateWindow((PCSTR)window_class_name, "MOWIC", WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_SIZEBOX), CW_USEDEFAULT, CW_USEDEFAULT | WM_ERASEBKGND, 1024, 768, NULL, NULL, hInstance, NULL);
	if (window_handle == NULL) {
		printf("Failed to create window handle");
		return -1; 
	}
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 255));
	SetClassLongPtr(window_handle, GCLP_HBRBACKGROUND, (LONG_PTR)brush);

	HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
	if (keyboardHook == NULL) { 
		printf("Failed to set keyboard hook");
		return -1;  
	}

	RECT clientRect;
	GetClientRect(window_handle, &clientRect);
	window_width = clientRect.right - clientRect.left;
	window_height = clientRect.bottom - clientRect.top;

	HFONT hFont = CreateFont(16, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Consolas");
	if (hFont == NULL) {
		printf("Failed to create font\n");
		return -1;
	}
	textbox_handle = CreateWindow("EDIT", "", ES_MULTILINE | WS_CHILD | WS_VISIBLE, 0, 0, window_width, window_height, window_handle, NULL, NULL, NULL);
	SendMessage(textbox_handle, WM_SETFONT, (WPARAM)hFont, TRUE);

	ShowWindow(window_handle, nCmdShow);
	InvalidateRect(window_handle, NULL, TRUE);
	UpdateWindow(window_handle);

	clock_t start, end, delta_time;
	int fps = 0;
	int fps_cap = 15;

	for (int y = 0; y < grid_height; y++) {
		for (int x = 0; x < grid_width - 2; x++) {
			//string_grid[y * grid_width + x] = 34 + y + x + frame_100;
			//cell_grid[y * grid_width + x].symbol = 34 + y + x + frame_100;
			string_grid[y * grid_width + x]      = ' ';
			cell_grid[y * grid_width + x].symbol = ' ';

		}
		string_grid[y * grid_width + grid_width - 2] = '\r';
		string_grid[y * grid_width + grid_width - 1] = '\n';
	}

	while (!window_quit) {

		start = clock();

		static MSG message = { 0 };
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		render_();	

		double frame_time_diff = ((clock() - start) / (double)CLOCKS_PER_SEC * 1000.0) - (1000.0 / (double)fps_cap);
		if (frame_time_diff < 0) {
			Sleep(-frame_time_diff);
		}

		end = clock();
		delta_time = end - start;
		if (delta_time > 0) {
			fps = CLOCKS_PER_SEC / delta_time;
		}
		printf("fps=%d\n", fps);
	}

	FreeConsole();
	UnhookWindowsHookEx(keyboardHook);

	return 0;
}

LRESULT CALLBACK WindowsProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	case WM_CTLCOLOREDIT: {
		HDC hdcEdit = (HDC)wParam;				     // Handle to the device context for the edit control
		SetTextColor(hdcEdit, RGB(255, 255, 255));   // Set text color (RGB format: Red = 255, Green = 0, Blue = 0)
		SetBkColor(hdcEdit, RGB(0, 0, 0));			 // Set background color (RGB format: Red = 255, Green = 255, Blue = 0)
		return (LRESULT)GetStockObject(NULL_BRUSH);  // Return a handle to a null brush to prevent background erasing
	}

	case WM_ERASEBKGND: {
		
	}

	case WM_CREATE: {
		break;
	}

	case WM_COMMAND:
		// Handle notifications from your text box here
		switch (LOWORD(wParam)) {
			// Handle notifications from your text box here
		default:
			return DefWindowProc(window_handle, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		window_quit = true;
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(window_handle, message, wParam, lParam);
	}

	return 0;
}

void insert_string_to_textbox_(wchar_t* text) {
	SendMessage(textbox_handle, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);  // Set selection to the end of the text
	SendMessage(textbox_handle, EM_REPLACESEL, FALSE, (LPARAM)text); // Replace selection with the specified text
}

void renderer_add_text(int x, int y, char* text) {
	for (int i = 0; i < strlen(text); i++) {
		string_grid[y*grid_width+x + i] = text[i];
		cell_grid[y * grid_width + x + i].symbol = text[i];
	}
}

/* MAIN */

int frame = 1;
int frame_100 = 1;

int render_() {

	SetFocus(window_handle);

	frame++;
	if (frame > 1) {
		frame_100 += 1;
		frame = 1;
	}
	if (frame_100 > 50) {
		frame_100 = 1;
	}
	
	//string_grid[(int)(cos(frame_100) * grid_height * grid_height + sin(frame_100) * grid_width)] = '*';
	SendMessage(textbox_handle, WM_SETTEXT, 0, (LPARAM)"");
	insert_string_to_textbox_((wchar_t*)string_grid);

	return 0;
}