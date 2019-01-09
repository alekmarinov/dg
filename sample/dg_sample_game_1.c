/*
	dg_sample_game_1.c
	Arkanoid game
*/

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <dg/dg_video.h>
#include <dg/dg_event.h>
#include <dg/dg_keys.h>
#include <dg/dg_surface.h>
#include <dg/dg_widget.h>
#include <dg/dg_rect.h>

#define XRES 640
#define YRES 480
#define BPP 32
#define OPTIONS DG_OPT_DOUBLE_BUFFERING

int initialize();

dg_video_p device=0;

void swap_buffers()
{
	if (device->options & DG_OPT_DOUBLE_BUFFERING)
	{
		DGFlip(device, 0);
	}
}

/* game definitions */
#define LIVES_COUNT 3

int current_lives=LIVES_COUNT;
int game_over=0;

/* brick definitions */
#define BRICKS_OFFSET_LEFT 20
#define BRICKS_OFFSET_RIGHT 20
#define BRICKS_OFFSET_TOP 50
#define BRICKS_OFFSET_BOTTOM 350
#define BRICKS_X_SPACE 2
#define BRICKS_Y_SPACE 2
#define BRICKS_X_COUNT 10
#define BRICKS_Y_COUNT  4
#define BRICK_WIDTH  ((XRES-BRICKS_OFFSET_LEFT-BRICKS_OFFSET_RIGHT -(BRICKS_X_COUNT-1)*BRICKS_X_SPACE)/BRICKS_X_COUNT)
#define BRICK_HEIGHT ((YRES-BRICKS_OFFSET_TOP -BRICKS_OFFSET_BOTTOM-(BRICKS_Y_COUNT-1)*BRICKS_Y_SPACE)/BRICKS_Y_COUNT)

#define BACKGROUND "c:\\cvs\\lrun-art\\projects\\aviq\\a1\\image\\backgroundRudy.jpg"

typedef struct {
	dg_widget_p w;
	int active;
	int dir_x;
	int dir_y;
} brick_t;
brick_t bricks[BRICKS_Y_COUNT][BRICKS_X_COUNT]={0};
int active_bricks=0;
void brick_renderer(dg_widget_p self) 
{ 
	DGSurfaceClear(self->surface, DG_COLOR_RED);
	DGSurfaceSetAlpha(self->surface, 200);
}

void initialize_bricks()
{
	int i, j;
	active_bricks=BRICKS_X_COUNT * BRICKS_Y_COUNT;
	for (i=0; i<BRICKS_Y_COUNT; i++)
		for (j=0; j<BRICKS_X_COUNT; j++)
		{
			if (!bricks[i][j].w)
			{
				bricks[i][j].w=DGCreateWidget(device, sizeof(dg_widget_t), BRICKS_OFFSET_LEFT+j*(BRICK_WIDTH+BRICKS_X_SPACE), BRICKS_OFFSET_TOP+i*(BRICK_HEIGHT+BRICKS_Y_SPACE), BRICK_WIDTH, BRICK_HEIGHT);
			}
			else
			{
				DGWidgetSetRect(bricks[i][j].w, BRICKS_OFFSET_LEFT+j*(BRICK_WIDTH+BRICKS_X_SPACE), BRICKS_OFFSET_TOP+i*(BRICK_HEIGHT+BRICKS_Y_SPACE), BRICK_WIDTH, BRICK_HEIGHT, 1);
				DGWidgetSetVisible(bricks[i][j].w, 1);
			}

			DGWidgetSetRenderer(bricks[i][j].w, brick_renderer);
			DGWidgetRender(bricks[i][j].w);
			DGWidgetInvalidate(bricks[i][j].w, 0);

			bricks[i][j].active=1;
			bricks[i][j].dir_x=rand()%3-1;
			bricks[i][j].dir_y=rand()%3-1;
			while (!bricks[i][j].dir_x && !bricks[i][j].dir_y)
			{
				bricks[i][j].dir_x=rand()%3-1;
				bricks[i][j].dir_y=rand()%3-1;
			}
		}
}

