#pragma once
#include<iostream>
#include<exception>

namespace cryptic::details {

inline void expects(bool condition, const char* file, int line, const char* func)
{
    if(condition) return;
    std::cerr << file << " " << line << " " << func << std::endl;
    std::terminate();
}

inline void ensures(bool condition, const char* file, int line, const char* func)
{
    if(condition) return;
    std::cerr << file << " " << line << " " << func << std::endl;
    std::terminate();
}

} // namespace cryptic::details

#ifndef __OPTIMIZE__

#ifndef Expects
#define Expects(condition) cryptic::details::expects(condition, __FILE__, __LINE__, __func__)
#endif

#ifndef Ensures
#define Ensures(condition) cryptic::details::ensures(condition, __FILE__, __LINE__, __func__)
#endif

#else
#define Expects(condition)
#define Ensures(condition)
#endif
