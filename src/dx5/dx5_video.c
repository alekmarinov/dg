/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dx5_video.c                                        */
/* Description:   DG/DirectX Video implementation                    */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_widget.h>
#include "dx5_video.h"
#include "dx5_surface.h"
#include "dx5_event.h"
#include "dx5_blit.h"
#include "dx5_error.h"
#include <zmouse.h>

#ifndef GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif

/*
	Disable warnings ('LONG' differs in levels of indirection from 'HBRUSH')
	in SetClassLong and SetWindowLong when assigning pointer instead of long
*/
#pragma warning (push)
#pragma warning( disable : 4047 4312 )

/* global definitions */
#define STYLE_FULLSCREEN WS_POPUP
#define STYLE_WINDOW WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS

#define EXSTYLE_FULLSCREEN WS_EX_TOPMOST
#define EXSTYLE_WINDOW WS_EX_APPWINDOW

const char *dx5_app_name="DG/DirectX App";

static int  dx5_unset_mode(dg_video_p);
static void dx5_set_popup_style(HWND);
static void dx5_set_window_style(HWND, int, int);
static void dx5_update_client_rect(dg_video_p);
static void dx5_pack_mask(DWORD, BYTE*, BYTE*);
static int  dx5_set_mode(dg_video_p device);
static void dx5_clear_buffers(dg_video_p);
static void dx5_master_flip(dg_surface_p, int, int, dg_rect_p);
static int dx5_master_blit(dg_surface_p, dg_rect_p, dg_surface_p, dg_rect_p, unsigned char, unsigned char*, dg_color_t, int);
static void dx5_master_resize(dg_surface_p, int, int);
static void dx5_master_fill_rect(dg_surface_p, int, int, int, int, dg_color_t);
static void dx5_master_clear(dg_surface_p, dg_color_t);
static int  dx5_master_lock(dg_surface_p, void**, unsigned long*);
static int  dx5_master_unlock(dg_surface_p);
static void dx5_destroy(dg_video_p);
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static void dx5_video_init_master_widget(dg_video_p);
static void dx5_video_set_mouse_capture(dg_video_p, int);
//static void dx5_video_set_background_image(dg_video_p, dg_color_t);
//static void dx5_video_set_background_color(dg_video_p, dg_surface_p);

static int dx5_unset_mode(dg_video_p device)
{
	if (device && device->context)
	{
		dx5_video_p context=device->context;

		if (context->lpDD)
		{
			dx5_surface_p screen_context=(dx5_surface_p)context->screen->context;

			if (device->options & DG_OPT_FULLSCREEN)
			{
				/* Restore display mode */
				IDirectDraw_RestoreDisplayMode(context->lpDD);
			}

			if (screen_context->lpDDSPrimary)
			{
				/* Release primary surface */
				IDirectDrawSurface_Release(screen_context->lpDDSPrimary);
				screen_context->lpDDSPrimary=0;
			}

			if ((device->options & DG_OPT_DOUBLE_BUFFERING) && (!(device->options & DG_OPT_FULLSCREEN)) && screen_context->lpDDSBack)
			{
				/* Release backbuffer surface */
				IDirectDrawSurface_Release(screen_context->lpDDSBack);
				screen_context->lpDDSBack=0;
			}
		}
	}
	return 1;
}

static void dx5_set_popup_style(HWND hWnd)
{
	HBRUSH hbrush;

    ShowWindow(hWnd, SW_MINIMIZE);

    SetWindowLong(hWnd, GWL_EXSTYLE, EXSTYLE_FULLSCREEN);
    SetWindowLong(hWnd, GWL_STYLE, STYLE_FULLSCREEN);

    hbrush = (HBRUSH)(GetStockObject(NULL_BRUSH));

	SetClassLong(hWnd, GCL_HBRBACKGROUND, hbrush);

    ShowWindow(hWnd, SW_RESTORE);
}

