/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_pixel.c                                         */
/* Description:   Color & Pixel Converstions API                     */
/*                                                                   */
/*********************************************************************/

#include <dg/dg.h>
#include <dg/dg_pixel.h>
#include <dg/dg_plugin.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static void dg_pixel_convert_any(dg_pixel_format_p, unsigned char*, dg_pixel_format_p, unsigned char*, unsigned long);

unsigned long dg_pixel_pack(dg_pixel_format_p pixel_format, dg_color_t color)
{
	dg_color_t ncolor;
	unsigned char r, g, b, a;

	/* normalize color components */

	r=(DG_GET_RED(color)   >> (8-pixel_format->bRedBits))   & ((1 << pixel_format->bRedBits)-1);
	g=(DG_GET_GREEN(color) >> (8-pixel_format->bGreenBits)) & ((1 << pixel_format->bGreenBits)-1);
	b=(DG_GET_BLUE(color)  >> (8-pixel_format->bBlueBits))  & ((1 << pixel_format->bBlueBits)-1);
	a=(DG_GET_ALPHA(color) >> (8-pixel_format->bAlphaBits)) & ((1 << pixel_format->bAlphaBits)-1);
	ncolor=DG_RGB_COLOR(r, g, b, a);

	/* pack color */
	return (DG_GET_RED(ncolor)  << pixel_format->bRedShift)
		| (DG_GET_GREEN(ncolor) << pixel_format->bGreenShift)
		| (DG_GET_BLUE(ncolor)  << pixel_format->bBlueShift)
		| (DG_GET_ALPHA(ncolor) << pixel_format->bAlphaShift);
}

dg_color_t dg_pixel_unpack(dg_pixel_format_p pixel_format, unsigned long color)
{
	return DG_RGB_COLOR(
		(unsigned char)((((color >> pixel_format->bRedShift) & ((1 << pixel_format->bRedBits)-1)) << (8-pixel_format->bRedBits)) & 0xFF), 
		(unsigned char)((((color >> pixel_format->bGreenShift) & ((1 << pixel_format->bGreenBits)-1)) << (8-pixel_format->bGreenBits)) & 0xFF),
		(unsigned char)((((color >> pixel_format->bBlueShift)  & ((1 << pixel_format->bBlueBits)-1))  << (8-pixel_format->bBlueBits))  & 0xFF),
		(unsigned char)((((color >> pixel_format->bAlphaShift) & ((1 << pixel_format->bAlphaBits)-1)) << (8-pixel_format->bAlphaBits)) & 0xFF));
}

static void dg_pixel_convert_any(dg_pixel_format_p dFormat, unsigned char* dst, dg_pixel_format_p sFormat, unsigned char* src, unsigned long width)
{
	while (width--)
	{
		unsigned long sColor;
		unsigned long dColor;
		dg_color_t unpacked;

		switch (sFormat->bitsPerPixel)
		{
			case 15: 
			case 16: 
				sColor=0;
				sColor=*(unsigned short *)src;
				src+=2;
			break;
			case 24: 
				sColor=*(unsigned long *)src;
				sColor&=0xFFFFFF;
				src+=3;
			break; 
			case 32:
				sColor=*(unsigned long *)src;
				src+=4; 
			break; 
		}

		unpacked=dg_pixel_unpack(sFormat, sColor);
		dColor=dg_pixel_pack(dFormat, unpacked);

		switch (dFormat->bitsPerPixel)
		{
			case 15: 
			case 16: 
				*(unsigned short *)dst=(unsigned short)(dColor&0xFFFF);
				dst+=2;
			break;
			case 24: 
				*(unsigned short *)dst=(unsigned short)(dColor&0xFFFF);
				dst+=2;
				*dst++=(unsigned char)((dColor>>16)&0xFF);
			break; 
			case 32:
				*(unsigned long *)dst=dColor;
				dst+=4; 
			break; 
		}
	}
}

int dg_pixel_format_equals(dg_pixel_format_p sFormat, dg_pixel_format_p dFormat)
{
	dg_color_t color=DG_RGB_COLOR(1, 2, 4, 8);
	return (dg_pixel_pack(sFormat, color)==dg_pixel_pack(dFormat, color));
}

static int plugin_enum_callback(dg_pixel_format_convert_p* converter, dg_plugin_info_p plugin_info)
{
	*converter=plugin_info->pixel_format_converter;

	/* pixel converter found, stop plugin enumeration */
	return 1;
}

