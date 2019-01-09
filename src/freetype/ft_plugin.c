/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      ft_plugin.c                                        */
/* Description:   Free Type plugin for True Type fonts               */
/*                                                                   */
/*********************************************************************/

#include "ft_plugin.h"
#include <dg/dg_utf8.h>
#include <dg/dg_gdc.h>

/* FreeType*/
#include <ft2build.h>
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H

static const char* plugin_name="freetype";

typedef struct
{
	char** fontpath;
	int npaths;
	FT_Library library;
} ft_font_info_t, *ft_font_info_p;

#define MAX_FONT_SIZE	200
typedef struct _ft_font_cache_t
{
	dg_font_p font[MAX_FONT_SIZE];
	char* fontface;
	struct _ft_font_cache_t* next;
} ft_font_cache_t, *ft_font_cache_p;

ft_font_cache_p font_cache=0;
ft_font_info_p font_info=0;

static const char* ttf_extension=".ttf";

#ifdef WIN32
static const char directory_separator='\\';
#else
static const char directory_separator='/';
#endif

static const char error_char='?';

/* FreeType font rendering functions */
static struct dg_font_t* ft_font_create(const char*, int);
static struct dg_font_t* ft_font_real_create(const char*, int);
static void ft_draw_text(struct dg_gdc_t*, int, int, const char*, int);
static void ft_get_string_box(dg_font_p, const char*, int, FT_BBox *);
static int ft_get_ascender(dg_font_p);
static int ft_get_descender(dg_font_p);
static int ft_get_height(dg_font_p);
static int ft_get_text_width(dg_font_p, const char*, int);
static void ft_font_destroy(dg_font_p);

/* Font path management functions */
static void ft_set_font_path(int, const char**);
static void ft_info_path_destroy(ft_font_info_p);

/* Font file location functions */
static int ft_font_exits(const char*);
static char* ft_font_name_create(const char*);
static char* ft_font_find(const char*);

/* Font caching */
static dg_font_p ft_cache_get(ft_font_cache_p*, const char*, int);
static void ft_cache_remove(ft_font_cache_p, dg_font_p);
static void ft_cache_destroy(ft_font_cache_p);


/* FreeType font rendering functions */
static struct dg_font_t* ft_font_real_create(const char* fontface, int fontsize)
{
	dg_font_p font = dg_font_create(fontface, fontsize);
	char* fontpath = ft_font_find(fontface);
	FT_Face ft_face;
	int error;

	error = FT_New_Face(font_info->library, fontpath, 0, &ft_face);
	if (error == FT_Err_Unknown_File_Format)
	{
		printf("Unsupported font format\n");
	}
	else if ( error )
	{
		printf("The font file `%s' is broken\n", fontpath);
	}
	else
	{
		error = FT_Set_Char_Size(
			ft_face,       /* handle to face object           */
			0,             /* char_width in 1/64th of points  */
			fontsize << 6, /* char_height in 1/64th of points */
			0,             /* horizontal device resolution    */
			0);            /* vertical device resolution      */
		if (error)
		{
			printf("Invalid font parameters\n");
		}
		else
		{
			if (FT_Select_Charmap(ft_face, FT_ENCODING_UNICODE))
			{
				printf("Unable to set charmap\n");
			}
			font->context = ft_face;
			font->destroy = ft_font_destroy;
			font->get_ascender = ft_get_ascender;
			font->get_descender = ft_get_descender;
			font->get_height = ft_get_height;
			font->get_text_width = ft_get_text_width;
			free(fontpath);
			return (struct dg_font_t*)font;
		}
	}
	free(fontpath);
	return 0;
}

static struct dg_font_t* ft_font_create(const char* fontface, int fontsize)
{
	return (struct dg_font_t*)ft_cache_get(&font_cache, fontface, fontsize);
}

static int ft_get_ascender(dg_font_p font)
{
	FT_Face ft_face = (FT_Face)font->context;
	return ft_face->size->metrics.ascender >> 6;
}

static int ft_get_descender(dg_font_p font)
{
	FT_Face ft_face = (FT_Face)font->context;
	return ft_face->size->metrics.descender >> 6;
}

static int ft_get_height(dg_font_p font)
{
	FT_Face ft_face = (FT_Face)font->context;
	return (63 + ft_face->size->metrics.ascender + abs(ft_face->size->metrics.descender)) >> 6;
}

static int ft_get_text_width(dg_font_p font, const char* text, int length)
{
	FT_BBox bbox;
	ft_get_string_box(font, text, length, &bbox);
	return bbox.xMax;
}

static void ft_font_destroy(dg_font_p font)
{
	if (font)
	{
		FT_Face ft_face = (FT_Face)font->context;
		FT_Done_Face(ft_face);
		ft_cache_remove(font_cache, font);
		dg_font_destroy(font);
	}
}

