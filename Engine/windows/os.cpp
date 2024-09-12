#include <engine.hpp>

extern void create_window();

#define WM_USER_SHOW_DIALOG (WM_USER + 0)

namespace os
{
	void initialize() {
		gfx.create();
	}

	void shutdown() {
		gfx.destroy();
	}

	void show_dialog(char const* title, char const* message) {
		// Wow, almost like Windows is better than Android 🤔🤔🤔
		SendNotifyMessageW(engine.window, WM_USER_SHOW_DIALOG, (WPARAM)title, (LPARAM)message);
	}
}

extern OS_MAIN();

int APIENTRY WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	create_window();

	os_create_mutex(engine.render_lock);

	DWORD thread_id {};
	engine.main_thread = CreateThread(nullptr, 0, main, nullptr, 0, &thread_id);

	BOOL bRet {};
	MSG msg;
	while ((bRet = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
		if (bRet == -1) {
			LOGI("An Error Occured!!");
			break;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	WaitForSingleObject(engine.main_thread, INFINITE);
	CloseHandle(engine.main_thread);

	os_destroy_mutex(engine.render_lock);

	return msg.wParam;
}
