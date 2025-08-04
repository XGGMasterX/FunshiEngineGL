#include "RuntimeException.h"
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#endif
using namespace std;
RuntimeException::RuntimeException(const string& msg)
    : std::runtime_error(msg),
    stackTrace(generateStackTrace()) {
}

std::string RuntimeException::getStackTrace() const {
    std::ostringstream oss;
    oss << "Exception: " << what() << "\nStack Trace:\n";

    for (const auto& frame : stackTrace) {
        oss << "\tat " << frame << "\n";
    }

    return oss.str();
}

std::vector<std::string> RuntimeException::generateStackTrace() {
    std::vector<std::string> trace;

#ifdef _WIN32
    void* stack[100];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD frames = CaptureStackBackTrace(0, 100, stack, NULL);
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (WORD i = 1; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        trace.push_back(symbol->Name);
    }
    free(symbol);
#else
    void* array[100];
    int size = backtrace(array, 100);
    char** symbols = backtrace_symbols(array, size);

    if (symbols) {
        for (int i = 1; i < size; i++) {
            Dl_info info;
            if (dladdr(array[i], &info) && info.dli_sname) {
                int status;
                char* demangled = abi::__cxa_demangle(info.dli_sname, NULL, NULL, &status);
                trace.push_back(demangled ? demangled : info.dli_sname);
                free(demangled);
            }
            else {
                trace.push_back(symbols[i]);
            }
        }
        free(symbols);
    }
#endif

    return trace;
}