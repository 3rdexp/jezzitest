#pragma once

#include "win_progress.hpp"
#include <iomanip>

void test_scheme_holder();
void test_scheme_data();

void test_herit();
void test_scheme_parse();

#ifndef ASSERT
#define ASSERT(x) if (!(x)) __asm int 3;
#endif