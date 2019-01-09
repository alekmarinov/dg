/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_stream.h                                        */
/* Description:   I/O streams                                        */
/*                                                                   */
/*********************************************************************/

#ifndef __DG_STREAM_H
#define __DG_STREAM_H

#include <dg/dg.h>
#include <dg/pstdint.h>

/*!
	\addtogroup dg_stream I/O streams
	<b>This page is under construction</b>
  *  @{
*/

/*! \brief \c dg_memory_t defines a memory object 
* to be used for memory exchange
*/
typedef struct _dg_memory_t
{
	void* memory;
	int   size;

	int  (*alloc)(struct _dg_memory_t*, int);
	void (*free)(struct _dg_memory_t*);
	void (*destroy)(struct _dg_memory_t*);
} dg_memory_t, *dg_memory_p;

/*! \brief \c dg_seek_t defines the possible seeking types */ 
typedef enum
{
	SEEK_FROM_START,
	SEEK_FROM_CURRENT,
	SEEK_FROM_END
} dg_seek_t;

/*! \brief \c dg_file_access_t defines the file access rights to open file */ 
typedef enum
{
	FILE_ACCESS_READ=1,
	FILE_ACCESS_WRITE=2,
	FILE_ACCESS_APPEND=4,
} dg_file_access_t;

/*! \brief \c dg_file_mode_t defines the file modes - binary or text */ 
typedef enum
{
	FILE_MODE_BINARY=0,
	FILE_MODE_TEXT=1
} dg_file_mode_t;

/*! \brief \c dg_stream_t defines a stream object 
* to be used for abstract input or output data source
*/
typedef struct _dg_stream_t
{
	/*! stream specific context data */
	void* context;

	/*! Reads buffer from stream. Returns the amount of bytes actually read */
	int (*read)(struct _dg_stream_t*, void*, int);
	/*! Writes buffer to stream. Returns the amount of bytes actually written */
	int (*write)(struct _dg_stream_t*, const void*, int);

	/*! Reads a byte from stream. Returns the amount of bytes actually read */
	int (*readbyte)(struct _dg_stream_t*, unsigned char*);
	/*! Writes a byte to stream. Returns the amount of bytes actually written */
	int (*writebyte)(struct _dg_stream_t*, unsigned char);

	/*! Read a short from stream. Returns the amount of bytes actually read */
	int (*readshort)(struct _dg_stream_t*, unsigned short*);
	/*! Writes a short to stream. Returns the amount of bytes actually written */
	int (*writeshort)(struct _dg_stream_t*, unsigned short);

	/*! Read a 32-bit int from stream. Returns the amount of bytes actually read */
	int (*readint)(struct _dg_stream_t*, int*);
	/*! Writes a 32-bit int to stream. Returns the amount of bytes actually written */
	int (*writeint)(struct _dg_stream_t*, int);

	/*! Read a 64-bit int from stream. Returns the amount of bytes actually read */
	int (*readlong)(struct _dg_stream_t*, int64_t*);
	/*! Writes a 64-bit int to stream. Returns the amount of bytes actually written */
	int (*writelong)(struct _dg_stream_t*, int64_t);

	/*! Seek at specified position in a stream. Returns 1 on success, 0 on failure */
	int (*seek)(struct _dg_stream_t*, int64_t, dg_seek_t);
	/*! Tell the current position from the beginning of stream. Returns 1 on success, 0 on failure */
	int (*tell)(struct _dg_stream_t*);

	/*! Destroys the stream object */
	void (*destroy)(struct _dg_stream_t*);
} dg_stream_t, *dg_stream_p;

DG_API dg_memory_p dg_memory_create(int);
DG_API dg_stream_p dg_stream_create(void*);
DG_API dg_stream_p dg_stream_file(const char*, dg_file_access_t, dg_file_mode_t);

/*! @} */

#endif
