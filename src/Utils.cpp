#include "../include/base_lib/Utils.h"

#define GLFW_INCLUDE_NONE
#include <algorithm>

#include "../include/base_lib/Logger.h"
#include "../include/base_lib/Path.h"

#if PLATFORM_WINDOWS
#include <dbghelp.h>
#include <psapi.h>
#endif

void Utils::init_base_lib(const String& executablePath)
{
    Path::init(executablePath);
    Logger::init(Path::get_app_path());
}

List<String> Utils::get_callstack(uint offset)
{
#if PLATFORM_WINDOWS
    offset += 1;

    List<String> result;
    
    HANDLE  process;
    HANDLE  thread;

    STACKFRAME64        stack;
    ULONG               frame;    
    DWORD64             displacement;

    DWORD disp;
    IMAGEHLP_LINE64 *line;

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

    memset( &stack, 0, sizeof( STACKFRAME64 ) );

    process                = GetCurrentProcess();
    thread                 = GetCurrentThread();
    displacement           = 0;
#if !defined(_M_AMD64)
    stack.AddrPC.Offset    = (*ctx).Eip;
    stack.AddrPC.Mode      = AddrModeFlat;
    stack.AddrStack.Offset = (*ctx).Esp;
    stack.AddrStack.Mode   = AddrModeFlat;
    stack.AddrFrame.Offset = (*ctx).Ebp;
    stack.AddrFrame.Mode   = AddrModeFlat;
#endif

    CONTEXT* context = new CONTEXT();
    context->ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(context);
#ifdef _M_X64
    stack.AddrPC.Offset = context->Rip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Rsp;
    stack.AddrStack.Mode = AddrModeFlat;    
    stack.AddrFrame.Offset = context->Rbp;
    stack.AddrFrame.Mode = AddrModeFlat;
#else
    stack.AddrPC.Offset = context->Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = context->Esp;
    stack.AddrStack.Mode = AddrModeFlat;    
    stack.AddrFrame.Offset = context->Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
#endif

    SymInitialize( process, NULL, TRUE ); //load symbols

    for(frame = 0; ; frame++)
    {
        //get next call from stack
        BOOL flag = StackWalk64
        (
#if defined(_M_AMD64)
            IMAGE_FILE_MACHINE_AMD64
#else
            IMAGE_FILE_MACHINE_I386
#endif
            ,
            process,
            thread,
            &stack,
            context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );

        if(!flag) break;        

        //get symbol name for address
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
        SymFromAddr(process, stack.AddrPC.Offset, &displacement, pSymbol);

        line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
        line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);       

        //try to get line
        if (SymGetLineFromAddr64(process, stack.AddrPC.Offset - 1, &disp, line))
        {
            if (frame >= offset)
            {
                String filename = line->FileName;
                result.add(String::format("\tat %s in %s:%lu", pSymbol->Name, filename.substring(filename.index_of("Hexa")).c(), line->LineNumber));
                if (strcmp(pSymbol->Name, "main") == 0) break;
            }
        }     

        free(line);
        line = NULL;
    }

    return result;
#else
    throw NotImplementedException();
#endif
}

String Utils::get_callstack_string(uint offset)
{
    return String::join(get_callstack(offset + 1), "\n");
}

void Utils::print_callstack(String category, uint offset)
{
    print_debug(category, get_callstack_string(offset + 1));
}

bool Utils::ask_yes_no(const String& message)
{
#if PLATFORM_WINDOWS
    return MessageBox(NULL, message.wc(), L"Decide", MB_YESNO) == IDYES;
#else
    return false;
#endif
}
