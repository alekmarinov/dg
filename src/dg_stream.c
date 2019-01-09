/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_stream.c                                        */
/* Description:   I/O streams                                        */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <dg/dg_stream.h>

static int dg_memory_alloc(dg_memory_p self, int size)
{
	if (self->memory)
	{
		self->free(self);
	}
	self->memory = malloc(size);
	if (self->memory)
	{
		return 1;
	}
	return 0;
}

static void dg_memory_free(dg_memory_p self)
{
	free(self->memory);
	self->memory=0;
}

static void dg_memory_destroy(dg_memory_p self)
{
	self->free(self);
	free(self);
}

dg_memory_p dg_memory_create(int size)
{
	dg_memory_p self = malloc(sizeof(dg_memory_t));
	self->memory     = 0;
	self->size       = size;

	self->alloc      = dg_memory_alloc;
	self->free       = dg_memory_free;
	self->destroy    = dg_memory_destroy;
	return self;
}


/* Common stream definitions */

static int dg_stream_read(dg_stream_p self, void* buffer, int size)
{
	/* override this method */
	return 0;
}

static int dg_stream_write(dg_stream_p self, const void* buffer, int size)
{
	/* override this method */
	return 0;
}

static int dg_stream_seek(dg_stream_p self, int64_t offset, dg_seek_t seektype)
{
	/* override this method */
	return 0;
}

static int dg_stream_tell(dg_stream_p self, int64_t* offset)
{
	/* override this method */
	return 0;
}

static int dg_stream_readbyte(dg_stream_p self, unsigned char* b)
{
	return self->read(self, b, 1);
}

static int dg_stream_writebyte(dg_stream_p self, unsigned char b)
{
	return self->write(self, &b, 1);
}

static int dg_stream_readshort(dg_stream_p self, unsigned short* w)
{
	return self->read(self, w, 2);
}

static int dg_stream_writeshort(dg_stream_p self, unsigned short w)
{
	return self->write(self, &w, 2);
}

static int dg_stream_readint(dg_stream_p self, int* n)
{
	return self->read(self, n, 4);
}

static int dg_stream_writeint(dg_stream_p self, int n)
{
	return self->write(self, &n, 4);
}

static int dg_stream_readlong(dg_stream_p self, int64_t* l)
{
	return self->read(self, l, 8);
}

static int dg_stream_writelong(dg_stream_p self, int64_t l)
{
	return self->write(self, &l, 8);
}

void dg_stream_destroy(dg_stream_p self)
{
	free(self);
}

dg_stream_p dg_stream_create(void* context)
{
	dg_stream_p self = (dg_stream_p)malloc(sizeof(dg_stream_t));

	self->context=context;

	/* abstract stream methods */
	self->read=0;
	self->write=0;
	self->seek=0;
	self->tell=0;

	self->readbyte   = dg_stream_readbyte;
	self->writebyte  = dg_stream_writebyte;
	self->readshort  = dg_stream_readshort;
	self->writeshort = dg_stream_writeshort;
	self->readint    = dg_stream_readint;
	self->writeint   = dg_stream_writeint;
	self->readlong   = dg_stream_readlong;
	self->writelong  = dg_stream_writelong;

	self->destroy    = dg_stream_destroy;

	return self;
}

/* File I/O stream */

static int dg_stream_file_read(dg_stream_p self, void* buffer, int size)
{
	FILE* file=(FILE*)self->context;
	return (int)fread(buffer, 1, size, file);
}

static int dg_stream_file_write(dg_stream_p self, const void* buffer, int size)
{
	FILE* file=(FILE*)self->context;
	return (int)fwrite(buffer, 1, size, file);
}

static int dg_stream_file_seek(dg_stream_p self, int64_t offset, dg_seek_t seektype)
{
	FILE* file=(FILE*)self->context;
	int fseektype=SEEK_SET;
	switch (seektype) 
	{
	case SEEK_FROM_CURRENT: fseektype=SEEK_CUR; break;
	case SEEK_FROM_END: fseektype=SEEK_END; break;
	}
	if (0 == fseek(file, (long)offset, fseektype))
		return 1; /* seek success */
	return 0; /* seek failure */
}

static int dg_stream_file_tell(dg_stream_p self)
{
	FILE* file=(FILE*)self->context;
	if (-1L != ftell(file))
		return 1; /* tell success */
	return 0; /* tell failure */
}

static void dg_stream_file_destroy(dg_stream_p self)
{
	if (self->context)
	{
		FILE* file = self->context;
		fclose(file);
	}
	dg_stream_destroy(self);
}

dg_stream_p dg_stream_file(const char* filename, dg_file_access_t file_access, dg_file_mode_t file_mode)
{
	FILE* file;
	const char* fopenmode;
	switch (file_access)
	{
	case FILE_ACCESS_READ:
	{
		fopenmode = (file_mode == FILE_MODE_BINARY)?"rb":"rt";
	}
	break;
	case FILE_ACCESS_WRITE:
	{
		fopenmode = (file_mode == FILE_MODE_BINARY)?"wb":"wt";
	}
	break;
	case FILE_ACCESS_READ|FILE_ACCESS_WRITE:
	{
		fopenmode = (file_mode == FILE_MODE_BINARY)?"r+b":"r+t";
	}
	break;
	case FILE_ACCESS_WRITE|FILE_ACCESS_APPEND:
	{
		fopenmode = (file_mode == FILE_MODE_BINARY)?"ab":"at";
	}
	break;
	case FILE_ACCESS_READ|FILE_ACCESS_WRITE|FILE_ACCESS_APPEND:
	{
		fopenmode = (file_mode == FILE_MODE_BINARY)?"a+b":"a+t";
	}
	break;
	default: 
		assert(0);
		return 0;
	}

	file = fopen(filename, fopenmode);
	if (file)
	{
		dg_stream_p file_stream = dg_stream_create(file);
		file_stream->read  = dg_stream_file_read;
		file_stream->write = dg_stream_file_write;
		file_stream->seek  = dg_stream_file_seek;
		file_stream->tell  = dg_stream_file_tell;
		file_stream->destroy = dg_stream_file_destroy;
		return file_stream;
	}
	return 0;
}
