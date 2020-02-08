#ifndef SLEEP_H_INCLUDED
#define SLEEP_H_INCLUDED

#if defined(_MSC_VER)
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x) * 1000)
#endif

#endif // SLEEP_H_INCLUDED
