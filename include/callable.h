#pragma once
#include "./interpreter.h"
#include <chrono>

class Interpreter;

class Callable {
public:
    Callable() = default;
    virtual std::any Call(Interpreter&inter,const std::vector<std::any>&args);
    virtual int getArity() const; // returns the number of expected arguments
    virtual std::string toString() const; // gives the function name and stuff
    virtual ~Callable() = default;
};

class ClockCallable : public Callable {
public:
    ClockCallable() = default;
    int getArity() const override {
        return 0;
    }

    std::any Call(Interpreter&inter,const std::vector<std::any>&args) override {
        (void)inter;
        (void)args;
        using namespace std::chrono;
        auto now_ms = duration_cast<milliseconds>(
                         system_clock::now().time_since_epoch()
                      ).count();
        // return seconds as a double
        // from unix epoch (1/1/1970 00:00)
        return static_cast<double>(now_ms) / 1000.0;
  }

    std::string toString() const override {
        return "<native fn>";
    }
    virtual ~ClockCallable() = default;
};