static void dx5_set_window_style(HWND hWnd, int w, int h)
{
    HBRUSH hbrush;
    RECT rect;
    int iX,iY;
    long lS, lES, lM;

    ShowWindow(hWnd, SW_MINIMIZE);

    SetWindowLong(hWnd, GWL_EXSTYLE, EXSTYLE_WINDOW);
    SetWindowLong(hWnd, GWL_STYLE, STYLE_WINDOW);

    hbrush = (HBRUSH)((HBRUSH)(COLOR_WINDOW+1));
    SetClassLong(hWnd, GCL_HBRBACKGROUND, hbrush);

    ShowWindow(hWnd, SW_RESTORE);

    iX = (GetSystemMetrics(SM_CXSCREEN)-w)/2;
    iY = (GetSystemMetrics(SM_CYSCREEN)-h)/2;

    rect.left = 0;
    rect.top = 0;
    rect.right = w;
    rect.bottom = h;

    lS = GetWindowLong(hWnd, GWL_STYLE);
    lES = GetWindowLong(hWnd, GWL_EXSTYLE);
    lM = GetWindowLong(hWnd, GCL_MENUNAME);

    AdjustWindowRectEx(&rect, lS, lM, lES);

    w = rect.right - rect.left;
    h = rect.bottom - rect.top;

    SetWindowPos(hWnd, HWND_TOP, iX, iY, w, h, 0);
}

static void dx5_update_client_rect(dg_video_p device)
{
	if (device && device->context)
	{
		RECT rect;
		POINT pUL, pBR;
		dx5_video_p context=device->context;

		GetClientRect(context->hwndMain, &rect);

		pUL.x = rect.left;
		pUL.y = rect.top;
		pBR.x = rect.right;
		pBR.y = rect.bottom;

		ClientToScreen(context->hwndMain, &pUL);
		ClientToScreen(context->hwndMain, &pBR);

		SetRect(&(context->rectClient), pUL.x, pUL.y, pBR.x, pBR.y);
	}
}

static void dx5_pack_mask(DWORD mask, BYTE* bits, BYTE* shifts)
{
	int maxBits=32;
	*bits=0;
	*shifts=0;
	while (maxBits>0 && (mask & 1) == 0)
	{
		(*shifts)++;
		mask>>=1;
		maxBits--;
	}

	while (maxBits>0 && (mask & 1) == 1)
	{
		(*bits)++;
		mask>>=1;
		maxBits--;
	}
}