static void ft_draw_text(struct dg_gdc_t* _gdc, int x, int y, const char* text, 
						 int length)
{
	int i, j;
	dg_gdc_p gdc = (dg_gdc_p)_gdc;
	dg_font_p font = gdc->get_font(gdc);
	FT_BBox bbox;
	int scale=0;
	if (font)
	{
		/* fetch freetype context from the font object */
		FT_Face ft_face = (FT_Face)font->context;
		FT_GlyphSlot slot = ft_face->glyph;  /* a shortcut to glyph */
		int prev_glyph_index = 0, use_kerning = FT_HAS_KERNING(ft_face);
		int width, height, y_corr;
		int skip;
		dg_align_t align = gdc->get_align(gdc);

		ft_get_string_box(font, text, length, &bbox);
		width  = bbox.xMax;
		height = ft_get_height(font);
		y_corr = bbox.yMax + (abs(font->get_descender(font)) >> 1);
		y += y_corr;

		/* align horizontaly */
		if (align & ALIGN_CENTER_X)
		{
			switch (align & ALIGN_CENTER_X)
			{
			case ALIGN_CENTER_X:
				x -= (width >> 1);
			break;
			case ALIGN_LEFT:
			break;
			case ALIGN_RIGHT:
				x -= width;
			break;
			}
		}

		/* align verticaly */
		if (align & ALIGN_CENTER_Y)
		{
			switch (align & ALIGN_CENTER_Y)
			{
			case ALIGN_CENTER_Y:
				y -= (height >> 1);
			break;
			case ALIGN_TOP:
			break;
			case ALIGN_BOTTOM:
				y -= height;
			break;
			}
		}

		if (gdc->brush)
			gdc->draw_fillrect(gdc, x, y - y_corr, width, height);

		while (length--)
		{
			FT_Bitmap* bitmap;
			int fg_red   = DG_GET_RED(gdc->pen->color);
			int fg_green = DG_GET_GREEN(gdc->pen->color);
			int fg_blue  = DG_GET_BLUE(gdc->pen->color);
			int fg_alpha = DG_GET_ALPHA(gdc->pen->color);
			unsigned int code = (*text & 0xFF);
			unsigned int glyph_index;
			int error;

			if (code >= 0x80)
			{
				skip = utf8_decode(text, &code);
				glyph_index = code;
			}
			else
			{
				skip = 1;
				if (code < 32)
					glyph_index = FT_Get_Char_Index(ft_face, code);
				else
				{
					glyph_index = code;
				}
			}


			/* load glyph image into the slot (erase previous one) */
			error = FT_Load_Char(ft_face, glyph_index, FT_LOAD_RENDER);
			if (error)
			{
				/* unable to load glyph for the current character */
				FT_Load_Char(ft_face, error_char, FT_LOAD_RENDER);
			}

			/* draw the character glyph */
			bitmap = &slot->bitmap;

			if (use_kerning && prev_glyph_index && glyph_index)
			{      
				FT_Vector delta;
				FT_Get_Kerning(ft_face, prev_glyph_index, glyph_index, FT_KERNING_DEFAULT, &delta);
				x += delta.x >> 6;
				y += delta.y >> 6;
			}

			prev_glyph_index = glyph_index;

			for (i=0; i<bitmap->rows; i++)
				for (j=0; j<bitmap->width; j++)
				{
					unsigned char gray_level = bitmap->buffer[j+i*bitmap->pitch];
					dg_color_t color;
					color = DG_RGB_COLOR(fg_red, fg_green, fg_blue, (fg_alpha * gray_level + 1) >> 8 );
					gdc->set_pixel(gdc, x + slot->bitmap_left + j, y - slot->bitmap_top + i, color);
				}

			/* increment pen position */
			x += slot->advance.x >> 6;
			y += slot->advance.y >> 6;

			text += skip;
		}
	}
}

