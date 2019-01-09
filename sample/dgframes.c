/*
	dgframes.c, by Alexander Marinov, Nov. 2006
	Movie deframizator
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dg/dg_video.h>
#include <dg/dg_widget.h>
#include <dg/dg_movie.h>
#include <dg/dg_plugin.h>

/* plugin enumeration callback */
static int plugin_enum_callback(dg_movie_reader_p* movie_reader, dg_plugin_info_p plugin_info)
{
	*movie_reader=plugin_info->movie_reader;

	/* movie reader found, stop plugin enumeration */
	return 1;
}

int main(int argc, char *argv[])
{
	dg_video_p device;                          /* video device */
	dg_movie_reader_p movie_reader=0;           /* movie reader */
	dg_movie_p movie;                           /* movie object */
	char* filename=0;                           /* movie file name */
	char errmsg[255];                           /* error message buf */
	int  iframe, nframes=20;                    /* frames to skip */
	dg_image_format_t image_format = IMAGE_FORMAT_JPEG; /* image format to produce */
	char image_ext[4];                          /* store image file extension */
	char pattern[255];                          /* store image file name's pattern */
	int i;
	sprintf(image_ext, "jpg");                  /* default jpg extension */
	sprintf(pattern, "frame");                  /* default `frame' pattern */

	/* process program options */
	for (i=1; i<argc; i++)
	{
		if (argv[i][0] == '-')
			switch (argv[i][1])
			{
			case 'f':
					nframes = atoi(argv[i]+2);
			break;
			case 'p':
					strcpy(pattern, argv[i]+2);
			break;
			case 'i':
				switch (argv[i][2])
				{
				case 'p': image_format = IMAGE_FORMAT_PNG; sprintf(image_ext, "png"); break;
				case 'j': image_format = IMAGE_FORMAT_JPEG; sprintf(image_ext, "jpg"); break;
				case 'g': image_format = IMAGE_FORMAT_GIF; sprintf(image_ext, "gif"); break;
				}
			break;
			}
		else
			filename = argv[i];
	}

	if (!filename)
	{
		sprintf(errmsg, "Argument file expected");
		goto opt_err;
	}

	/* create video device */
	device = dg_video_create(sizeof(dg_widget_t), 0, 0, 0, 0);

	/* find movie reader plugin */
	dg_plugin_enum(PLUGIN_MOVIE_READER, plugin_enum_callback, &movie_reader);
	if (!movie_reader)
	{
		sprintf(errmsg, "Unable to find plugin for movie reading");
		goto init_err;
	}

	/* create movie object */
	movie = (dg_movie_p)movie_reader->open_movie_file((struct dg_video_t*)device, filename);
	if (!movie)
	{
		sprintf(errmsg, "Unable to play file `%s'", filename);
		goto init_err;
	}

	iframe = 0;
	/* frame processing loop */
	while (movie_reader->read_next_frame((struct dg_movie_t*)movie))
	{
		char framefile[55];
		/* save movie frame on file */
		if ((0 < nframes) && (0 == (++iframe) % nframes))
		{
			sprintf(framefile, "%s_%d.%s", pattern, iframe, image_ext);
			device->save_surface_file(device, movie->surface, image_format, framefile);
			printf("%s\n", framefile);
		}
	}

	/* destroy movie object */
	movie_reader->close_movie((struct dg_movie_t*)movie);

init_err:
	/* destroy video device  */
	device->destroy(device);
	return 0;

opt_err:
	/* show usage info and error message */
	fprintf(stderr, "Usage: dgframes [options] file\n\n");
	fprintf(stderr, "-f<frames> frames to skip (default =20)\n");
	fprintf(stderr, "-p<pattern> filename pattern (default=`frame')\n");
	fprintf(stderr, "-ip png output image\n");
	fprintf(stderr, "-ij jpeg output image (default)\n");
	fprintf(stderr, "-ig gif output image\n");
	fprintf(stderr, "\nError: %s\n", errmsg);
	return 1;
}
