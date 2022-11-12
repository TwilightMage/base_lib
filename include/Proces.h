#pragma once

#include "framework.h"
#include "List.h"
#include "Name.h"

class String;
class Path;

class EXPORT Process
{
public:
    enum class ErrorType
    {
        None,
        InputPipeCreation,
        InputPipeSetInfo,
        OutputPipeCreation,
        OutputPipeSetInfo,
        ErrorPipeCreation,
        ErrorPipeSetInfo,
        ProcessCreation
    };

    Process();
    Process(const Path& path, const List<String>& args);

    bool is_alive() const;
    void wait();
    void stop();
    FORCEINLINE ErrorType get_error() const { return dataBlock->error; }

    String read_new_output();
    String read_new_errors();
    void input_string(const String& input);

private:
    struct DataBlock
    {
        ~DataBlock();
        
        Path path;
        List<String> args;
        ErrorType error;

        HANDLE inputPipe = NULL;
        HANDLE outputPipe = NULL;
        HANDLE errorPipe = NULL;

        PROCESS_INFORMATION processInfo;

        bool isValid;
    };

    FORCEINLINE bool is_valid() const { return dataBlock != nullptr && dataBlock->isValid && dataBlock->error == ErrorType::None; }
    
    Shared<DataBlock> dataBlock;
};