static int dx5_set_mode(dg_video_p device)
{
	dx5_video_p context=device->context;
    DDSURFACEDESC ddsd;
    DDSCAPS       ddscaps;
    LPDIRECTDRAWCLIPPER lpddClipper;
	HRESULT hResult;
    HDC hdc;
	dx5_surface_p screen_context;

    /* if the window doesn't exist yet, create it */
    if (! (context && context->hwndMain) )
    {
        WNDCLASS wc;
		int w, h;

		/* Register window class */
        wc.lpszClassName = dx5_app_name;
        wc.lpfnWndProc = WndProc;
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wc.hInstance = GetModuleHandle(0);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(device->options & DG_OPT_FULLSCREEN?NULL_BRUSH:BLACK_BRUSH);
        wc.lpszMenuName  = 0;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;

        if (!(RegisterClass(&wc)))
		{
			printf("RegisterClass failed!");
			return 0;
		}

		/* Calculate appropriate window size */
        if (device->options & DG_OPT_FULLSCREEN)
        {
			w = GetSystemMetrics(SM_CXSCREEN);
			h = GetSystemMetrics(SM_CYSCREEN);
        } 
		else
		{
			RECT rect;
			SetRect(&rect, 0, 0, device->xres, device->yres);
            AdjustWindowRectEx(&rect, STYLE_WINDOW, FALSE, EXSTYLE_WINDOW);
            w = rect.right  - rect.left;
            h = rect.bottom - rect.top;
		}

        /* create window */
		context->hwndMain = CreateWindowEx(
            (device->options & DG_OPT_FULLSCREEN?EXSTYLE_FULLSCREEN:EXSTYLE_WINDOW),
            dx5_app_name,
            dx5_app_name,
            (device->options & DG_OPT_FULLSCREEN?STYLE_FULLSCREEN:STYLE_WINDOW),
			CW_USEDEFAULT, CW_USEDEFAULT, w, h,
            NULL, NULL, wc.hInstance, NULL
		);

        if (!(context->hwndMain))
		{
			printf("CreateWindowEx failed!");
			return 0;
		}

        /* set window's userdata to common video device */
        SetWindowLong(context->hwndMain, GWL_USERDATA, device);

        /* show this window finally */
		ShowWindow(context->hwndMain, SW_SHOW);
		SetForegroundWindow(context->hwndMain);
    }
    else
    {
        /* otherwise set the right style */
        if (device->options & DG_OPT_FULLSCREEN)
            dx5_set_popup_style(context->hwndMain);
        else
            dx5_set_window_style(context->hwndMain, device->xres, device->yres);
    }

    hResult = IDirectDraw_SetCooperativeLevel(context->lpDD, context->hwndMain,
        device->options & DG_OPT_FULLSCREEN?DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN:DDSCL_NORMAL|DDSCL_NOWINDOWCHANGES);
	
    if (FAILED(hResult))
    {
		DX5_CHECK_RESULT(hResult)
		return 0;
    }

    if (!(device->options & DG_OPT_FULLSCREEN))
    {
        hdc = GetDC(0);
		device->bpp = GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL);
        ReleaseDC(0, hdc);
    }

    if (device->options & DG_OPT_FULLSCREEN)
	{
		hResult = IDirectDraw_SetDisplayMode(context->lpDD, device->xres, device->yres, device->bpp);
		if (FAILED(hResult))
        {
			DX5_CHECK_RESULT(hResult)
			return 0;
        }
	}

    ddsd.dwSize = sizeof(ddsd);
	if (device->options & DG_OPT_FULLSCREEN)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 0;
	}

	screen_context = (dx5_surface_p)context->screen->context;
	hResult = IDirectDraw_CreateSurface(context->lpDD, &ddsd, &(screen_context->lpDDSPrimary), NULL);
    if (FAILED(hResult))
    {
		DX5_CHECK_RESULT(hResult)
		return 0;
    }

	if (device->options & DG_OPT_DOUBLE_BUFFERING)
	{
		/* attach back buffer */
		if (device->options & DG_OPT_FULLSCREEN)
		{
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
			hResult = IDirectDrawSurface_GetAttachedSurface(screen_context->lpDDSPrimary,
				&ddscaps, &(screen_context->lpDDSBack));

			if(FAILED(hResult))
			{
				DX5_CHECK_RESULT(hResult)
				return 0;
			}
		}
		else
		{
			if (FAILED(dx5_create_physical_surface(context->lpDD, device->xres, device->yres, &(screen_context->lpDDSBack), &screen_context->ddsd, 0)))
			{
				return 0;
			}
		}
	}
	else
	{
		/* no attached back buffers */
		screen_context->lpDDSBack = screen_context->lpDDSPrimary;
	}

    if (!(device->options & DG_OPT_FULLSCREEN))
    {
        hResult = DirectDrawCreateClipper(0, &lpddClipper, NULL);
        if (FAILED(hResult))
        {
			DX5_CHECK_RESULT(hResult)
			return 0;
        }
        hResult = IDirectDrawClipper_SetHWnd(lpddClipper, 0, context->hwndMain);
        if (FAILED(hResult))
        {
            IDirectDrawClipper_Release(lpddClipper);
			DX5_CHECK_RESULT(hResult)
			return 0;
        }
        hResult = IDirectDrawSurface_SetClipper(screen_context->lpDDSPrimary, lpddClipper);
        if (FAILED(hResult))
        {
            IDirectDrawClipper_Release(lpddClipper);
			DX5_CHECK_RESULT(hResult)
			return 0;
        }
        IDirectDrawClipper_Release(lpddClipper);
    }

    screen_context->ddpf.dwSize = sizeof(DDPIXELFORMAT);
    hResult = IDirectDrawSurface_GetPixelFormat(screen_context->lpDDSPrimary, &(screen_context->ddpf));
    if (FAILED(hResult))
    {
		DX5_CHECK_RESULT(hResult)
		return 0;
    }

	/* set color info */
	dx5_pack_mask(screen_context->ddpf.dwRBitMask, &device->pixel_format.bRedBits, &device->pixel_format.bRedShift);
	dx5_pack_mask(screen_context->ddpf.dwGBitMask, &device->pixel_format.bGreenBits, &device->pixel_format.bGreenShift);
	dx5_pack_mask(screen_context->ddpf.dwBBitMask, &device->pixel_format.bBlueBits, &device->pixel_format.bBlueShift);
	dx5_pack_mask(screen_context->ddpf.dwRGBAlphaBitMask, &device->pixel_format.bAlphaBits, &device->pixel_format.bAlphaShift);
	device->pixel_format.bitsPerPixel = (unsigned char)screen_context->ddpf.dwRGBBitCount;
	if (device->pixel_format.bRedBits + device->pixel_format.bGreenBits + 
		device->pixel_format.bBlueBits + device->pixel_format.bAlphaBits < device->pixel_format.bitsPerPixel)
	{
		device->pixel_format.bAlphaBits = device->pixel_format.bitsPerPixel - 
			(device->pixel_format.bRedBits + device->pixel_format.bGreenBits + device->pixel_format.bBlueBits);
	}

    dx5_update_client_rect(device);

    return 1;
}

