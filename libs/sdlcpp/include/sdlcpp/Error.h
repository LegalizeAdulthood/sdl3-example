#pragma once

#include <stdexcept>

namespace core::sdl
{

class Error : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace core::sdl
