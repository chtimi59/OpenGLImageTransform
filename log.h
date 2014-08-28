#ifndef LOGHEADER
#define LOGHEADER

#include <stdio.h>
#include <stdarg.h>

#define LOG_MAXDBG 3
#define LOG_MAXUSR 2
#define LOG_MAXLVL 3

#ifdef LOGENABLE

#define LOG_SET(_level_) do { setloglevel_(_level_); } while(0)

// user oriented
#define STDERR(...)    log_(stderr, 1, 0,  __FILE__, __LINE__, __VA_ARGS__)
#define STDOUT(...)    log_(stdout, 2, 0,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_MAXUSR 2
// debug Oriented
#define DBGERROR(...)  log_(stderr, 3, 1,  __FILE__, __LINE__, __VA_ARGS__)
#define DBGMSG(...)    log_(stderr, 3, 0,  __FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
void setloglevel_(int level); 
void log_(FILE* f, int level, int showline, char* file, int lineno, const char *str, ...); 
#ifdef __cplusplus
}
#endif    

#else

#define LOG_SET(_level_)
#define STDERR(...)
#define STDOUT(...)
#define DBGERROR(...)
#define DBGMSG(...)

#endif


#endif     