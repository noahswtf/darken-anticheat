#pragma once

#ifndef d_log

#ifdef DEBUG
#define d_log(format, ...) DbgPrint(format, __VA_ARGS__)
#define d_wide_log(format, ...) DbgPrint(format, __VA_ARGS__)
#else
#define d_log(format, ...)
#define d_wide_log(format, ...)
#endif

#endif