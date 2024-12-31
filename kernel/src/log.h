#pragma once

#ifndef d_log

#ifdef DEBUG

#define d_dpfltr_ihvdriver_id 77

// not going to manually import these, as in production we will not be in debug mode
// if you require debug prints in production then manually import them
#define d_log(format, ...) DbgPrintEx(d_dpfltr_ihvdriver_id, 0, format, __VA_ARGS__)
#define d_wide_log(format, ...) DbgPrintEx(d_dpfltr_ihvdriver_id, 0, format, __VA_ARGS__)
#else
#define d_log(format, ...)
#define d_wide_log(format, ...)
#endif

#endif