#include "log.h"

static int loglevel_ = 2; // default value

void setloglevel_(int level) {
    loglevel_ = level;
}

void log_(FILE* f, int level, int showline, char* file, int lineno, const char *str, ...) {
    va_list argptr;

    if (level>loglevel_) return;
    va_start(argptr, str);    
    if ((level > LOG_MAXUSR) && ((level <= LOG_MAXDBG))) fprintf(f,"[DBG] ");
    if (showline) fprintf(f,"%s:%d: ", file, lineno);
    vfprintf(f,str,argptr);
    va_end(argptr);
}




// TESTU
//#define TESTU
#ifdef TESTU
int main(int argc, char** argv)
{    
    STDERR("User stderr\n");
    STDOUT("User stdout\n");
    DBGERROR("Dbg error\n");
    DBGMSG("Dbg message\n");
    
    LOG_SET(LOG_MAXLVL);
    
    STDERR("User stderr\n");
    STDOUT("User stdout\n");
    DBGERROR("Dbg error\n");
    DBGMSG("Dbg message\n");
    
    return 0;
}
#endif