/* Clear primary and backbuffer surfaces */
static void dx5_clear_buffers(dg_video_p device)
{
	if (device && device->context)
	{
		dx5_video_p context=device->context;
		dx5_surface_p screen_context=(dx5_surface_p)context->screen->context;
		HRESULT hResult;
	    DDBLTFX ddBltFx;
		ddBltFx.dwSize = sizeof(DDBLTFX);
		ddBltFx.dwFillColor = 0;

		if (device->options & DG_OPT_DOUBLE_BUFFERING)
		{
			hResult=IDirectDrawSurface_Blt(screen_context->lpDDSBack, NULL, NULL, NULL, /*DDBLT_WAIT*/DDBLT_DONOTWAIT | DDBLT_ASYNC | DDBLT_COLORFILL, &ddBltFx);
			DX5_CHECK_RESULT(hResult)
		}

		hResult = IDirectDrawSurface_Blt(screen_context->lpDDSPrimary, &context->rectClient, NULL, NULL, /*DDBLT_WAIT*/DDBLT_DONOTWAIT | DDBLT_ASYNC | DDBLT_COLORFILL, &ddBltFx);
		DX5_CHECK_RESULT(hResult)
	}
}

static void dx5_master_flip(dg_surface_p self, int x, int y, dg_rect_p rect)
{
	if (self && self->context)
	{
		dx5_video_p context=self->context;
		dx5_surface_p screen_context=(dx5_surface_p)context->screen->context;

		if (context->options & DG_OPT_DOUBLE_BUFFERING)
		{
			HRESULT hResult;
			RECT srcRect;
			RECT dstRect;
			LPRECT pSrcRect=0;
			LPRECT pDstRect=0;

			if (!(context->options & DG_OPT_FULLSCREEN))
			{
				dstRect=context->rectClient;
				pDstRect=&dstRect;
			}

			if (rect)
			{
				SetRect(&srcRect, rect->x, rect->y, rect->x+rect->w, rect->y+rect->h);
				pSrcRect=&srcRect;

				if (pDstRect)
				{
					pDstRect->left+=rect->x;
					pDstRect->top+=rect->y;
					pDstRect->right=pDstRect->left+rect->w;
					pDstRect->bottom=pDstRect->top+rect->h;
				}
				else
				{
					SetRect(&dstRect, rect->x, rect->y, rect->x+rect->w, rect->y+rect->h);
					pDstRect=&dstRect;
				}
			}

			hResult = IDirectDrawSurface_Blt(screen_context->lpDDSPrimary,
				pDstRect, screen_context->lpDDSBack, pSrcRect, /*DDBLT_WAIT*/ DDBLT_DONOTWAIT | DDBLT_ASYNC, NULL);

			DX5_CHECK_RESULT(hResult)
		}
	}
}

