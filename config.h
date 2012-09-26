#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <cstdio>
#include <stdint.h>
#include <assert.h>

#ifdef _DEBUG
    #include <cstdio>
    #define LOG(format, args...) printf("TRACE:%-25s ", strerror(errno));printf(format, ##args);printf("\n");
#else
    #define LOG(format, args...)
#endif 

#endif