/* stick definitions */
#define STICK_WIDTH  80
#define STICK_HEIGHT 10
#define STICK_OFFSET_BOTTOM 5
typedef struct {
	dg_widget_p w;
} stick_t;
stick_t stick={0};
void stick_renderer(dg_widget_p self) { DGSurfaceClear(self->surface, DG_COLOR_BLUE); }

#define COL(c, s) (((c) >> s) & 0xFF)
#define FIRE_COLOR1(c1, c2, c3, c4, s) ((( COL(c1, s) +  COL(c2, s) + COL(c3, s) + COL(c4, s))/4)&(0xFF))
#define FIRE_COLOR(c1, c2, c3, c4, s) (FIRE_COLOR1(c1, c2, c3, c4, s)>255?255:FIRE_COLOR1(c1, c2, c3, c4, s));

void stick_fire_renderer(dg_widget_p self)
{
	int i, j;
	if (DGSurfaceLock(self->surface, 1))
	{
		for (i=0; i<DGSurfaceWidth(self->surface); i++)
		{
			if (rand()%5 == 1)
				DGSurfaceSetPixel(self->surface, i, DGSurfaceHeight(self->surface)-1, DG_COLOR_YELLOW)
			else
				DGSurfaceSetPixel(self->surface, i, DGSurfaceHeight(self->surface)-1, DG_COLOR_BLACK)
		}

		for (i=0; i<self->surface->height-1; i++)
		{
			for (j=1; j<self->surface->width-1; j++)
			{
				dg_color_t color;
				unsigned char r, g, b;
				dg_color_t color1=DGSurfacePixel(self->surface, -1+j, i+1);
				dg_color_t color2=DGSurfacePixel(self->surface, 0+j,  i+1);
				dg_color_t color3=DGSurfacePixel(self->surface, 1+j,  i+1);
				dg_color_t color4=DGSurfacePixel(self->surface, 0+j,  i);

				//color.red=(color1.red + color2.red + color3.red + color4.red)/4;
				r=FIRE_COLOR(color1, color2, color3, color4, R_SHIFT);
				DG_SET_RED(color, r);

				if (DG_GET_RED(color)>0) 
				{
					DG_SET_RED(color, DG_GET_RED(color) - 1);
				}
				//if (color.red<255)color.red++;

				//color.green=(color1.green + color2.green + color3.green + color4.green)/4;
				g=FIRE_COLOR(color1, color2, color3, color4, G_SHIFT);
				DG_SET_GREEN(color, g);
				if (DG_GET_GREEN(color)>0) 
				{
					DG_SET_GREEN(color, DG_GET_GREEN(color) - 1);
				}

				//if (color.green<255)color.green++;

				//color.blue=(color1.blue + color2.blue + color3.blue + color4.blue)/4;
				b=FIRE_COLOR(color1, color2, color3, color4, B_SHIFT);
				DG_SET_BLUE(color, b);
				if (DG_GET_BLUE(color)>0) 
				{
					DG_SET_BLUE(color, DG_GET_BLUE(color) - 1);
				}
				//if (color.blue<255)color.blue++;

				DG_SET_ALPHA(color, 0);

				DGSurfaceSetPixel(self->surface, j, i, color);
				//r=color & 0xFF;
				//g=(color>>8) & 0xFF;
				//b=(color>>16) & 0xFF;

			}
		}
		DGSurfaceUnlock(self->surface);
	}
}

stick_t lives[LIVES_COUNT]={0};

void initialize_stick()
{
	if (!stick.w)
	{
		stick.w=DGCreateWidget(device, sizeof(dg_widget_t), (XRES-STICK_WIDTH)/2, YRES-STICK_HEIGHT-STICK_OFFSET_BOTTOM, STICK_WIDTH, STICK_HEIGHT);
	}
	else
	{
		DGWidgetSetRect(stick.w, (XRES-STICK_WIDTH)/2, YRES-STICK_HEIGHT-STICK_OFFSET_BOTTOM, STICK_WIDTH, STICK_HEIGHT, 1);
	}
	DGWidgetSetRenderer(stick.w, stick_renderer);
	DGWidgetRender(stick.w);
	DGWidgetInvalidate(stick.w, 0);
}

void live_renderer(dg_widget_p self) 
{ 
	DGSurfaceClear(self->surface, DG_COLOR_BLUE);
	DGSurfaceSetAlpha(self->surface, 128);
}

