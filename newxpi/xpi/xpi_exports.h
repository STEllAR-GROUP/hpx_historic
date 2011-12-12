/*
 * Copyright (c) 2011 Alex Nagelberg
 * Distributed under the Boost Software License, Version 1.0. (See accompanying 
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef _XPI_EXPORTS_H_
#define _XPI_EXPORTS_H_

#if defined(WIN32)
# define XPI_SYMBOL_EXPORT      __declspec(dllexport)
# define XPI_SYMBOL_IMPORT      __declspec(dllimport)
# define XPI_SYMBOL_INTERNAL    /* empty */
# define XPI_APISYMBOL_EXPORT   __declspec(dllexport)
# define XPI_APISYMBOL_IMPORT   __declspec(dllimport)
#elif defined(XPI_GCC_HAVE_VISIBILITY)
# define XPI_SYMBOL_EXPORT      __attribute__((visibility("default")))
# define XPI_SYMBOL_IMPORT      __attribute__((visibility("default")))
# define XPI_SYMBOL_INTERNAL    __attribute__((visibility("hidden")))
# define XPI_APISYMBOL_EXPORT   /* empty */
# define XPI_APISYMBOL_IMPORT   /* empty */
#endif

// make sure we have reasonable defaults
#if !defined(XPI_SYMBOL_EXPORT)
# define XPI_SYMBOL_EXPORT      /* empty */
#endif
#if !defined(XPI_SYMBOL_IMPORT)
# define XPI_SYMBOL_IMPORT      /* empty */
#endif
#if !defined(XPI_SYMBOL_INTERNAL)
# define XPI_SYMBOL_INTERNAL    /* empty */
#endif
#if !defined(XPI_APISYMBOL_EXPORT)
# define XPI_APISYMBOL_EXPORT   /* empty */
#endif
#if !defined(XPI_APISYMBOL_IMPORT)
# define XPI_APISYMBOL_IMPORT   /* empty */
#endif

///////////////////////////////////////////////////////////////////////////////
#if defined(XPI_EXPORTS)
# define  XPI_EXPORT             XPI_SYMBOL_EXPORT
# define  XPI_EXCEPTION_EXPORT   XPI_SYMBOL_EXPORT
# define  XPI_API_EXPORT         XPI_APISYMBOL_EXPORT
#else
# define  XPI_EXPORT             XPI_SYMBOL_IMPORT
# define  XPI_EXCEPTION_EXPORT   XPI_SYMBOL_IMPORT
# define  XPI_API_EXPORT         XPI_APISYMBOL_IMPORT
#endif
    
#endif