static int dx5_master_blit(dg_surface_p self, dg_rect_p dstRect, dg_surface_p sSurface, dg_rect_p srcRect, unsigned char alpha, unsigned char* alphaChannel, dg_color_t colorKey, int options)
{
	if (self && self->context)
	{
		dx5_video_p context=(dx5_video_p)self->device->context;
		dg_rect_p dstRect2=0;
		int result;

		if ( !(context->options & DG_OPT_DOUBLE_BUFFERING) )
		{
			/* add offset according the current left/top window position */
			dstRect2=dg_rect_create(context->rectClient.left+dstRect->x, context->rectClient.top+dstRect->y, dstRect->w, dstRect->h);
			dstRect=dstRect2;
		}

		result = dx5_surface_blit(context->screen, dstRect, sSurface, srcRect, alpha, alphaChannel, colorKey, options);

		if (dstRect2)
		{
			dstRect2->destroy(dstRect2);
		}
		return result;
	}
	return 0;
}

static void dx5_master_resize(dg_surface_p self, int width, int height)
{
	if ( ((self->device->options & (DG_OPT_DOUBLE_BUFFERING | DG_OPT_FULLSCREEN)) == DG_OPT_DOUBLE_BUFFERING) && self->context)
	{
		dx5_video_p context=(dx5_video_p)self->device->context;
		dx5_surface_p screen_context=(dx5_surface_p)context->screen->context;
		LPDIRECTDRAWSURFACE lpOldDDSurface=screen_context->lpDDSBack;

		/* create new direct draw surface */
		if (SUCCEEDED(dx5_create_physical_surface(context->lpDD, width, height, &(screen_context->lpDDSBack), &screen_context->ddsd, 0)))
		{
			/* release the old surface */
			IDirectDrawSurface_Release(lpOldDDSurface);
		} 
		else
		{
			/* Failed to create the new surface, restore the old */
			screen_context->lpDDSBack=lpOldDDSurface;
		}
	}
	dg_surface_resize(self, width, height);
}

static void dx5_master_fill_rect(dg_surface_p self, int x, int y, int width, int height, dg_color_t color)
{
	dx5_video_p context=(dx5_video_p)self->device->context;
	if (context)
	{
		if ( !(context->options & DG_OPT_DOUBLE_BUFFERING) )
		{
			x+=context->rectClient.left;
			y+=context->rectClient.top;
		}

		dx5_surface_fill_rect(context->screen, x, y, width, height, color);
	}
}

static void dx5_master_clear(dg_surface_p self, dg_color_t color)
{
	dx5_master_fill_rect(self, 0, 0, self->width, self->height, color);
}

static void dx5_no_render(dg_widget_p self)
{
}

