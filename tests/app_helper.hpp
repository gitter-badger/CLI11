#pragma once

#ifdef CLI_SINGLE_FILE
#include "CLI11.hpp"
#else
#include "CLI/CLI.hpp"
#endif

#include "gtest/gtest.h"
#include <iostream>

typedef std::vector<std::string> input_t;

struct TApp : public ::testing::Test {
    CLI::App app{"My Test Program"};
    input_t args;

    void run() {
        input_t newargs = args;
        std::reverse(std::begin(newargs), std::end(newargs));
        app.parse(newargs);
    }

};


class TempFile {
    std::string _name;

public:

    TempFile(std::string name) : _name(name) {
        if(!CLI::NonexistentPath(_name))
            throw std::runtime_error(_name);

    }

    ~TempFile() {
        std::remove(_name.c_str()); // Doesn't matter if returns 0 or not
    }

    operator const std::string& () const {return _name;}
    const char* c_str() const {return _name.c_str();}
};
