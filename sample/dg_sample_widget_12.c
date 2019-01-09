/*
	dg_sample_widget_11.c
	Tests direct access widget surface
*/

#include <math.h>
#include "dg_sample_shell.h"

#define W_WIDTH  250
#define W_HEIGHT 250

#define EPS 0.000001
char *alpha_channel;

int first1=1;
int first2=1;
int first3=1;
int first4=1;


double c1;
double c2;
double dc1=0.001;
double dc2=0.001;
void render_random_attractor(dg_widget_p self)
{
	if (first1)
	{
		dg_color_t color=0;
		self->surface->clear(self->surface, color);
		first1=0;
		c1 = 1;
		c2 = 2;
	}
	else
	{
		int i;
		dg_color_t color=0;
		self->surface->clear(self->surface, color);
		if (self->surface->lock(self->surface, 1))
		{
			double ff = 10;
			double wx1=-ff;
			double wx2= ff;
			double wy1=-ff;
			double wy2= ff;

			double x = 1;
			double y = 1;
			int Px, Py;
			int Cx = self->surface->width / 2;
			int Cy = self->surface->height / 2;

			if (c1 < wx1 || c1 > wx2) dc1=-dc1;
			if (c2 < wy1 || c2 > wy2) dc2=-dc2;

			c1=c1+dc1;
			c2=c2+dc2;

			for (i=0; i<1000; i++)
			{
				double x1 = tan(x) * tan(x) - sin(y)*sin(y) + c1;
				double y1 = ff*tan(x)*sin(y) + c2;
				//PLOT x1,y1
				Px = (int)((double)self->surface->width * x1 / (wx2-wx1));
				Py = (int)((double)self->surface->height * y1 / (wy2-wy1));
				if (Px>-Cx && Px < Cx &&
					Py>-Cy && Py < Cy)
				{
					self->surface->pixels[Cx+Px+((Cy+Py)*self->surface->width)]=DG_COLOR_WHITE;
				}
				x=x1;
				y=y1;
			}

			self->surface->unlock(self->surface);
		}
	}
}

double Xn=0, Yn=0;
double c3;
double c4;
double dc3=0.0001;
double dc4=0.0001;

void render_henon_map(dg_widget_p self)
{
	if (first2)
	{
		dg_color_t color=0;
		self->surface->clear(self->surface, color);
		first2=0;
		c3 = 0.5;
		c4 = 0.3;
	}
	else
	{
		int i;
		dg_color_t color=0;
		self->surface->clear(self->surface, color);
		if (self->surface->lock(self->surface, 1))
		{
			double ff = 4;
			double wx1=-ff;
			double wx2= ff;
			double wy1=-ff;
			double wy2= ff;
			
			double x = 1;
			double y = 1;
			int Px, Py;
			int Cx = self->surface->width / 2;
			int Cy = self->surface->height / 2;

			if (c1 < wx1 || c1 > wx2) dc1=-dc1;
			if (c2 < wy1 || c2 > wy2) dc2=-dc2;

			c3=c3+dc3;
			c4=c4+dc4;

			for (i=0; i<1000; i++)
			{
				double x1 = 1 + y - c3 * x* x;
				double y1 = c4 * x;

				//PLOT x1,y1
				Px = (int)((double)self->surface->width * x1 / (wx2-wx1));
				Py = (int)((double)self->surface->height * y1 / (wy2-wy1));
				if (Px>-Cx && Px < Cx &&
					Py>-Cy && Py < Cy)
				{
					self->surface->pixels[Cx+Px+((Cy+Py)*self->surface->width)]=DG_COLOR_WHITE;
				}
				x=x1;
				y=y1;
			}

			self->surface->unlock(self->surface);
		}
	}
}

double X0,Y0,Z0;
double a = 10.0;
double b = 28.0;
double c = 8.0 / 3.0;
double dca=0.01;
double dcb=-0.01;
double dcc=0.0001;

void render_lorentz_atractor(dg_widget_p self)
{
	if (first3)
	{
		dg_color_t color=0;
		self->surface->clear(self->surface, color);
		first3=0;
		a = 10.0;
		b = 28.0;
		c = 8.0 / 3.0;

		a =  0;
		b =  0;
		c =  0;

		X0 = 0.1;
		Y0 = 0;
		Z0 = 0;
	}
	else
	{
		dg_color_t color=0;
		self->surface->clear(self->surface, color);
		if (self->surface->lock(self->surface, 1))
		{
			double ff = 30;
			double wx1=-ff;
			double wx2= ff;
			double wy1=-ff;
			double wy2= ff;
			
			int i=0;
			double x1,y1,z1;
			double h = 0.001;

			int Px, Py;
			int Cx = self->surface->width / 2;
			int Cy = self->surface->height / 2;

			//if (a < wx1 || a > wx2) dc1=-dc1;
			//if (c < wy1 || c > wy2) dc2=-dc2;

			//a=a+dc1;
			//c=c+dc2;
			a=a+dca;
			b=b+dcb;
			c=c+dcc;

			if (a>50 || a<0) dca=-dca;
			if (b>100 || b<0) dcb=-dcb;
			if (c<0 || c>10) dcc=-dcc;

			X0 = 0.1;
			Y0 = 0;
			Z0 = 0;

			for (i=0; i<5000; i++)
			{
				x1 = X0 + h * a * (Y0 - X0);
				y1 = Y0 + h * (X0 * (b - Z0) - Y0);
				z1 = Z0 + h * (X0 * Y0 - c * Z0);

				//PLOT x1,y1
				Px = (int)((double)self->surface->width * x1 / (wx2-wx1));
				Py = (int)((double)self->surface->height * y1 / (wy2-wy1));
				if (i > 100)
					if (Px>-Cx && Px < Cx &&
						Py>-Cy && Py < Cy)
					{
						self->surface->pixels[Cx+Px+((Cy+Py)*self->surface->width)]=DG_COLOR_WHITE;
					}
				X0 = x1;
				Y0 = y1;
				Z0 = z1;

			}

			self->surface->unlock(self->surface);
		}
	}
}

