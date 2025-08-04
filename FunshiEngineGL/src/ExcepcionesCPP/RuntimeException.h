#ifndef RUNTIMEEXCEPTION_H
#define RUNTIMEEXCEPTION_H

#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

class RuntimeException : public std::runtime_error {
private:
    std::vector<std::string> stackTrace;

    // Declaración correcta de los métodos privados
    std::vector<std::string> generateStackTrace();
    std::string formatStackTrace();

public:
    explicit RuntimeException(const string& msg);
    std::string getStackTrace() const;
    virtual ~RuntimeException() = default;
};

#endif