void initialize_lives()
{
	int i;
	current_lives=LIVES_COUNT;
	for (i=0; i<LIVES_COUNT; i++)
	{
		if (!lives[i].w)
		{
			lives[i].w=DGCreateWidget(device, sizeof(dg_widget_t), 5+i*(STICK_WIDTH+5), 5, STICK_WIDTH, STICK_HEIGHT);
		}
		else
		{
			DGWidgetSetRect(lives[i].w, 5+i*(STICK_WIDTH+5), 5, STICK_WIDTH, STICK_HEIGHT, 1);
			DGWidgetSetVisible(lives[i].w, 1);
		}
		DGWidgetSetRenderer(lives[i].w, live_renderer);
		DGWidgetRender(lives[i].w);
		DGWidgetInvalidate(lives[i].w, 0);
	}
}

/* ball definitions */
#define BALL_WIDTH  6
#define BALL_HEIGHT 6
#define MAX_BALLS   5
#define INITIAL_SPEED 0.9

typedef struct 
{
	dg_widget_p w;
	int sticked;
	double dir_x;
	double dir_y;
	double pos_x;
	double pos_y;
	int offset;
	int active;
} ball_t, *ball_p;
ball_t balls[MAX_BALLS]={0};
int ball_count=1;

double speed=INITIAL_SPEED;

void ball_renderer(dg_widget_p self)
{
	DGSurfaceClear(self->surface, DG_COLOR_WHITE); 
	DGSurfaceFillRect(self->surface, 0, 0, 1, 1, DG_COLOR_BLACK); 
	DGSurfaceFillRect(self->surface, BALL_WIDTH-1, 0, 1, 1, DG_COLOR_BLACK); 
	DGSurfaceFillRect(self->surface, 0, BALL_HEIGHT-1, 1, 1, DG_COLOR_BLACK); 
	DGSurfaceFillRect(self->surface, BALL_WIDTH-1, BALL_HEIGHT-1, 1, 1, DG_COLOR_BLACK); 
	DGSurfaceSetColorKey(self->surface, DG_COLOR_BLACK);
}

int detect_collision(dg_window_p o1, dg_window_p o2)
{
	int ix, iy, iw, ih;
	if (o1->intersect(o1, o2, &ix, &iy, &iw, &ih))
	{
		int x=o2->rect->x + o2->rect->w/2;
		int y=o2->rect->y + o2->rect->h/2;

		/* horizontal collision */
		if (x<o1->rect->x)
		{
			return -1; 
		} 
		else
		if (x>o1->rect->x+o1->rect->w)
		{
			return -2;
		}

		/* vertical collision */
		if (y<o1->rect->y)
		{
			return 1;  
		}
		else
		if (y>o1->rect->y+o1->rect->h)
		{
			return 2;
		}
	}
	return 0;
}

