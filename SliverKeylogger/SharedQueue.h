#pragma once
#include "pch.h"
#include <queue>
#include <mutex>
class SharedQueue
{
public:
	DWORD Count();
	bool empty();
	std::string convertToMultiByte(std::wstring& Value);
	void Push(std::string& Value);
	void Push(std::string&& Value);
	void Push(std::wstring& Value);
	void Push(std::wstring&& Value);
	std::wstring Pop();

private:
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::mutex queue_mutex{};
	std::queue<std::wstring> _queue{};
	DWORD _count{ 0 };
};

