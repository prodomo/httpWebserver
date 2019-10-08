#include <stdio.h>
#include <time.h>

/* usr define */
#define LOG_LEVEL LOG_LEVEL_DBG /*more higher more detail*/
#define LOG_WITH_MODULE_PREFIX 1 /*print prefix or not*/

/* The different log levels available */
#define LOG_LEVEL_NONE         0 /* No log */
#define LOG_LEVEL_ERR          1 /* Errors */
#define LOG_LEVEL_WARN         2 /* Warnings */
#define LOG_LEVEL_INFO         3 /* Basic info */
#define LOG_LEVEL_DBG          4 /* Detailled debug */

/*funcitons*/
static inline char *timenow();

#define LOG_OUTPUT(...) fprintf(stdout, __VA_ARGS__)
#define LOG_OUTPUT_PREFIX(level, levelstr) LOG_OUTPUT("[%-4s] %s ", levelstr, timenow())

#define LOG(newline, level, levelstr, ...) do {  \
                            if(level <= (LOG_LEVEL)) { \
                              if(newline) { \
                                if(LOG_WITH_MODULE_PREFIX) { \
                                  LOG_OUTPUT_PREFIX(level, levelstr); \
                                } \
                              } \
                              LOG_OUTPUT(__VA_ARGS__); \
                            } \
                          } while (0)

                          
                          
/* LOG macros */
#define LOG_PRINT(...)         LOG(1, 0, "PRI", __VA_ARGS__)
#define LOG_ERR(...)           LOG(1, LOG_LEVEL_ERR, "ERR", __VA_ARGS__)
#define LOG_WARN(...)          LOG(1, LOG_LEVEL_WARN, "WARN", __VA_ARGS__)
#define LOG_INFO(...)          LOG(1, LOG_LEVEL_INFO, "INFO", __VA_ARGS__)
#define LOG_DBG(...)           LOG(1, LOG_LEVEL_DBG, "DBG", __VA_ARGS__)


static inline char *timenow() {
    static char buffer[64];
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", timeinfo);
    
    return buffer;
}