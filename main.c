// cl .\main.c user32.lib gdi32.lib winmm.lib

#include <windows.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

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

void insert_string_to_textbox(HWND textbox_handle, wchar_t* text);

/* WINDOWs (& input) */

LRESULT CALLBACK WindowsProcessMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			int virtual_key_code = pKeyboard->vkCode;

			if (virtual_key_code == 220) { // §

			}
			printf("key pressed: %d\n", virtual_key_code);
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
}

// this is the new main function called by windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {

	AllocConsole(); // Allocate a new console for the calling process.
	freopen("CONOUT$", "w", stdout); // Redirect stdout to the console.

	static WNDCLASS window_class = { 0 };
	static const wchar_t window_class_name[] = L"My Window Class";
	window_class.lpszClassName = (PCSTR)window_class_name;
	window_class.lpfnWndProc = WindowsProcessMessage;
	window_class.hInstance = hInstance;

	typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALEXT)(int interval);

	PFNWGLSWAPINTERVALEXT wglSwapIntervalEXT;

	RegisterClass(&window_class);

	HWND window_handle =
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
	HWND textbox_handle = CreateWindow(TEXT("Edit"), TEXT(""), ES_MULTILINE | WS_CHILD | WS_VISIBLE, 0, 0, window_width, window_height, window_handle, NULL, NULL, NULL);
	SendMessage(textbox_handle, WM_SETFONT, (WPARAM)hFont, TRUE);

	ShowWindow(window_handle, nCmdShow);
	InvalidateRect(window_handle, NULL, TRUE);
	UpdateWindow(window_handle);

	clock_t start, end, delta_time;
	int frame     = 1;
	int frame_100 = 1;
	int fps = 0;
	int fps_cap = 30;

	while (!window_quit) {

		start = clock();

		frame++;
		if (frame > 1) {
			frame_100 += 1;
			frame = 1;
		}
		if (frame_100 >  50) {
			frame_100 = 1;
		}


		static MSG message = { 0 };
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

	


		for (int y = 0; y < grid_height; y++) {
			for (int x = 0; x < grid_width-2; x++) {
				string_grid[y*grid_width+x] = 34 + y+x + frame_100;
				//string_grid[y*grid_width+x] = ' ';
			}
			string_grid[y * grid_width + grid_width-2]   = '\r';
			string_grid[y * grid_width + grid_width - 1] = '\n';
		}
		//string_grid[(int)(cos(frame_100) * grid_height * grid_height + sin(frame_100) * grid_width)] = '*';
		SendMessage(textbox_handle, WM_SETTEXT, 0, (LPARAM)"");
		insert_string_to_textbox((HWND)textbox_handle, (wchar_t*)string_grid);
		
		main_loop();	

		double frame_time_diff = ((clock() - start) / (double)CLOCKS_PER_SEC * 1000.0) - (1000.0 / (double)fps_cap);
		if (frame_time_diff < 0) {
			Sleep(-frame_time_diff);
		}
		
		//eller:
		//int delay = start + 15 - clock();
		//if (delay > 0) Sleep(delay);

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


void insert_string_to_textbox(HWND textbox_handle, wchar_t* text) {
	SendMessage(textbox_handle, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);  // Set selection to the end of the text
	SendMessage(textbox_handle, EM_REPLACESEL, FALSE, (LPARAM)text); // Replace selection with the specified text
}

/* MAIN */

int main_loop() {

	

	return 0;
}