#pragma once

#ifndef d_log

#ifdef DEBUG

#define d_dpfltr_ihvdriver_id 77

#define d_log(format, ...) DbgPrintEx(d_dpfltr_ihvdriver_id, 0, format, __VA_ARGS__)
#define d_wide_log(format, ...) DbgPrintEx(d_dpfltr_ihvdriver_id, 0, format, __VA_ARGS__)
#else
#define d_log(format, ...)
#define d_wide_log(format, ...)
#endif

#endif