/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      ffmpeg_reader.h                                    */
/* Description:   Movie reader implementation via ffmpeg             */
/*                                                                   */
/*********************************************************************/

#ifndef __FFMPEG_READER_H
#define __FFMPEG_READER_H

#include <dg/dg_movie.h>

struct dg_movie_t* ffmpeg_open_read_movie_file(struct dg_video_t*, const char*);
struct dg_movie_t* ffmpeg_open_read_movie_stream(struct dg_video_t*, dg_stream_p);
struct dg_movie_t* ffmpeg_open_read_movie_memory(struct dg_video_t*, dg_memory_p);
int ffmpeg_read_next_frame(struct dg_movie_t*);
void ffmpeg_close_movie(struct dg_movie_t*);

#endif
