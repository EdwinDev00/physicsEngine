//------------------------------------------------------------------------------
/**
    @file core/config.h
    
	Main configure file for types and OS-specific stuff.
	
	(C) 2015-2020 See the LICENSE file.
*/
#ifdef __WIN32__
#include "win32/pch.h"
#endif

#include <stdint.h>
#include <atomic>
#include <xmmintrin.h>
#include <assert.h>

#include "../engine/core/mathLibFunction.h"

typedef uint32_t	uint32;
typedef int32_t		int32;
typedef uint16_t	uint16;
typedef int16_t		int16;
typedef uint8_t		uint8;
typedef int8_t		int8;
typedef uint8_t		uchar;

// eh, windows already defines byte, so don't redefine byte if we are running windows
#ifndef __WIN32__
typedef uint8_t      byte;
#endif

typedef uint8_t		ubyte;
typedef float		float32;
typedef double		float64;

#define j_min(x, y) x < y ? x : y
#define j_max(x, y) x > y ? x : y

#ifdef NULL
#undef NULL
#define NULL nullptr
#endif
