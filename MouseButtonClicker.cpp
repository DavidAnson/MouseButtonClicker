// MouseButtonClicker - http://blogs.msdn.com/Delay
//
// "MouseButtonClicker clicks the mouse so you don't have to!"
//
// Simple Windows utility to click the primary mouse button whenever the mouse
// stops moving as a usability convenience/enhancement.


#include <windows.h>
#include <tchar.h>
#include <malloc.h>
#include <strsafe.h>

// Macro for compile-time assert
#define COMPILE_TIME_ASSERT(e) typedef int CTA[(e) ? 1 : -1]

// Constants for code simplification
#define RI_ALL_MOUSE_BUTTONS_DOWN (RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_5_DOWN)
#define RI_ALL_MOUSE_BUTTONS_UP (RI_MOUSE_BUTTON_1_UP | RI_MOUSE_BUTTON_2_UP | RI_MOUSE_BUTTON_3_UP | RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_5_UP)

// Check that bit-level assumptions are correct
COMPILE_TIME_ASSERT(RI_MOUSE_BUTTON_1_DOWN == (RI_MOUSE_BUTTON_1_UP >> 1));
COMPILE_TIME_ASSERT(RI_MOUSE_BUTTON_2_DOWN == (RI_MOUSE_BUTTON_2_UP >> 1));
COMPILE_TIME_ASSERT(RI_MOUSE_BUTTON_3_DOWN == (RI_MOUSE_BUTTON_3_UP >> 1));
COMPILE_TIME_ASSERT(RI_MOUSE_BUTTON_4_DOWN == (RI_MOUSE_BUTTON_4_UP >> 1));
COMPILE_TIME_ASSERT(RI_MOUSE_BUTTON_5_DOWN == (RI_MOUSE_BUTTON_5_UP >> 1));
COMPILE_TIME_ASSERT(RI_ALL_MOUSE_BUTTONS_DOWN == (RI_ALL_MOUSE_BUTTONS_UP >> 1));

// Macro for absolute value
#define ABS(v) ((0 <= v) ? v : -v)

// Application-level constants
#define APPLICATION_NAME (TEXT("MouseButtonClicker"))
#define TIMER_EVENT_ID (1)
#define MOUSE_MOVE_THRESHOLD (2)