void ball_advance()
{
	int b;
	for (b=0; b<MAX_BALLS; b++)
	{
		if (balls[b].active)
		{
			ball_p ball=&balls[b];
			if (!ball->sticked)
			{
				ball->pos_x+=speed*ball->dir_x;
				ball->pos_y+=speed*ball->dir_y;
				DGWidgetSetRect(ball->w, (int)ball->pos_x, (int)ball->pos_y, BALL_WIDTH, BALL_HEIGHT, 1);

				if (DGWidgetX(ball->w) < 0 || DGWidgetX(ball->w)>(XRES-BALL_WIDTH))
				{
					ball->dir_x=-ball->dir_x;
				}

				if (DGWidgetY(ball->w) < 0)
				{
					ball->dir_y=-ball->dir_y;
				} 
				else
				if (DGWidgetY(ball->w)>(YRES))
				{
					ball->sticked=1;
					//ball->w->set_rect(ball->w, ball->w->window->rect->x, stick.w->window->rect->y - BALL_HEIGHT, ball->w->window->rect->w, ball->w->window->rect->h, 1);

					ball->offset=STICK_WIDTH/2;
					DGWidgetSetRect(ball->w, stick.w->window->rect->x+ball->offset-BALL_WIDTH/2, stick.w->window->rect->y - BALL_HEIGHT, BALL_WIDTH, BALL_HEIGHT, 1);
					ball->pos_x=(double)DGWidgetX(ball->w);
					ball->pos_y=(double)DGWidgetY(ball->w);
					ball->dir_x=0;
					ball->dir_y=-1;

					/* You are dead! */
					if (current_lives>0)
					{
						current_lives--;
						DGWidgetSetVisible(lives[current_lives].w, 0);
						DGWidgetInvalidate(lives[current_lives].w, 0);
					} 
					else
					{
						/* Game Over! */
						game_over=1;
					}
				}

				/* check collisions */
				{
					/* brick collision */
					int i, j;
					int collision_type;
					for (i=0; i<BRICKS_Y_COUNT; i++)
						for (j=0; j<BRICKS_X_COUNT; j++)
						{
							if (bricks[i][j].active)
							{
								collision_type=detect_collision(bricks[i][j].w->window, ball->w->window);
								if (collision_type)
								{
									/* collision */
									active_bricks--;
									bricks[i][j].active=0;
									DGWidgetSetVisible(bricks[i][j].w, 0);
									DGWidgetInvalidate(bricks[i][j].w, 0);
									if (collision_type < 0) /* horizontal */
									{
										ball->dir_x=-ball->dir_x;
									} 
									else
									if (collision_type > 0) /* vertical */
									{
										ball->dir_y=-ball->dir_y;
									}

									if (!active_bricks)
									{
										/* next level */
										if (ball_count<MAX_BALLS)
										{
											ball_count++;
										}
										else
										{
											speed+=0.3;
											ball_count=1;
										}
										initialize();
									}
								}
							}
						}
					/* stick collision */
					collision_type=detect_collision(stick.w->window, ball->w->window);
					if (collision_type)
					{
						DGWidgetSetRect(ball->w, DGWidgetX(ball->w), DGWidgetY(ball->w)-BALL_HEIGHT-1, BALL_WIDTH, BALL_HEIGHT, 1);
						if (collision_type == 1) /* top vertical */
						{
							double alpha;
							//ball->sticked=1;
							ball->offset=DGWidgetX(ball->w)-DGWidgetX(stick.w)+BALL_WIDTH/2;
							alpha=(double)(STICK_WIDTH-ball->offset)*M_PI/STICK_WIDTH;
							if (alpha<M_PI/12) alpha=M_PI/12;
							if (alpha>(M_PI-M_PI/12)) alpha=(M_PI-M_PI/12);
							ball->dir_y=-sin(alpha);
							ball->dir_x=cos(alpha);

							if (ball->dir_y>0)
							{
								ball->dir_y=-ball->dir_y;
							}
						}
					}
				}
			}
		}
	}
	swap_buffers();
}

void initialize_ball()
{
	int b;
	for (b=0; b<MAX_BALLS; b++)
	{
		ball_p ball=&balls[b];

		if (b<ball_count)
			ball->active=1;
		else 
			ball->active=0;

		if (!ball->w)
		{
			ball->w=DGCreateWidget(device, sizeof(dg_widget_t), (XRES-BALL_WIDTH)/2-BALL_WIDTH*(MAX_BALLS/2)+BALL_WIDTH*b, YRES-BALL_HEIGHT-STICK_HEIGHT-STICK_OFFSET_BOTTOM, BALL_WIDTH, BALL_HEIGHT);
		}
		else
		{
			DGWidgetSetRect(ball->w, (XRES-BALL_WIDTH)/2-BALL_WIDTH*(MAX_BALLS/2)+BALL_WIDTH*b, YRES-BALL_HEIGHT-STICK_HEIGHT-STICK_OFFSET_BOTTOM, BALL_WIDTH, BALL_HEIGHT, 1);
		}
		DGWidgetSetVisible(ball->w, ball->active);
		DGWidgetSetRenderer(ball->w, ball_renderer);
		DGWidgetRender(ball->w);
		DGWidgetInvalidate(ball->w, 0);
		ball->sticked=1;
		ball->pos_x=(double)DGWidgetX(ball->w);
		ball->pos_y=(double)DGWidgetY(ball->w);
		ball->dir_x=0.0;
		ball->dir_y=-1.0;
		ball->offset=STICK_WIDTH/2;
	}
}

