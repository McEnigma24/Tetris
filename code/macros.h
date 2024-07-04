#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string.h>
#include "_Game_params_.h"

using namespace std;
using namespace sf;

#define EXIT window.close(); break;

#define VAR(x) cout << #x << " = " << x << '\n';
#define log(x) cout << x << '\n';
#define logg cout << '\n';

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;