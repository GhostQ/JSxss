#include <stdio.h>
#include <wtf/Platform.h>
#include <wtf/StdLibExtras.h>
#define WIN_LOG_FILENAME "WinLog"

namespace WTF
{	
	WTF_EXPORT_PRIVATE void LogWriteW(const wchar_t* source, unsigned length);
	WTF_EXPORT_PRIVATE void LogWriteA(const char* source, unsigned length);
	WTF_EXPORT_PRIVATE void LogWriteLineW(const wchar_t* source, unsigned length);
	WTF_EXPORT_PRIVATE void LogWriteLineA(const char* source, unsigned length);
	class WinLog
	{
	public:
		static WinLog* Instance();
		int Write(const wchar_t* source, unsigned length);
		int Write(const char* source, unsigned length);
		int WriteLine(const wchar_t* source, unsigned length);
		int WriteLine(const char* source, unsigned length);
	private:
		static WinLog* Open(const char* filename, const char* mode);
		WinLog(FILE* pFile);
		static WinLog* instance;
		static FILE* file;
	};
}

using WTF::LogWriteW;
using WTF::LogWriteA;
using WTF::LogWriteLineW;
using WTF::LogWriteLineA;