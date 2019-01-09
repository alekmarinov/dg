/*
	dg_sample_screen_1.c
	Tests freetype library for rendering font directly to DG surface
*/

#include <tchar.h>
#include <string.h>
#include "dg_sample_shell.h"

/* FreeType*/
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library  library=0;   /* handle to library     */
FT_Face     face;        /* handle to face object */


#define BOX_WIDTH  100
#define BOX_HEIGHT 100

void my_draw_bitmap( dg_color_p buf, int width, FT_Bitmap* bitmap, int x, int y)
{
	int i, j;
	for (i=0; i<bitmap->rows; i++)
		for (j=0; j<bitmap->width; j++)
		{
			unsigned char c=bitmap->buffer[j+i*bitmap->pitch];
			dg_color_t color;
			color = DG_RGB_COLOR(c, c, c, 255);
			buf[x+j+(y+i)*width]=color;
		}
}

void font_renderer(dg_widget_p self)
{
	if (!library)
	{
		int error = FT_Init_FreeType( &library );
		if ( error )
		{
			printf("... an error occurred during library initialization ...\n");
		}
		else
		{
			error = FT_New_Face( library,
                       "C:\\WINDOWS\\Fonts\\arial.ttf",
                       0,
                       &face );
			if ( error == FT_Err_Unknown_File_Format )
			{
				printf("... the font file could be opened and read, but it appears\n");
				printf("... that its font format is unsupported\n");
			}
			else if ( error )
			{
				printf("... another error code means that the font file could not\n");
				printf("... be opened or read, or simply that it is broken...\n");
			}
			else
			{
				error = FT_Set_Char_Size(
					face,    /* handle to face object           */
					0,       /* char_width in 1/64th of points  */
					16*64,   /* char_height in 1/64th of points */
					300,     /* horizontal device resolution    */
					300 );   /* vertical device resolution      */
			}
		}
	}
	{
		FT_GlyphSlot  slot = face->glyph;  /* a small shortcut */
		int           pen_x, pen_y, n;
		const char*   text="Hello, FreeType!";
		int           num_chars=(int)strlen(text);
		if (self->surface->lock(self->surface, 1))
		{

			pen_x = 0;
			pen_y = 200;

			for ( n = 0; n < num_chars; n++ )
			{
				/* load glyph image into the slot (erase previous one) */
				int error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
				if ( error )
					continue;  /* ignore errors */

				/* now, draw to our target surface */
				my_draw_bitmap( self->surface->pixels, self->surface->pitch, &slot->bitmap,
								pen_x + slot->bitmap_left,
								pen_y - slot->bitmap_top );

				/* increment pen position */
				pen_x += slot->advance.x >> 6;
			}
			self->surface->unlock(self->surface);
		}
	}
}

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event) 
	{
		dg_widget_p widget;

		widget = device->create_widget(device, sizeof(dg_widget_t), 0, 0, 640, 480);
		widget->render=font_renderer;
		widget->render(widget);
		widget->invalidate(widget, 0);

		swap_buffers(device);
	}
	return 1;
}

int main()
{
	start_sample(event_handler);
	return 0;
}

