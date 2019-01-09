/*
	Tests rect intersection
*/

#include <stdio.h>
#include <malloc.h>
#include <dg/dg_rect.h>

void top(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 100, 50, 100, 100);
}

void left(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 50, 100, 100, 100);
}

void right(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 150, 100, 100, 100);
}

void bottom(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 100, 150, 100, 100);
}

void top_left(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 50, 50, 100, 100);
}

void top_right(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 150, 50, 100, 100);
}

void bottom_left(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 50, 150, 100, 100);
}

void bottom_right(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 150, 150, 100, 100);
}

void inside(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 120, 120, 50, 50);
}

void outside(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 120, 120, 50, 50);
	rect2->copy(rect2, 100, 100, 200, 200);
}

void top_inside(dg_rect_p rect1, dg_rect_p rect2)
{
	rect1->copy(rect1, 100, 100, 100, 100);
	rect2->copy(rect2, 120, 50, 50, 100);
}

void free_rects(dg_rect_p* rects, int count)
{
	int i;
	for (i=0; i<count; i++)
	{
		rects[i]->destroy(rects[i]);
	}
	if (rects)
		free(rects);
}

void print_rects(dg_rect_p* rects, int count)
{
	int i;
	for (i=0; i<count; i++)
	{
		if (i>0)
		{
			printf(",");
		}
		printf("(%d %d %d %d)", rects[i]->x, rects[i]->y, rects[i]->w, rects[i]->h);
	}
}

void rectangulize(dg_rect_p rect1, dg_rect_p rect2, dg_rect_p** rects, int* count)
{
	int ix, iy, iw, ih;
	if ( rect2->x < rect1->x && (rect2->x+rect2->w) > (rect1->x+rect1->w) && rect2->y < rect1->y && (rect2->y+rect2->h) > (rect1->y+rect1->h) )
	{
		printf("case outside\n");
		*count=0;
		*rects=0;
		return ;
	}

	if (rect1->intersect(rect1, rect2, &ix, &iy, &iw, &ih))
	{
		if ( ix >= rect1->x && (ix+iw) <= (rect1->x+rect1->w) && iy >= rect1->y && (iy+ih) <= (rect1->y+rect1->h) )
		{
			printf("case inside\n");
			*rects=(dg_rect_p*)malloc(4*sizeof(dg_rect_p));
			(*rects)[0] = dg_rect_create(rect1->x, iy, ix-rect1->x, ih); // left
			(*rects)[1] = dg_rect_create(ix+iw, iy, rect1->x+rect1->w-ix-iw, ih); // rigth
			(*rects)[2] = dg_rect_create(rect1->x, rect1->y, rect1->w, iy-rect1->y); // top
			(*rects)[3] = dg_rect_create(rect1->x, iy+ih, rect1->w, rect1->y+rect1->h-ih-iy); // bottom
			*count=4;
			return ;
		}
		else
		if ( ix > rect1->x )
		{
			if (iy > rect1->y)
			{
				printf("case bottom,right\n");
				*rects=(dg_rect_p*)malloc(2*sizeof(dg_rect_p));
				(*rects)[0] = dg_rect_create(rect1->x, iy, ix-rect1->x, ih);
				(*rects)[1] = dg_rect_create(rect1->x, rect1->y, rect1->w, rect1->h-ih);
				*count=2;
			} else
			{
				printf("case top,right\n");
				*rects=(dg_rect_p*)malloc(2*sizeof(dg_rect_p));
				(*rects)[0] = dg_rect_create(rect1->x, rect1->y, rect1->w-iw, ih);
				(*rects)[1] = dg_rect_create(rect1->x, rect1->y+ih, rect1->w, rect1->h-ih);
				*count=2;
			}
		}
		else
		{
			if (iy > rect1->y)
			{
				printf("case bottom,left\n");
				*rects=(dg_rect_p*)malloc(2*sizeof(dg_rect_p));
				(*rects)[0] = dg_rect_create(ix+iw, iy, rect1->w-iw, ih);
				(*rects)[1] = dg_rect_create(rect1->x, rect1->y, rect1->w, rect1->h-ih);
				*count=2;
			} else
			{
				printf("case top,left\n");
				*rects=(dg_rect_p *)malloc(2*sizeof(dg_rect_p));
				(*rects)[0] = dg_rect_create(ix+iw, iy, rect1->w-iw, ih);
				(*rects)[1] = dg_rect_create(ix, iy+ih, rect1->w, rect1->h-ih);
				*count=2;
			}
		}
	}
}

#define test_case(test_name) test_name(rect1, rect2); printf("%s -> ", #test_name); rectangulize(rect1, rect2, &rects, &count); print_rects(rects, count); printf("\n"); free_rects(rects, count);

int main()
{
	dg_rect_p rect1=dg_rect_create(0, 0, 0, 0);
	dg_rect_p rect2=dg_rect_create(0, 0, 0, 0);
	dg_rect_p* rects=0;
	int count=0;

	test_case(outside)
	test_case(inside)
	test_case(top_left)
	test_case(top_right)
	test_case(bottom_left)
	test_case(bottom_right)
	test_case(top)
	test_case(left)
	test_case(right)
	test_case(bottom)
	test_case(top_inside)

	rect1->destroy(rect1);
	rect2->destroy(rect2);

	return 0;
}
