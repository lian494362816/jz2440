#ifndef __MY_ERROR_H_
#define __MY_ERROR_H_

#include "common.h"
#include "my_printf.h"

#define FG_BLACK     "\033[30m"
#define FG_RED       "\033[31m"
#define FG_GREEN     "\033[32m"
#define FG_YELLOW    "\033[33m"
#define FG_BLUE      "\033[34m"
#define FG_VIOLET    "\033[35m"
#define FG_DARKGREE  "\033[36m"
#define FG_WHITE     "\033[37m"

#define BG_BLACK     "\033[40m"
#define BG_RED       "\033[41m"
#define BG_GREEN     "\033[42m"
#define BG_YELLOW    "\033[43m"
#define BG_BLUE      "\033[44m"
#define BG_VIOLET    "\033[45m"
#define BG_DARKGREE  "\033[46m"
#define BG_WHITE     "\033[47m"

#define NONE         "\033[0m"

#define fflush(value)

#define PRINT_INFO(...) do{\
    printf(FG_GREEN);\
    printf("[%s] [%s] [%d]:","INF", __FILE__, __LINE__);\
    printf(__VA_ARGS__);\
    printf(BG_BLACK);\
    printf(NONE);\
    fflush(NULL);\
}while(0)

#define PRINT_WRN(...) do{\
    printf(FG_YELLOW);\
    printf("[%s] [%s] [%d]:","WAR", __FILE__, __LINE__);\
    printf(__VA_ARGS__);\
    printf(BG_BLACK);\
    printf(NONE);\
    fflush(NULL);\
}while(0)

#define PRINT_ERR(...) do{\
    printf(FG_RED);\
    printf("[%s] [%s] [%d]:","ERR", __FILE__, __LINE__);\
    printf(__VA_ARGS__);\
    printf(BG_BLACK);\
    printf(NONE);\
    fflush(NULL);\
}while(0)

#define BUG_ON(...) do{\
    printf(FG_RED);\
    printf("[%s] [%s] [%s] [%d]:","BUG", __FILE__, __FUNCTION__, __LINE__);\
    printf(__VA_ARGS__);\
    printf(BG_BLACK);\
    printf(NONE);\
    fflush(NULL);\
    while(1);\
}while(0)


#endif //MY_ERROR_H_
