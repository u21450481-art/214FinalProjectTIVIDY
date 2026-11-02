#pragma once
#include <iostream>
#ifdef TIVIDY_TUI
#define TLOG(x) ((void)0)
#else
#define TLOG(x) (std::cout << x << std::endl)
#endif