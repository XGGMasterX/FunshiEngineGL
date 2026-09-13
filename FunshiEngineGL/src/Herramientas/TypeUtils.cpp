#include "TypeUtils.h"

#include <typeinfo>

#if defined(__GNUG__)
#include <cxxabi.h>
#include <cstdlib>
#endif

std::string demangle(const char* name) {
#if defined(__GNUG__)
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result((status == 0 && demangled) ? demangled : name);
    std::free(demangled);
    return result;
#else
    return name; // En MSVC no necesita demangling
#endif
}