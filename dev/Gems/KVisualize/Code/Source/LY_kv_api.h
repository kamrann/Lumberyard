
#pragma once

// @NOTE: This is due to compilation errors caused by CryEngine redefiniion of the malloc functions, which Eigen looks for in the std namespace.
// EIGEN_NO_MALLOC alone seems to be insufficient, doesn't actually guard out code at compile time.

// @TODO: Ideally these should only span Eigen includes. LY_kantan_conversion.h includes Eigin, but CryEngine headers which should obviously have these macros defined...
#pragma push_macro("malloc")
#undef malloc
#pragma push_macro("free")
#undef free
#pragma push_macro("realloc")
#undef realloc
#pragma push_macro("calloc")
#undef calloc

#define EIGEN_NO_MALLOC
#include "kv_primitives.h"
#include "LY_kantan_conversion.h"

#pragma pop_macro("malloc")
#pragma pop_macro("free")
#pragma pop_macro("realloc")
#pragma pop_macro("calloc")