void render_fire(dg_widget_p self)
{
	if (first4)
	{
		dg_color_t color=0;
		self->surface->clear(self->surface, color);
		first4=0;
	}
	else
	{
		int i, j;
		if (self->surface->lock(self->surface, 1))
		{
			dg_color_t yellow=DG_COLOR_YELLOW;

			for (i=0; i<self->surface->width; i++)
			{
				self->surface->pixels[(self->surface->height-1)*self->surface->width+i]=DG_COLOR_BLACK;
			}
			for (i=0; i<self->surface->width-4; i++)
			{
				dg_color_t color=DG_RGB_COLOR(rand()%32, rand()%32, 0, 0);

				self->surface->pixels[(self->surface->height-1)*self->surface->width+i] = color;
			}

			for (i=1; i<self->surface->height-1; i++)
			{
				dg_color_t blue_color=DG_COLOR_BLUE;
				//buf[0 + i*self->surface->width]=self->device->pack_color(self->device, blue_color);
				//buf[self->surface->width-1 + (i+1)*self->surface->width]=self->device->pack_color(self->device, blue_color);
				for (j=1; j<self->surface->width-1; j++)
				{
					dg_color_t color;
					dg_color_t color1=self->surface->pixels[-1+j + (i+1)*self->surface->width];
					dg_color_t color2=self->surface->pixels[0+j + (i+1)*self->surface->width];
					dg_color_t color3=self->surface->pixels[1+j + (i+1)*self->surface->width];
					dg_color_t color4=self->surface->pixels[0+j + (i)*self->surface->width];

					color4=DG_RGB_COLOR(
						DG_GET_RED(color4)<<1, 
						DG_GET_GREEN(color4)<<1, 
						DG_GET_BLUE(color4)<<1, 
						0);
					DG_SET_RED(color, (DG_GET_RED(color1) + DG_GET_RED(color2) + DG_GET_RED(color3) + DG_GET_RED(color4))/4);
					//if (color.red>0)color.red--;
					//if (color.red>0)color.red--;
					//if (color.red>0)color.red--;
					if (DG_GET_RED(color)<255) DG_ADD_RED(color, 1);
					DG_SET_RED(color, 0);

					DG_SET_GREEN(color, (DG_GET_GREEN(color1) + DG_GET_GREEN(color2) + DG_GET_GREEN(color3) + DG_GET_GREEN(color4))/4);
					//if (color.green>0)color.green--;
					//if (color.green>0)color.green--;
					//if (color.green>0)color.green--;
					if (DG_GET_GREEN(color)<255) DG_ADD_GREEN(color, 1);


					DG_SET_BLUE(color, (DG_GET_BLUE(color1) + DG_GET_BLUE(color2) + DG_GET_BLUE(color3) + DG_GET_BLUE(color4))/4);
					//if (color.blue>0)color.blue--;
					//if (color.blue>0)color.blue--;
					//if (color.blue>0)color.blue--;
					if (DG_GET_BLUE(color)<255) DG_ADD_BLUE(color, 1);
					if (DG_GET_BLUE(color)<255) DG_ADD_BLUE(color, 1);
					if (DG_GET_BLUE(color)<255) DG_ADD_BLUE(color, 1);
					DG_SET_ALPHA(color, 0);

					self->surface->pixels[j + i*self->surface->width]=color;
					//r=color & 0xFF;
					//g=(color>>8) & 0xFF;
					//b=(color>>16) & 0xFF;

				}
			}
			self->surface->unlock(self->surface);
		}
	}
}

int event_handler(dg_video_p device, dg_event_p event)
{
	if (!event)
	{
		int w=0;
		dg_widget_p fire_widget = device->create_widget(device, sizeof(dg_widget_t), 0, device->yres-W_HEIGHT+10, W_WIDTH, W_HEIGHT);
		dg_widget_p attractor_widget = device->create_widget(device, sizeof(dg_widget_t), device->xres-W_WIDTH, 0, W_WIDTH, W_HEIGHT);
		dg_widget_p henon_map_widget = device->create_widget(device, sizeof(dg_widget_t), 0, 0, W_WIDTH, W_HEIGHT);
		dg_widget_p lorentz_widget = device->create_widget(device, sizeof(dg_widget_t), device->xres-W_WIDTH, device->yres-W_HEIGHT, W_WIDTH, W_HEIGHT);

		fire_widget->render=render_fire;
		attractor_widget->render=render_random_attractor;
		henon_map_widget->render=render_henon_map;
		lorentz_widget->render=render_lorentz_atractor;
		do
		{
			test_performance();
			fire_widget->render(fire_widget);
			attractor_widget->render(attractor_widget);
			henon_map_widget->render(henon_map_widget);
			lorentz_widget->render(lorentz_widget);

			fire_widget->invalidate(fire_widget, 0);
			attractor_widget->invalidate(attractor_widget, 0);
			henon_map_widget->invalidate(henon_map_widget, 0);
			lorentz_widget->invalidate(lorentz_widget, 0);

			swap_buffers(device);
			device->master_widget->event_manager->idle(device->master_widget->event_manager);
		} while (device->master_widget->event_manager->active);
	}
	return 1;
}

int main()
{
	start_sample(event_handler);
	return 0;
}
