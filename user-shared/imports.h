#pragma once

#ifdef _DARKEN_USER_PROCESS
#define d_import(function) function
#else
#define d_import(function) function
#endif
