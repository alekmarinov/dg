/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_debug.h                                         */
/* Description:   Debugging utility macros                           */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_DEBUG_H
#define __DG_DEBUG_H

#include <assert.h>

/*! \cond */

#if defined(_DEBUG) && !defined(DEBUG)
  #define DEBUG
#endif

#if defined(DEBUG)
  #if defined(_WIN32)
    /* Enable leak detection for Win32 */
	#define _CRTDBG_MAP_ALLOC 
    #include <stdlib.h>
    #include <crtdbg.h>
  #endif
  #include <stdio.h>
#endif

/* logging levels */

/*! no logging */
#define LOG_LEVEL_SILENT	0
/*! log important messages */
#define LOG_LEVEL_IMPORTANT	1
/*! log informative messages */
#define LOG_LEVEL_INFO		5
/*! log warning messages */
#define LOG_LEVEL_WARN		7
/*! log debug messages */
#define LOG_LEVEL_DEBUG		10

/* debug logging macros */

#if defined(DEBUG) 
  #if defined(_WIN32)
    #define DPRINTF(level, x)                                      \
      if (level<=_loglevel) {	                                   \
        printf("%s <%d>:", __FILE__, __LINE__);                    \
        printf(x);                                                 \
      }
    #define LOG_SILENT(x) DPRINTF(LOG_LEVEL_SILENT, x)
    #define LOG_IMPORTANT(x) DPRINTF(LOG_LEVEL_IMPORTANT, x)
    #define LOG_INFO(x) DPRINTF(LOG_LEVEL_INFO, x)
    #define LOG_WARN(x) DPRINTF(LOG_LEVEL_WARN, x)
    #define LOG_DEBUG(x) DPRINTF(LOG_LEVEL_DEBUG, x)
  #else
    #define DPRINTF(level, x...)                                   \
      if (level<=_loglevel) {	                                   \
        printf("%s <%d>:", __FILE__, __LINE__);                    \
        printf(x);                                                 \
      }

      #define LOG_IMPORTANT(x...)                                  \
        if (LOG_LEVEL_IMPORTANT<=_loglevel) {                      \
          printf("%s <%d> important:", __FILE__, __LINE__);        \
          printf(x);                                               \
        }

      #define LOG_INFO(x...)                                       \
        if (LOG_LEVEL_INFO<=_loglevel) {                           \
          printf("%s <%d> info:", __FILE__, __LINE__);             \
          printf(x);                                               \
        }

      #define LOG_WARN(x...)                                       \
        if (LOG_LEVEL_WARN<=_loglevel) {                           \
          printf("%s <%d> warn:", __FILE__, __LINE__);             \
          printf(x);                                               \
        }

      #define LOG_DEBUG(x...)                                      \
        if (LOG_LEVEL_DEBUG<=_loglevel) {                          \
          printf("%s <%d> debug:", __FILE__, __LINE__);            \
          printf(x);                                               \
        }
      #endif
#else
	#define DPRINTF
	#define LOG_SILENT
	#define LOG_IMPORTANT
	#define LOG_INFO
	#define LOG_WARN
	#define LOG_DEBUG
#endif

extern int _loglevel;

/*! \endcond */

#endif