void dg_pixel_format_convert(struct dg_pixel_format_t* dFormat, unsigned char* dst, unsigned long dPitch, struct dg_pixel_format_t* sFormat, unsigned char* src, unsigned long sPitch, int width, int hlines)
{
	if (dg_pixel_format_equals((dg_pixel_format_p)sFormat, (dg_pixel_format_p)dFormat))
	{
		int bpp = (7 + ((dg_pixel_format_p)sFormat)->bitsPerPixel) >> 3;
		assert(sPitch && dPitch);
		while (hlines>0)
		{
			memcpy(dst, src, width * bpp);
			dst+=dPitch;
			src+=sPitch;
			hlines--;
		}
	}
	else
	{
		static int plugin_checked=0;
		static dg_pixel_format_convert_p pixel_format_converter=0;

		if (!plugin_checked)
		{
			dg_plugin_enum(PLUGIN_PIXEL_FORMAT, plugin_enum_callback, &pixel_format_converter);
			plugin_checked=1;
		}

		if (pixel_format_converter)
		{
			pixel_format_converter->pixel_format_convert(dFormat, dst, dPitch, sFormat, src, sPitch, width, hlines);
		}
		else
		while (hlines--) 
		{
			dg_pixel_convert_any((dg_pixel_format_p)dFormat, dst, (dg_pixel_format_p)sFormat, src, width);
			dst+=dPitch;
			src+=sPitch;
		}
	}
}

dg_hsv_t dg_rgb_to_hsv(dg_color_t color)
{
    unsigned char alpha = DG_GET_ALPHA(color);
    float red = DG_GET_RED(color) / 255.0f;
    float green = DG_GET_GREEN(color) / 255.0f;
    float blue = DG_GET_BLUE(color) / 255.0f;
    float mn = red, mx = red, hue, saturation, value, delta;
    int maxVal = 0;

    if (green > mx) 
	{ 
		mx = green; 
		maxVal = 1; 
	}

    if (blue > mx) 
	{ 
		mx = blue; 
		maxVal = 2; 
	}

    if (green < mn) 
	{
		mn = green;
	}

    if (blue < mn) 
	{
		mn = blue;
	}

    delta = mx - mn;

    value = mx;
    if (mx != 0)
	{
        saturation = delta / mx;
	}
    else 
	{
        saturation = 0;
        hue = 0;
    }

    if (saturation == 0.0f)
	{
		/* Arbritrary */
        hue = 0;
    } 
	else 
	{
        switch (maxVal)
        {
            case 0: hue = 0 + ( green - blue) / delta; break;    /* yel  < hue < mag  */
            case 1: hue = 2 + ( blue - red )  / delta; break;    /* cyan < hue < yel  */
            case 2: hue = 4 + ( red - green ) / delta; break;    /* mag  < hue < cyan */
            default: assert(0); hue = 0;
        }
    }

    hue *= 60;
    if (hue < 0) 
	{
		hue += 360;
	}

	hue = hue * (255.0f / 360.0f);

	return DG_HSV_COLOR( 
		(unsigned char)(hue),
		(unsigned char)(saturation * 255.0f),
		(unsigned char)(value * 255.0f),
		alpha
	);
}

dg_color_t dg_hsv_to_rgb(dg_hsv_t hsv)
{
	int i;
	float f, p, q, t, hTemp;
    unsigned char alpha = DG_GET_ALPHA(hsv);
	float hue = DG_GET_HUE(hsv) * (360.0f / 255.0f);
	float saturation = DG_GET_SATURATION(hsv) / 255.0f;
	float value = DG_GET_VELOCITY(hsv) / 255.0f;
	float red, green, blue;

	if (saturation == 0.0f)
	{ 
		/* totally unsaturated = grey */
		unsigned char v = DG_GET_VELOCITY(hsv);
		return DG_RGB_COLOR(v, v, v, DG_GET_ALPHA(hsv));
	}

	hTemp = hue / 60.0f;
	i = (int) floor(hTemp);             /* which sector */
	f = hTemp - i;                      /* how far through sector */
	p = value * ( 1 - saturation );
	q = value * ( 1 - saturation * f );
	t = value * ( 1 - saturation * ( 1 - f ) );

	switch (i)
	{
		case 0:  red = value; green = t; blue = p; break;
		case 1:  red = q; green = value; blue = p; break;
		case 2:  red = p; green = value; blue = t; break;
		case 3:  red = p; green = q; blue = value; break;
		case 4:  red = t; green = p; blue = value; break;
		case 5:  red = value; green = p; blue = q; break;
		default: red = 0; green = 0; blue = 0; assert(0);
	}

	return DG_RGB_COLOR( 
		(unsigned char)(red * 255.0f),
		(unsigned char)(green * 255.0f),
		(unsigned char)(blue * 255.0f),
		alpha
	);
}