static void dx5_destroy(dg_video_p self)
{
	if (self)
	{
		dx5_video_p context=self->context;
		if (context)
		{
			dx5_surface_p screen_context=(dx5_surface_p)context->screen->context;

			if (self->background_image)
				self->background_image->destroy(self->background_image);

			dx5_unset_mode(self);

			/* destroy screen */
			if (screen_context->lpDDSPrimary)
			{
				IDirectDrawSurface_Release(screen_context->lpDDSPrimary);
			} 
			
			if ((self->options & DG_OPT_DOUBLE_BUFFERING) && !(self->options & DG_OPT_FULLSCREEN) && screen_context->lpDDSBack)
			{
	            IDirectDrawSurface_Release(screen_context->lpDDSBack);
			}

			/* Destroy master surface */
			dg_widget_destroy(self->master_widget);

			/* Release direct draw */
			IDirectDraw_Release(context->lpDD);

			/* destroy screen context */
			free(context->screen->context);
			dg_surface_destroy(context->screen);

			/* destroy context */
			free(context);
		}
		/* destroy device */
		dg_video_destroy(self);
	}
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	dg_video_p device;
	dg_event_t event;

	switch( msg )
    {
    case WM_SIZE:
    case WM_MOVE:
        {
			device = (dg_video_p)GetWindowLong(hWnd, GWL_USERDATA);
			if (device && device->active && (!(device->options & DG_OPT_FULLSCREEN)) && device->context)
			{
				dx5_video_p context=(dx5_video_p)device->context;
				dx5_update_client_rect(device);
				if (msg == WM_SIZE && context->initialized)
				{
					device->xres = context->rectClient.right-context->rectClient.left;
					device->yres = context->rectClient.bottom-context->rectClient.top;
					device->master_widget->set_rect(device->master_widget, 0, 0, device->xres, device->yres, 1);
				}
			}
			break;
        }
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			device = (dg_video_p)GetWindowLong(hWnd, GWL_USERDATA);
			if (device)
			{
				dg_rect_p rect=dg_rect_create(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right-ps.rcPaint.left, ps.rcPaint.bottom-ps.rcPaint.top);
				if (device->options & DG_OPT_DOUBLE_BUFFERING)
				{
					device->flip(device, rect);
				}
				else
				{
					device->master_widget->invalidate(device->master_widget, rect);
				}
				rect->destroy(rect);
			}
			EndPaint(hWnd, &ps);
			/* avoid default windows painter */
			ValidateRect(hWnd, &(ps.rcPaint));
			return 1;
		}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
        {
			device = (dg_video_p)GetWindowLong(hWnd, GWL_USERDATA);
			if (device && device->active)
			{
				event.type = EVT_TYPE_MOUSE;
				event.e.mouse.x = LOWORD(lParam);
				event.e.mouse.y = HIWORD(lParam);
				switch (msg)
				{
					case WM_LBUTTONDOWN: 
						event.e.mouse.mouse_event = EVT_MOUSE_LBUTTON | EVT_PRESSED; 
					break;
					case WM_LBUTTONUP: 
						event.e.mouse.mouse_event = EVT_MOUSE_LBUTTON | EVT_RELEASED; 
					break;
					case WM_RBUTTONDOWN: 
						event.e.mouse.mouse_event = EVT_MOUSE_RBUTTON | EVT_PRESSED; 
					break;
					case WM_RBUTTONUP: 
						event.e.mouse.mouse_event = EVT_MOUSE_RBUTTON | EVT_RELEASED; 
					break;
					case WM_MBUTTONDOWN: 
						event.e.mouse.mouse_event = EVT_MOUSE_MBUTTON | EVT_PRESSED; 
					break;
					case WM_MBUTTONUP: 
						event.e.mouse.mouse_event = EVT_MOUSE_MBUTTON | EVT_RELEASED; 
					break;
					case WM_MOUSEWHEEL:
					{
						POINT wpt={0};
						POINT pt;
						GetCursorPos(&pt);
						ClientToScreen(hWnd, &wpt);
						event.e.mouse.x = pt.x - wpt.x;
						event.e.mouse.y = pt.y - wpt.y;
						event.e.mouse.mouse_event = EVT_MOUSE_WHEEL; 
						if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
							event.e.mouse.mouse_event|=EVT_RELEASED; /* Wheel Up */
						else
							event.e.mouse.mouse_event|=EVT_PRESSED;  /* Wheel Down */
					}
					break;
				}
				DGWidgetTriggerEvent(device->master_widget, &event);
			}
            break;
        }
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
        {
			device = (dg_video_p)GetWindowLong(hWnd, GWL_USERDATA);
			if (device && device->active)
			{
				event.type = EVT_TYPE_MOUSE;
				event.e.mouse.x = LOWORD(lParam);
				event.e.mouse.y = HIWORD(lParam);

				switch (msg)
				{
					case WM_LBUTTONDBLCLK: 
						event.e.mouse.mouse_event = EVT_MOUSE_LBUTTON; 
					break;
					case WM_RBUTTONDBLCLK: 
						event.e.mouse.mouse_event = EVT_MOUSE_RBUTTON; 
					break;
					case WM_MBUTTONDBLCLK: 
						event.e.mouse.mouse_event = EVT_MOUSE_MBUTTON; 
					break;
				}
				event.e.mouse.mouse_event |= EVT_MOUSE_DBL | EVT_PRESSED;
				DGWidgetTriggerEvent(device->master_widget, &event);
			}
            break;
        }
    case WM_MOUSEMOVE:
		{
			device = (dg_video_p)GetWindowLong(hWnd, GWL_USERDATA);
			if (device && device->active)
			{
				event.type = EVT_TYPE_MOUSE;
				event.e.mouse.x = LOWORD(lParam);
				event.e.mouse.y = HIWORD(lParam);
				event.e.mouse.mouse_event = 0;
				DGWidgetTriggerEvent(device->master_widget, &event);
			}
			break;
		}
    case WM_ACTIVATEAPP:
        {
			device = (dg_video_p)GetWindowLong(hWnd, GWL_USERDATA);
			if (device)
			{
				device->active=(int)wParam;
				if (device->active)
				{
					HRESULT hResult;
					dx5_video_p context=(dx5_video_p)device->context;
					if (context && context->screen && context->screen->context)
					{
						dx5_surface_p screen_context = (dx5_surface_p)context->screen->context;
						if (screen_context->lpDDSPrimary)
						{
							if (IDirectDrawSurface_IsLost(screen_context->lpDDSPrimary)==DDERR_SURFACELOST)
							{
								hResult = IDirectDrawSurface_Restore(screen_context->lpDDSPrimary);
								if (FAILED(hResult))
								{
									DX5_CHECK_RESULT(hResult)
									/* FIXME: Handle surface lost problem */
								}
							}
						}
					}
				}
            }
        }
        break;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );
}

