#ifndef THROWABLE_H
#define THROWABLE_H

#include <stdexcept>
#include <string>

// Equivalente a Throwable en Java pero con runtime
class Throwable : public std::runtime_error {
protected:
    std::string message;
    std::string stackTrace;

public:
    Throwable(const std::string& msg = "") : std::runtime_error("Error gráfico: " + msg) {
        // Aquí podrías capturar el stack trace (usando librerías como backward-cpp)
    }

    virtual const char* what() const noexcept override {
        return message.c_str();
    }

    virtual const char* getStackTrace() const {
        return stackTrace.c_str();
    }

    virtual ~Throwable() = default;
};


#endif