// Window procedure
LRESULT CALLBACK WndProc(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
	// Tracks the mouse move delta threshold across calls to WndProc
	static LONG lLastClickDeltaX = 0;
	static LONG lLastClickDeltaY = 0;
	static bool fOkToClick = false;

	switch (message)
	{
	// Raw input message
	case WM_INPUT:
		{
			// Query for required buffer size
			UINT cbSize = 0;
			if(0 == GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &cbSize, sizeof(RAWINPUTHEADER)))
			{
				// Allocate buffer on stack (falls back to heap)
				const LPVOID pData = _malloca(cbSize);
				if(NULL != pData)
				{
					// Get raw input data
					if(cbSize == GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, pData, &cbSize, sizeof(RAWINPUTHEADER)))
					{
						// Only interested in mouse input
						const RAWINPUT* const pRawInput = static_cast<LPRAWINPUT>(pData);
						if (RIM_TYPEMOUSE == pRawInput->header.dwType)
						{
							// Only interested in devices that use relative coordinates
							// Specifically, input from pens/tablets is ignored
							if(0 == (pRawInput->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
							{
								// Tracks the state of the mouse buttons across calls to WndProc
								static UINT usMouseButtonsDown = 0;

								// Update mouse delta variables
								lLastClickDeltaX += pRawInput->data.mouse.lLastX;
								lLastClickDeltaY += pRawInput->data.mouse.lLastY;

								// Enable clicking once the mouse has exceeded the threshold in any direction
								fOkToClick |= ((MOUSE_MOVE_THRESHOLD < ABS(lLastClickDeltaX)) || (MOUSE_MOVE_THRESHOLD < ABS(lLastClickDeltaY)));

								// Determine the input type
								const UINT usButtonFlags = pRawInput->data.mouse.usButtonFlags;
								if(0 == (usButtonFlags & (RI_ALL_MOUSE_BUTTONS_DOWN | RI_ALL_MOUSE_BUTTONS_UP | RI_MOUSE_WHEEL)))
								{
									// Mouse move: (Re)set click timer if no buttons down and mouse moved enough to avoid jitter
									if((0 == usMouseButtonsDown) && fOkToClick)
									{
										// Use double-click time as an indication of the user's responsiveness preference
										(void)SetTimer(hWnd, TIMER_EVENT_ID, GetDoubleClickTime(), NULL);
									}
								}
								else
								{
									// Mouse button down/up or wheel rotation: Cancel click timer
									(void)KillTimer(hWnd, TIMER_EVENT_ID);

									// Update mouse button state variable (asserts above ensure the bit manipulations are correct)
									usMouseButtonsDown |= (usButtonFlags & RI_ALL_MOUSE_BUTTONS_DOWN);
									usMouseButtonsDown &= ~((usButtonFlags & RI_ALL_MOUSE_BUTTONS_UP) >> 1);

									// Reset mouse delta and threshold variables
									lLastClickDeltaX = 0;
									lLastClickDeltaY = 0;
									fOkToClick = false;
								}
							}
						}
					}
					// Free buffer
					(void)_freea(pData);
				}
			}
		}
		break;
	// Timer message
	case WM_TIMER:
		{
			// Timeout, stop timer and click primary button
			(void)KillTimer(hWnd, TIMER_EVENT_ID);
			INPUT pInputs[2] = {0};
			pInputs[0].type = INPUT_MOUSE;
			pInputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			pInputs[1].type = INPUT_MOUSE;
			pInputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
			(void)SendInput(2, pInputs, sizeof(INPUT));

			// Reset mouse delta and threshold variables
			lLastClickDeltaX = 0;
			lLastClickDeltaY = 0;
			fOkToClick = false;
		}
		break;
	// Close message
	case WM_DESTROY:
		(void)PostQuitMessage(0);
		break;
	// Unhandled message
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	// Return value 0 indicates message was processed
	return 0;
}

// WinMain entry point
int APIENTRY _tWinMain(const HINSTANCE hInstance, const HINSTANCE hPrevInstance, const LPTSTR lpCmdLine, const int nCmdShow)
{
	// Avoid compiler warnings for unreferenced parameters
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// Create a mutex to prevent running multiple simultaneous instances
	const HANDLE mutex = CreateMutex(NULL, FALSE, APPLICATION_NAME);
	if((NULL != mutex) && (ERROR_ALREADY_EXISTS != GetLastError()))
	{
		// Register the window class
		WNDCLASS wc = {0};
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = APPLICATION_NAME;
		if(0 != RegisterClass(&wc))
		{
			// Create a message-only window to receive WM_INPUT and WM_TIMER
			const HWND hWnd = CreateWindow(APPLICATION_NAME, APPLICATION_NAME, 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_MESSAGE, NULL, hInstance, NULL);
			if (NULL != hWnd)
			{
				// Register for the mouse's raw input data
				RAWINPUTDEVICE rid = {0};
				rid.usUsagePage = 1;  // HID_DEVICE_SYSTEM_MOUSE
				rid.usUsage = 2;  // HID_DEVICE_SYSTEM_MOUSE
				rid.dwFlags = RIDEV_INPUTSINK;
				rid.hwndTarget = hWnd;
				if(RegisterRawInputDevices(&rid, 1, sizeof(rid)))
				{
					// Pump Windows messages
					MSG msg = {0};
					while (GetMessage(&msg, NULL, 0, 0))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					// Return success
					return static_cast<int>(msg.wParam);
				}
			}
		}
		// Failed to initialize, output a diagnostic message (which is not more
		// friendly because it represents a scenario that should never occur)
		TCHAR szMessage[64];
		if(SUCCEEDED(StringCchPrintf(szMessage, sizeof(szMessage)/sizeof(szMessage[0]), TEXT("Initialization failure. GetLastError=%d\r\n"), GetLastError())))
		{
			(void)MessageBox(NULL, szMessage, APPLICATION_NAME, MB_OK | MB_ICONERROR);
		}
	}
	// Return failure
	return 0;
	// By contract, Windows frees all resources as part of process exit
}