static void dx5_video_set_mouse_capture(dg_video_p self, int is_capture)
{
	if (self && self->context)
	{
		if (is_capture)
		{
			SetCapture(((dx5_video_p)self->context)->hwndMain);
		}
		else
		{
			ReleaseCapture();
		}
	}
}

//static void dx5_video_set_background_image(dg_video_p self, dg_surface_p background_image)
//{
//	dg_video_set_background_image(self, background_image);
//
//}
//
//static void dx5_video_set_background_color(dg_video_p self, dg_color_t background_color)
//{
//	dg_video_set_background_color(self, background_color);
//}
//

static void dx5_video_init_master_widget(dg_video_p device)
{
	device->master_widget->surface = dg_surface_create(device, device->xres, device->yres, 0);
	device->master_widget->surface->context=device->context;
	device->master_widget->surface->flip=dx5_master_flip;
	device->master_widget->surface->blit=dx5_master_blit;
	device->master_widget->surface->resize=dx5_master_resize;
	device->master_widget->surface->clear=dx5_master_clear;
	device->master_widget->surface->fill_rect=dx5_master_fill_rect;
	device->master_widget->surface->lock=dx5_surface_lock;
	device->master_widget->surface->unlock=dx5_surface_unlock;
	device->master_widget->render=dx5_no_render;
}

int dx5_video_create(struct dg_video_t* _device)
{
	HRESULT hResult;
	dg_video_p device=(dg_video_p)_device;
	dx5_video_p self=(dx5_video_p)malloc(sizeof(dx5_video_t));
	dx5_surface_p screen_context;
	memset(self, 0, sizeof(dx5_video_t));

	/* set directx5 context */
	device->context=(void*)self;

	/* initialize main DX5 screen */
	self->screen=dg_surface_create(device, device->xres, device->yres, 0);
	screen_context=self->screen->context=(dx5_surface_p)malloc(sizeof(dx5_surface_t));
	memset(screen_context, 0, sizeof(dx5_surface_t));
	screen_context->device=device;

	self->screen->lock=dx5_surface_lock;
	self->screen->unlock=dx5_surface_unlock;
	self->screen->fill_rect=dx5_surface_fill_rect;
	self->screen->set_color_key=dx5_surface_set_color_key;

	/* set is fullscreen to directx5 context */
	self->options = device->options;

	/* create direct draw object */
    hResult = DirectDrawCreate(NULL, &(self->lpDD), NULL);
	if (SUCCEEDED(hResult)) 
	{
		/* set video mode */
		if (!dx5_set_mode(device))
		{
			printf("Unable to set video mode!");
			return 0;
		}

		/* set video memory */
		dx5_clear_buffers(device);

		/* set directx 5 surface create method */
		device->create_surface = dx5_create_surface;

		/* set directx 5 event manager */
		device->master_widget->event_manager->destroy(device->master_widget->event_manager);
		device->master_widget->event_manager = dx5_create_event_manager(device);

		/* set directx 5 mouse capture */
		device->set_mouse_capture = dx5_video_set_mouse_capture;

		/* set video device destroyer */
		device->destroy = dx5_destroy;

		/* set video active */
		device->active = 1;

		/* init master widget */
		dx5_video_init_master_widget(device);

		/* set video device initialized */
		self->initialized = 1;
	}
	else
	{
		DX5_CHECK_RESULT(hResult)
		return 0;
	}
	return 1;
}

#pragma warning (pop)
