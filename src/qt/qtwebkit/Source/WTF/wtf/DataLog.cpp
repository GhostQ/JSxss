/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "DataLog.h"
#include <stdarg.h>
#include <wtf/FilePrintStream.h>
#include <wtf/WTFThreadData.h>
#include <wtf/Threading.h>

#include <Windows.h>

#if OS(UNIX)
#include <unistd.h>
#endif

#if OS(WINCE)
#ifndef _IONBF
#define _IONBF 0x0004
#endif
#endif

#define DATA_LOG_TO_FILE 1

// Uncomment to force logging to the given file regardless of what the environment variable says. Note that
// we will append ".<pid>.txt" where <pid> is the PID.

// This path won't work on Windows, make sure to change to something like C:\\Users\\<more path>\\log.txt.
#define DATA_LOG_FILENAME "WTFLog"

namespace WTF {

#if USE(PTHREADS)
static pthread_once_t initializeLogFileOnceKey = PTHREAD_ONCE_INIT;
#endif

static FilePrintStream* file;

static void initializeLogFileOnce()
{
#if DATA_LOG_TO_FILE
#ifdef DATA_LOG_FILENAME
    const char* filename = DATA_LOG_FILENAME;
#else
    const char* filename = getenv("WTF_DATA_LOG_FILENAME");
#endif
    char actualFilename[1024];

    _snprintf(actualFilename, sizeof(actualFilename), "%s.%d.txt", filename, GetCurrentProcessId());


    if (filename) {
        file = FilePrintStream::open(actualFilename, "w").leakPtr();
        if (!file)
            fprintf(stderr, "Warning: Could not open log file %s for writing.\n", actualFilename);
    }
#endif // DATA_LOG_TO_FILE
    if (!file)
        file = new FilePrintStream(stderr, FilePrintStream::Borrow);
    
    setvbuf(file->file(), 0, _IONBF, 0); // Prefer unbuffered output, so that we get a full log upon crash or deadlock.
}

static void initializeLogFile()
{
#if USE(PTHREADS)
    pthread_once(&initializeLogFileOnceKey, initializeLogFileOnce);
#else
    if (!file)
        initializeLogFileOnce();
#endif
}

int UseBuiltInOutput;

enum BuiltInJSType
{
	None,
	JQDev,
	JQ,
	AngDev,
	Ang
};

BuiltInJSType ConfirmJSType()
{
	return BuiltInJSType::None;
}

FilePrintStream& dataFile()
{
    initializeLogFile();
    return *file;
}

void dataLogFV(const char* format, va_list argList)
{
    dataFile().vprintf(format, argList);
}

void dataLogF(const char* format, ...)
{
	if (UseBuiltInOutput)
		return;
    va_list argList;
    va_start(argList, format);
    dataLogFV(format, argList);
    va_end(argList);
}

void dataLogWin(const unsigned char* format, unsigned length)
{
	dataLogWin((char*)format, length);
}

void dataLogWin(const char* format, unsigned length)
{
	FILE* file = dataFile().file();
	fwrite(format, sizeof(char), length, file);
}

void dataLogWin(const wchar_t* format, unsigned length)
{
	FILE* file = dataFile().file();
	char buffer[5];
	for (int i = 0; i < length; i++)
	{
		DWORD num = WideCharToMultiByte(
			CP_ACP,
			NULL,
			format + i,
			1,
			buffer,
			5,
			NULL,
			NULL);
		fwrite(buffer, sizeof(char), num, file);
	}
}

void dataLogWin(String format)
{
	if (UseBuiltInOutput)
		return;
	if (format.isNull()) return;
	if (format.is8Bit())
		dataLogWin(format.characters8(), format.length());
	else
		dataLogWin(format.characters16(), format.length());
}

void dataLogC(const char* format)
{
	DWORD dw;
	WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), format, strlen(format), &dw, NULL);
}

void dataLogC(String format)
{
	if (format.isNull()) return;
	DWORD dw;
	if (format.is8Bit())
		WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), format.characters8(), format.length(), &dw, NULL);
	else
		WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), format.characters16(), format.length(), &dw, NULL);
}

void dataLogFString(const char* str)
{
    dataFile().printf("%s", str);
}

static bool dataLogEnabled;

int enabled = 0;

bool enabledDataLog()
{
	return enabled;
}

void enableDataLog()
{
	enabled++;
}

void disableDataLog()
{
	enabled--;
}

JSC::ExecState** frameStack;
JSC::ExecState** currentStack;
int depth = 0;

int currentStackDepth()
{
	return depth;
}

void initCallFrameStack()
{
	if (!frameStack)
	{
		frameStack = (JSC::ExecState**)malloc(999*sizeof(JSC::ExecState*));
		currentStack = frameStack - 1;
	}
}

void pushCallFrame(JSC::ExecState* callFrame)
{
	ASSERT(depth < 998);
	initCallFrameStack();
	currentStack++;
	*currentStack = callFrame;
	depth++;
}

JSC::ExecState* popCallFrame()
{
	ASSERT(depth > 0);
	depth--;
	return *(currentStack--);
}

JSC::ExecState* peekCallFrame()
{
	if (depth == 0)
		return nullptr;
	return *currentStack;
}

JSC::ExecState* peekPrevCallFrame()
{
	if (depth <= 1)
		return nullptr;
	return *(currentStack - 1);
}

int lPushDepth = 0;

int lastPushDepth()
{
	return lPushDepth;
}

void setPushDepth()
{
	lPushDepth = depth;
}

static String preData[] = {
	String("____")
};

void compareDefault(String data)
{
	size_t thash = data.hash();
	if(thash == preData[0].hash())
	{
		dataLogWin(data);
		UseBuiltInOutput = 1;
	}
}

} // namespace WTF