int keyboard_handler(dg_video_p device, dg_event_p event)
{
	if (event->e.keyboard.key == KEY_Escape)
	{
		device->master_widget->event_manager->active=0;
	}
	return 1;
}

int mouse_handler(dg_video_p device, dg_event_p event)
{
	int b;
	if (event->e.mouse.mouse_event & EVT_MOUSE_LBUTTON && event->e.mouse.mouse_event & EVT_PRESSED)
	{
		/* Fire */
		if (game_over)
		{
			game_over=0;
			speed=INITIAL_SPEED;
			ball_count=1;
			initialize();
		}
		else
		{
			for (b=0; b<MAX_BALLS; b++)
			{
				if (balls[b].active)
				{
					balls[b].sticked=0;
				}
			}
		}
	}
	if (!game_over)
	{
		
		/* move */
		if (event->e.mouse.x >= (XRES-STICK_WIDTH/2))
		{
			event->e.mouse.x=XRES-STICK_WIDTH/2;
		}
		if (event->e.mouse.x < (STICK_WIDTH/2))
		{
			event->e.mouse.x=STICK_WIDTH/2;
		}
		DGWidgetSetRect(stick.w, event->e.mouse.x-STICK_WIDTH/2, DGWidgetY(stick.w), DGWidgetWidth(stick.w), DGWidgetHeight(stick.w), 1);
		for (b=0; b<MAX_BALLS; b++)
		{
			if (balls[b].active && balls[b].sticked)
			{
				DGWidgetSetRect(balls[b].w, DGWidgetX(stick.w)+balls[b].offset-BALL_WIDTH/2-BALL_WIDTH*(MAX_BALLS/2)+BALL_WIDTH*b, DGWidgetY(balls[b].w), BALL_WIDTH, BALL_HEIGHT, 1);
				balls[b].pos_x=(double)DGWidgetX(balls[b].w);
			}
		}
		swap_buffers();
	}
	return 1;
}

void game()
{
	do 
	{	
		if (game_over)
		{
			int i,j;
			DGWidgetSetRenderer(stick.w, stick_fire_renderer);
			DGWidgetSetRect(stick.w, DGWidgetX(stick.w), YRES-10*STICK_HEIGHT, DGWidgetWidth(stick.w), STICK_HEIGHT*10, 1);
			DGWidgetRender(stick.w);
			DGWidgetInvalidate(stick.w, 0);

			for (i=0; i<BRICKS_Y_COUNT; i++)
				for (j=0; j<BRICKS_X_COUNT; j++)
				{
					dg_rect_p rect=bricks[i][j].w->window->rect;
					DGWidgetSetRect(bricks[i][j].w, DGWidgetX(bricks[i][j].w)+bricks[i][j].dir_x, DGWidgetY(bricks[i][j].w)+bricks[i][j].dir_y, DGWidgetWidth(bricks[i][j].w), DGWidgetHeight(bricks[i][j].w), 1);
				}

			swap_buffers();
		}
		else
		{
			ball_advance();
		}

		device->master_widget->event_manager->idle(device->master_widget->event_manager);
	} while (device->master_widget->event_manager->active);
}

void deinitialize()
{
	device->destroy(device);
}

int initialize()
{
	if (!device)
	{
		/* initialize video device */
		device = DGCreate(sizeof(dg_widget_t), XRES, YRES, BPP, OPTIONS);
		device->set_background_color(device, DG_COLOR_BLUE);
		device->set_background_image(device, device->load_surface_file(device, BACKGROUND));

		if (device)
		{
			device->master_widget->event_manager->add_listener(device->master_widget->event_manager, keyboard_handler, EVT_TYPE_KEYBOARD, device);
			device->master_widget->event_manager->add_listener(device->master_widget->event_manager, mouse_handler, EVT_TYPE_MOUSE, device);
		}
	}
	if (device)
	{
		/* initialize game */
		initialize_ball();
		initialize_stick();
		initialize_bricks();
		initialize_lives();

		/* show game */
		swap_buffers();

		return 1;
	}
	else
	{
		printf("Unable to initialize video driver (%dx%d-%d)\n", XRES, YRES, BPP);
	}
	return 0;
}

int main()
{
	if (initialize())
	{
		game();
		deinitialize();
	}
	return 0;
}
