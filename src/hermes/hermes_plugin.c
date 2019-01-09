/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      hermes_plugin.c                                    */
/* Description:   DG pixel format converter plugin                   */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dg/dg.h>
#include <dg/dg_plugin.h>
#include <dg/dg_pixel.h>
#include <Hermes.h>

static const char* plugin_name="hermes";

static void hermes_pixel_format_convert(
	struct dg_pixel_format_t* _dFormat, 
	unsigned char* dst, 
	unsigned long dPitch, 
	struct dg_pixel_format_t* _sFormat, 
	unsigned char* src, 
	unsigned long sPitch, 
	int width, 
	int hlines)
{
	dg_color_t color_alpha=0;
	HermesFormat *hfSrc;
	HermesFormat *hfDst;
	dg_pixel_format_p sFormat=(dg_pixel_format_p)_sFormat;
	dg_pixel_format_p dFormat=(dg_pixel_format_p)_dFormat;
	HermesHandle h=Hermes_BlitterInstance(HERMES_CONVERT_NORMAL);
	hfSrc=Hermes_FormatNew(sFormat->bitsPerPixel, 
		dg_pixel_pack(sFormat, DG_COLOR_RED & 0xFFFFFF),
		dg_pixel_pack(sFormat, DG_COLOR_GREEN & 0xFFFFFF),
		dg_pixel_pack(sFormat, DG_COLOR_BLUE & 0xFFFFFF),
		dg_pixel_pack(sFormat, color_alpha), 0);
	hfDst=Hermes_FormatNew(dFormat->bitsPerPixel, 
		dg_pixel_pack(dFormat, DG_COLOR_RED & 0xFFFFFF),
		dg_pixel_pack(dFormat, DG_COLOR_GREEN & 0xFFFFFF),
		dg_pixel_pack(dFormat, DG_COLOR_BLUE & 0xFFFFFF),
		dg_pixel_pack(dFormat, color_alpha), 0);
	Hermes_BlitterRequest(h,hfSrc,hfDst);
	//Hermes_ConverterCopy(h, 
	//	src, 0, 0, width, hlines, sPitch,
	//	dst, 0, 0, width, hlines, dPitch);

	Hermes_BlitterBlit(h,
		src, 0, 0, width, hlines, sPitch,
		dst, 0, 0, width, hlines, dPitch);

	Hermes_FormatFree(hfSrc);
	Hermes_FormatFree(hfDst);
	Hermes_ConverterReturn(h);
}

DG_API int dg_plugin_init( dg_plugin_info_p plugin_info, int initializing)
{
	if (initializing)
	{
		if (!Hermes_Init()) 
		{
			return 0;
		}
		strcpy(plugin_info->plugin_name, plugin_name);
		plugin_info->provide_mask=PLUGIN_PIXEL_FORMAT;
		plugin_info->pixel_format_converter = 
			(dg_pixel_format_convert_p)malloc(sizeof(dg_pixel_format_convert_t));
		plugin_info->pixel_format_converter->pixel_format_convert=hermes_pixel_format_convert;

	}
	else
	{
		free(plugin_info->pixel_format_converter);
		Hermes_Done();
	}
	return 1;
}