static void ft_get_string_box_hellper(dg_font_p font, const char* text, int length, FT_BBox *pbbox)
{
	FT_BBox bbox;
	FT_Face ft_face = font->context;
	int pen_x = 0, pen_y = 0, prev_glyph_index = 0, use_kerning = FT_HAS_KERNING(ft_face);
	int skip;
	FT_GlyphSlot slot = ft_face->glyph; /* shortcut to the slot */

	/* initialize string bbox to "empty" values */
	bbox.xMin = bbox.yMin =  32000;
	bbox.xMax = bbox.yMax = -32000;

	/* for each glyph image, compute its bounding box, */
	/* translate it, and grow the string bbox          */
	while (length--)
	{
		FT_BBox glyph_bbox;
		FT_Glyph glyph;
		unsigned int code = (*text & 0xFF);
		int glyph_index, error;

		if (code >= 0x80)
			skip = utf8_decode(text, &code);
		else
		{
			skip = 1;
		}

		glyph_index = FT_Get_Char_Index(ft_face, code);

		/* load glyph image into the slot without rendering */    
		error = FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
		if (error)
		{
			/* unable to load glyph for the current character */
			glyph_index = FT_Get_Char_Index(ft_face, error_char);
			FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
		}

		/* extract glyph image and store it in our table */    
		error = FT_Get_Glyph(ft_face->glyph, &glyph);
		if ( error )
		{
			assert(0);
			continue;  /* ignore errors, jump to next glyph */  
		}

		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &glyph_bbox);

		if (use_kerning && prev_glyph_index && glyph_index)
		{      
			FT_Vector delta;
			FT_Get_Kerning(ft_face, prev_glyph_index, glyph_index, FT_KERNING_DEFAULT, &delta);
			pen_x += delta.x >> 6;
			pen_y += delta.y >> 6;
		}

		prev_glyph_index = glyph_index;

		glyph_bbox.xMin += pen_x;
		glyph_bbox.xMax += pen_x;
		glyph_bbox.yMin += pen_y;
		glyph_bbox.yMax += pen_y;

		if (glyph_bbox.xMin < bbox.xMin)
			bbox.xMin = glyph_bbox.xMin;

		if (glyph_bbox.yMin < bbox.yMin)
			bbox.yMin = glyph_bbox.yMin;

		if (glyph_bbox.xMax > bbox.xMax)
			bbox.xMax = glyph_bbox.xMax;

		if (glyph_bbox.yMax > bbox.yMax)
			bbox.yMax = glyph_bbox.yMax;
		    /* increment pen position */    

		pen_x += slot->advance.x >> 6;
		pen_y += slot->advance.y >> 6;

		text += skip;
	}

	/* check that we really grew the string bbox */    
	if ( bbox.xMin > bbox.xMax )
	{
		bbox.xMin = 0;
		bbox.yMin = 0;
		bbox.xMax = 0;
		bbox.yMax = 0;
	}

	/* return string bbox */    
	*pbbox = bbox;
}

static void ft_get_string_box(dg_font_p font, const char* text, int length, FT_BBox *pbbox)
{
	if (length > 0)
	{
		char ibeam[3];
		int size = (int)(utf8_strpos(text, length) - text);
		char* newtext = (char*)malloc(size + 5);
		FT_BBox ibeambox;
		ibeam[0] = 0xC3;
		ibeam[1] = 0x84;
		ibeam[2] = 0;

		ft_get_string_box_hellper(font, ibeam, 1, &ibeambox);

		newtext[0] = ibeam[0];
		newtext[1] = ibeam[1];
		memcpy(newtext+2, text, size);
		newtext[size+3] = ibeam[0];
		newtext[size+4] = 0;

		ft_get_string_box_hellper(font, newtext, length+2, pbbox);
		pbbox->xMax -= 2*ibeambox.xMax;
		pbbox->xMin += 2*ibeambox.xMin;
		free(newtext);
	}
	else
	{
		pbbox->xMax = pbbox->xMin = pbbox->xMax = pbbox->xMin = 0;
	}
}

/* Font path management functions */
static void ft_set_font_path(int npaths, const char** fontpath)
{
	int i;

	if (!font_info)
	{
		font_info = (ft_font_info_p)malloc(sizeof(ft_font_info_t));
		memset(font_info, 0, sizeof(ft_font_info_t));
	}
	else
	{
		ft_info_path_destroy(font_info);
	}

	/* set new font path */
	font_info->fontpath = (char**)malloc(npaths * sizeof(char*));
	for (i=0; i<npaths; i++)
	{
		font_info->fontpath[i] = strdup(fontpath[i]);
	}
	font_info->npaths = npaths;
}

static void ft_info_path_destroy(ft_font_info_p fontinfo)
{
	if (fontinfo && fontinfo->fontpath)
	{
		int i;
		/* destroy font info */
		for (i=0; i<fontinfo->npaths; i++)
		{
			free(fontinfo->fontpath[i]);
		}
		free(fontinfo->fontpath);
	}
}

/* Font file location functions */
static int ft_font_exits(const char* fontpath)
{
	FILE* file = fopen(fontpath, "rb");
	if (file)
	{
		fclose(file);
		return 1;
	}
	return 0;
}

static char* ft_font_name_create(const char* fontface)
{
	char* fontname, *p;
	p = strrchr(fontface, '.');
	if (p)
	{
		/* the font face has extension */
		fontname = strdup(fontface);
	}
	else
	{
		/* the font face hasn't extension */
		fontname = (char*)malloc(strlen(fontface) + strlen(ttf_extension) + 1);
		strcpy(fontname, fontface);
		strcat(fontname, ttf_extension);
	}
	return fontname;
}

