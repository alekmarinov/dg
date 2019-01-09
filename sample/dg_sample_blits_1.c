/*
	Tests blit operations
*/

#include <stdio.h>
#include <string.h>
#include <dg/dg_blit.h>
#include <dg/dg_pixel.h>

dg_pixel_format_t ci_15={5,5,5,0,10,5,0,0,15};
dg_pixel_format_t ci_16={5,6,5,0,11,5,0,0,16};
dg_pixel_format_t ci_24={8,8,8,0,16,8,0,0,24};
dg_pixel_format_t ci_32={8,8,8,8,16,8,0,24,32};

#define width 5
const unsigned short src_15[width]={1,2,3,4,5};
unsigned short dst_15[width]={0};

const unsigned short src_16[width]={1,2,3,4,5};
unsigned short dst_16[width]={0};

const struct { unsigned char r,g,b; } src_24[width]={ {1,2,3},{1,2,3},{1,2,3},{1,2,3},{1,2,3} };
struct { unsigned char r,g,b; } dst_24[width]={0};

const unsigned long src_32[width]={ 1,2,3,4,5 };
unsigned long dst_32[width]={0};

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


void clear_buffers()
{
	memset(dst_15, 0, width*sizeof(unsigned short));
	memset(dst_16, 0, width*sizeof(unsigned short));
	memset(dst_24, 0, width*3*sizeof(unsigned char));
	memset(dst_32, 0, width*sizeof(unsigned long));
}

void print_color(dg_color_t color)
{
	printf("(%d %d %d %d)", DG_GET_RED(color), DG_GET_GREEN(color), DG_GET_BLUE(color), DG_GET_ALPHA(color));
}

void print_buffer(dg_pixel_format_p ci, unsigned char* buffer)
{
	int i;
	unsigned long l;
	unsigned short s;
	for (i=0; i<width; i++)
	{
		switch (ci->bitsPerPixel)
		{
			case 32:
				l=*(unsigned long*)buffer;
				print_color(dg_pixel_unpack(ci, l));
				buffer+=4;
			break;
			case 24:
				l=*(unsigned long*)buffer;
				print_color(dg_pixel_unpack(ci, l));
				buffer+=3;
			break;
			case 16:
				s=*(unsigned short*)buffer;
				print_color(dg_pixel_unpack(ci, (unsigned long)s));
				buffer+=2;
			break;
			case 15:
				s=*(unsigned short*)buffer;
				print_color(dg_pixel_unpack(ci, (unsigned long)s));
				buffer+=2;
			break;
		}
	}
}

void print_buffers(dg_pixel_format_p ci, unsigned char* dst, unsigned char* src)
{
	print_buffer(ci, src); printf("->\n");
	print_buffer(ci, dst); printf("\n\n");
}

#define TEST(name) \
	clear_buffers(); dg_blit_#name(ci_15.bitsPerPixel, (unsigned char*)dst_15, (unsigned char*)src_15, width); \
	clear_buffers(); dg_blit_#name(&ci_16, (unsigned char*)dst_16, (unsigned char*)src_16, width); \
	clear_buffers(); dg_blit_#name(&ci_24, (unsigned char*)dst_24, (unsigned char*)src_24, width); \
	clear_buffers(); dg_blit_#name(&ci_32, (unsigned char*)dst_32, (unsigned char*)src_32, width);



int test_blit_copy()
{
	printf("-- test_blit_copy -- \n");
	clear_buffers(); dg_blit_copy(ci_15.bitsPerPixel, (unsigned char*)dst_15, (unsigned char*)src_15, width); print_buffers(&ci_15, (unsigned char*)dst_15, (unsigned char*)src_15);
	clear_buffers(); dg_blit_copy(ci_16.bitsPerPixel, (unsigned char*)dst_16, (unsigned char*)src_16, width); print_buffers(&ci_16, (unsigned char*)dst_16, (unsigned char*)src_16);
	clear_buffers(); dg_blit_copy(ci_24.bitsPerPixel, (unsigned char*)dst_24, (unsigned char*)src_24, width); print_buffers(&ci_24, (unsigned char*)dst_24, (unsigned char*)src_24);
	clear_buffers(); dg_blit_copy(ci_32.bitsPerPixel, (unsigned char*)dst_32, (unsigned char*)src_32, width); print_buffers(&ci_32, (unsigned char*)dst_32, (unsigned char*)src_32);

	return 1;
}

int test_blit_alpha_fast()
{
	printf("-- test_blit_alpha_fast -- \n");
	clear_buffers(); dg_blit_alpha(ci_15.bitsPerPixel, (unsigned char*)dst_15, (unsigned char*)src_15, width, 128); print_buffers(&ci_15, (unsigned char*)dst_15, (unsigned char*)src_15);
	clear_buffers(); dg_blit_alpha(ci_16.bitsPerPixel, (unsigned char*)dst_16, (unsigned char*)src_16, width, 128); print_buffers(&ci_16, (unsigned char*)dst_16, (unsigned char*)src_16);
	clear_buffers(); dg_blit_alpha(ci_24.bitsPerPixel, (unsigned char*)dst_24, (unsigned char*)src_24, width, 128); print_buffers(&ci_24, (unsigned char*)dst_24, (unsigned char*)src_24);
	clear_buffers(); dg_blit_alpha(ci_32.bitsPerPixel, (unsigned char*)dst_32, (unsigned char*)src_32, width, 128); print_buffers(&ci_32, (unsigned char*)dst_32, (unsigned char*)src_32);

	return 1;
}

int test_blit_alpha()
{
	printf("-- test_blit_alpha -- \n");
	clear_buffers(); dg_blit_alpha(ci_15.bitsPerPixel, (unsigned char*)dst_15, (unsigned char*)src_15, width, 200); print_buffers(&ci_15, (unsigned char*)dst_15, (unsigned char*)src_15);
	clear_buffers(); dg_blit_alpha(ci_16.bitsPerPixel, (unsigned char*)dst_16, (unsigned char*)src_16, width, 200); print_buffers(&ci_16, (unsigned char*)dst_16, (unsigned char*)src_16);
	clear_buffers(); dg_blit_alpha(ci_24.bitsPerPixel, (unsigned char*)dst_24, (unsigned char*)src_24, width, 200); print_buffers(&ci_24, (unsigned char*)dst_24, (unsigned char*)src_24);
	clear_buffers(); dg_blit_alpha(ci_32.bitsPerPixel, (unsigned char*)dst_32, (unsigned char*)src_32, width, 200); print_buffers(&ci_32, (unsigned char*)dst_32, (unsigned char*)src_32);

	return 1;
}

main()
{
	dg_color_t color;
	unsigned long packed;

	color = 0x40FFC080; packed=dg_pixel_pack(&ci_15, color); color=dg_pixel_unpack(&ci_15, packed); print_color(color);
	color = 0x40FFC080; packed=dg_pixel_pack(&ci_16, color); color=dg_pixel_unpack(&ci_16, packed); print_color(color);
	color = 0x40FFC080; packed=dg_pixel_pack(&ci_24, color); color=dg_pixel_unpack(&ci_24, packed); print_color(color);
	color = 0x40FFC080; packed=dg_pixel_pack(&ci_32, color); color=dg_pixel_unpack(&ci_32, packed); print_color(color);
	printf("\n");

	test_blit_copy();
	test_blit_alpha_fast();
	test_blit_alpha();
	return 0;
}
