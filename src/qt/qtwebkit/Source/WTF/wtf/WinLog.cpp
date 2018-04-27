#include "WinLog.h"
#include <Windows.h>

namespace WTF
{
	WinLog* WinLog::Open(const char* filename, const char* mode)
	{
		file = fopen(filename, mode);
		setvbuf(file, 0, _IONBF, 0);
		instance->file = file;
		return instance;
	}

	WinLog* WinLog::Instance()
	{
		if (!instance)
		{
			instance = new WinLog(file);
			char fileName[100];
			strcpy(fileName, WIN_LOG_FILENAME);
			strcat(fileName, ".log");
			return Open(fileName, "wb");
		}
		return instance;
	}

	int WinLog::Write(const wchar_t* source, unsigned length)
	{
		char buffer[5];
		int count = 0;
		for (int i = 0; i < length; i++)
		{
			DWORD num = WideCharToMultiByte(
				CP_ACP,
				NULL,
				source + i,
				1,
				buffer,
				5,
				NULL,
				NULL);
			fwrite(buffer, sizeof(char), 5, file);
			count += num;
		}
		return count;
	}

	int WinLog::WriteLine(const wchar_t* source, unsigned length)
	{
		int count = Write(source, length);
		fprintf_s(file, "\n");
		return count + 1;
	}

	int WinLog::Write(const char* source, unsigned length)
	{
		fwrite(source, sizeof(char), length, file);
		return length;
	}

	int WinLog::WriteLine(const char* source, unsigned length)
	{
		fwrite(source, sizeof(char), length, file);
		fprintf_s(file, "\n");
		return length + 1;
	}

	WinLog::WinLog(FILE* pFile)
	{
		file = pFile;
	}

	void LogWriteW(const wchar_t* source, unsigned length)
	{
		WinLog::Instance()->Write(source, length);
	}
	void LogWriteA(const char* source, unsigned length)
	{
		WinLog::Instance()->Write(source, length);
	}
	void LogWriteLineW(const wchar_t* source, unsigned length)
	{
		WinLog::Instance()->WriteLine(source, length);
	}
	void LogWriteLineA(const char* source, unsigned length)
	{
		WinLog::Instance()->WriteLine(source, length);
	}
}