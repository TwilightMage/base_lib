#include <process.h>

#include "../include/base_lib/Map.h"
#include "../include/base_lib/Path.h"
#include "../include/base_lib/Proces.h"

Process::Process()
    : dataBlock(nullptr)
{
}

Process::Process(const Path& path, const List<String>& args)
{
    dataBlock = MakeShared<DataBlock>();

    dataBlock->path = path;
    dataBlock->args = args;

    String command = path.get_absolute_string() + String::join(args, "");

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE inputPipe_R;
    HANDLE outputPipe_W;
    HANDLE errorPipe_W;

    if (!CreatePipe(&inputPipe_R, &dataBlock->inputPipe, &saAttr, 0))
    {
        dataBlock->error = ErrorType::InputPipeCreation;
        return;
    }

    if (!SetHandleInformation(dataBlock->inputPipe, HANDLE_FLAG_INHERIT, 0))
    {
        dataBlock->error = ErrorType::InputPipeSetInfo;
        return;
    }

    if (!CreatePipe(&dataBlock->outputPipe, &outputPipe_W, &saAttr, 0))
    {
        dataBlock->error = ErrorType::OutputPipeCreation;
        return;
    }

    if (!SetHandleInformation(dataBlock->outputPipe, HANDLE_FLAG_INHERIT, 0))
    {
        dataBlock->error = ErrorType::OutputPipeSetInfo;
        return;
    }

    if (!CreatePipe(&dataBlock->errorPipe, &errorPipe_W, &saAttr, 0))
    {
        dataBlock->error = ErrorType::ErrorPipeCreation;
        return;
    }

    if (!SetHandleInformation(dataBlock->errorPipe, HANDLE_FLAG_INHERIT, 0))
    {
        dataBlock->error = ErrorType::ErrorPipeSetInfo;
        return;
    }

    ZeroMemory(&dataBlock->processInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = errorPipe_W;
    siStartInfo.hStdOutput = outputPipe_W;
    siStartInfo.hStdInput = inputPipe_R;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    bool success = CreateProcess(NULL,
      command.begin(),          // command line
      NULL,                     // process security attributes
      NULL,                     // primary thread security attributes
      TRUE,                     // handles are inherited
      0,                        // creation flags
      NULL,                     // use parent's environment
      NULL,                     // use parent's current directory
      &siStartInfo,             // STARTUPINFO pointer
      &dataBlock->processInfo); // receives PROCESS_INFORMATION

    if (!success)
    {
        dataBlock->error = ErrorType::ProcessCreation;
        return;
    }

    CloseHandle(inputPipe_R);
    CloseHandle(outputPipe_W);
    CloseHandle(errorPipe_W);
}

int Process::call(const Path& path, const List<String>& args)
{
    return system((path.get_absolute_string() + " " + String::join(args, " ")).c());
}

bool Process::is_alive() const
{
    if (!is_valid()) return false;
    
    DWORD status;
    return GetExitCodeProcess(dataBlock->processInfo.hProcess, &status) && status == STILL_ACTIVE;
}

void Process::wait()
{
    while (is_alive()){}
}

void Process::stop()
{
    if (!is_valid()) return;
    
    TerminateProcess(dataBlock->processInfo.hProcess, 1);
    dataBlock->isValid = false;
}

String Process::read_new_output()
{
    constexpr int buffSize = 4096;
    
    String output;
    
    while (true)
    {
        char buff[buffSize];
        DWORD read;
        bool success = ReadFile(dataBlock->outputPipe, buff, 4096, &read, NULL);

        if (!success || read == 0) return output;

        output += String(buff, buffSize);
    }
}

String Process::read_new_errors()
{
    constexpr int buffSize = 4096;
    
    String output;
    
    while (true)
    {
        char buff[buffSize];
        DWORD read;
        bool success = ReadFile(dataBlock->errorPipe, buff, 4096, &read, NULL);

        if (!success || read == 0) return output;

        output += String(buff, buffSize);
    }
}

void Process::input_string(const String& input)
{
    DWORD num;
    WriteFile(dataBlock->inputPipe, input.c(), input.length(), &num, NULL);
}

Process::DataBlock::~DataBlock()
{
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    CloseHandle(inputPipe);
    CloseHandle(outputPipe);
    CloseHandle(errorPipe);
}
