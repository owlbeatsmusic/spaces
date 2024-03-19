#include <windows.h>
#include "renderer.h"

// called from renderer (start of winMain());
int main_thread() {
	while (1) {
		renderer_add_text(10, 10, "hejsan");
		Sleep(100);
	}
	return 0;
};