static char* ft_font_find(const char* fontface)
{
	if (!ft_font_exits(fontface))
	{
		if (font_info)
		{
			int i;
			char* fontpath;
			for (i=0; i<font_info->npaths; i++)
			{
				char* fontname = ft_font_name_create(fontface);
				size_t fontpathsize=strlen(font_info->fontpath[i]);
				if ( font_info->fontpath[i][fontpathsize] !=  directory_separator)
					fontpathsize++;
				fontpathsize+=strlen(fontname);
				fontpath = (char*)malloc(fontpathsize+1);
				strcpy(fontpath, font_info->fontpath[i]);
				if ( font_info->fontpath[i][fontpathsize] !=  directory_separator)
				{
					if (strlen(font_info->fontpath[i]) > 0)
					{
						size_t fontpathlen = strlen(fontpath);
						fontpath[fontpathlen] = directory_separator;
						fontpath[1+fontpathlen] = 0;
					}
				}
				strcat(fontpath, fontname);
				free(fontname);
				if (ft_font_exits(fontpath))
				{
					return fontpath;
				}
				free(fontpath);
			}
		}
		return 0;
	}
	else
	{
		return strdup(fontface);
	}
}

/* Font caching */
static dg_font_p ft_cache_get(ft_font_cache_p* pfont_cache, const char* fontface, int fontsize)
{
	ft_font_cache_p pcache = *pfont_cache;
	assert(fontsize < MAX_FONT_SIZE);
	while (pcache && 0 != strcmp(fontface, pcache->fontface))
		pcache=pcache->next;

	if (!pcache)
	{
		pcache = (ft_font_cache_p)malloc(sizeof(ft_font_cache_t));
		memset(pcache, 0, sizeof(ft_font_cache_t));
		pcache->fontface = strdup(fontface);
		pcache->next = *pfont_cache;
		*pfont_cache = pcache;
	}

	if (!pcache->font[fontsize])
		pcache->font[fontsize] = (dg_font_p)ft_font_real_create(fontface, fontsize);

	return pcache->font[fontsize];
}

static void ft_cache_remove(ft_font_cache_p cache, dg_font_p font)
{
	int size = font->get_size(font);
	const char* face = font->get_face(font);
	ft_font_cache_p pcache = cache;
	while (pcache && 0 != strcmp(face, pcache->fontface))
		pcache=pcache->next;

	if (pcache)
	{
		pcache->font[size] = 0;
	}
}

static void ft_cache_destroy(ft_font_cache_p cache)
{
	ft_font_cache_p pcache = cache;
	while (pcache)
	{
		int i;
		ft_font_cache_p tcache = pcache;
		pcache=pcache->next;
		for (i=0; i<MAX_FONT_SIZE; i++)
		{
			if (tcache->font[i])
			{
				ft_font_destroy(tcache->font[i]);
			}
		}
		free(tcache->fontface);
		free(tcache);
	}
}

/* DG plugin interface */
DG_API int dg_plugin_init( dg_plugin_info_p plugin_info, int initializing)
{
	if (initializing)
	{
		strcpy(plugin_info->plugin_name, plugin_name);
		plugin_info->provide_mask=PLUGIN_FONT_RENDERER;
		plugin_info->font_renderer=(dg_font_renderer_p)malloc(sizeof(dg_font_renderer_t));
		plugin_info->font_renderer->set_font_path = ft_set_font_path;
		plugin_info->font_renderer->font_create = ft_font_create;
		plugin_info->font_renderer->draw_text = ft_draw_text;

		font_info = (ft_font_info_p)malloc(sizeof(ft_font_info_t));
		memset(font_info, 0, sizeof(ft_font_info_t));
		if (FT_Init_FreeType(&font_info->library))
		{
			printf("Error occurred during freetype library initialization.\n");
			dg_plugin_init(plugin_info, 0);
			return 0;
		}

		/* FIXME: temporary hack! */
		{

#ifdef NDEBUG
			const char* font_paths[1] = {""};
#else
			const char* font_paths[1] = {"c:\\cvs\\lrun-art\\font"};
#endif

			ft_set_font_path(1, font_paths);
		}
	}
	else
	{
		/* deinitialize freetype */
		FT_Done_FreeType(font_info->library);

		/* destroy font info */
		if (font_info)
		{
			ft_info_path_destroy(font_info);
			free(font_info);
		}

		/* destroy font cache */
		ft_cache_destroy(font_cache);

		/* destroy plugin data */
		free(plugin_info->font_renderer);
	}
	return 1;
}
