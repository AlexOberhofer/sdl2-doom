// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for SDL2, UNIX.
//
//-----------------------------------------------------------------------------

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <SDL2/SDL.h>

#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

//SDL Vars
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;
SDL_Event sdl_e;

int Width, Center_X;
int Height, Center_Y;

// Fake mouse handling.
// This cannot work properly w/o DGA.
// Needs an invisible mouse cursor at least.
boolean		grabMouse;

int 		lastMouse;

int SDL_GetKey(SDL_Keycode key) {

	//Map SDL keys to engine keys
	//SEE: Doomdef.h for engine values

	switch (key) {
		case SDLK_a: return KEY_LEFTARROW;
		case SDLK_d: return KEY_RIGHTARROW;
		case SDLK_s: return KEY_DOWNARROW;
		case SDLK_w: return KEY_UPARROW;
		case SDLK_ESCAPE: return KEY_ESCAPE;
		case SDLK_RETURN: return KEY_ENTER;
		case SDLK_TAB: return KEY_TAB;
		case SDLK_F1: return KEY_F1;
		case SDLK_F2: return KEY_F2;
		case SDLK_F3: return KEY_F3;
		case SDLK_F4: return KEY_F4;
		case SDLK_F5: return KEY_F5;
		case SDLK_F6: return KEY_F6;
		case SDLK_F7: return KEY_F7;
		case SDLK_F8: return KEY_F8;
		case SDLK_F9: return KEY_F9;
		case SDLK_F10: return KEY_F10;
		case SDLK_F11: return KEY_F11;
		case SDLK_F12: return KEY_F12;

		case SDLK_BACKSPACE:
		case SDLK_DELETE: return KEY_BACKSPACE;

		case SDLK_PAUSE: return KEY_PAUSE;

		case SDLK_KP_EQUALS:
		case SDLK_EQUALS: return KEY_EQUALS;

		case SDLK_KP_MINUS:
		case SDLK_MINUS: return KEY_MINUS;

		case SDLK_LSHIFT:
		case SDLK_RSHIFT: return KEY_RSHIFT;

		case SDLK_LCTRL:
		case SDLK_RCTRL: return KEY_RCTRL;

		case SDLK_LALT:
		case SDLK_RALT: return KEY_RALT;
		default: return key;
	}
}

void SDL_ManageEvents(){
	event_t event;
	uint8_t button;

	while (SDL_PollEvent(&sdl_e)) {

		switch (sdl_e.type) {

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				event.type = (sdl_e.type == SDL_KEYDOWN) ? ev_keydown : ev_keyup;
				event.data1 = SDL_GetKey(sdl_e.key.keysym.sym);
				D_PostEvent(&event);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				event.type = ev_mouse;
				event.data1 = lastMouse;

				button = sdl_e.button.button;

				if (sdl_e.type == SDL_MOUSEBUTTONDOWN) {
					event.data1 |= button == SDL_BUTTON_LEFT;
					event.data1 |= button == SDL_BUTTON_MIDDLE ? 2 : 0;
					event.data1 |= button == SDL_BUTTON_RIGHT ? 4 : 0;
				} else {
					event.data1 ^= button == SDL_BUTTON_LEFT;
					event.data1 ^= button == SDL_BUTTON_MIDDLE ? 2 : 0;
					event.data1 ^= button == SDL_BUTTON_RIGHT ? 4 : 0;
				}

				event.data2 = event.data3 = 0;

				D_PostEvent(&event);

				lastMouse = event.data1;
				break;
			case SDL_MOUSEMOTION:
				if (!grabMouse) {
					break; //X11 acts this way
				}

				event.type = ev_mouse;

				event.data1 = 0;
				event.data2 = sdl_e.motion.xrel << 2;
				event.data3 = -(sdl_e.motion.yrel << 2);

				D_PostEvent(&event);
				break;
			case SDL_QUIT:
				I_Quit();
				break;
		}
	}
}

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=1;

void I_ShutdownGraphics(void)
{
  	if(surface){
		SDL_FreeSurface(surface);
	}
	if(renderer){
		SDL_DestroyRenderer(renderer);
	}
	if(window){
		SDL_DestroyWindow(window);
	}
}



//
// I_StartFrame
//
void I_StartFrame (void)
{
    // er?

}

void I_GetEvent(void)
{

}

//
// I_StartTic
//
void I_StartTic (void)
{
	//EVENT HANDLE HERE
	SDL_ManageEvents();
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//
void I_FinishUpdate (void)
{

    static int	lasttic;
    int		tics;
    int		i;
    // UNUSED static unsigned char *bigscreen=0;

	SDL_Texture *texture;

    // draws little dots on the bottom of the screen
    if (devparm)
    {

	i = I_GetTime();
	tics = i - lasttic;
	lasttic = i;

	if (tics > 20){ tics = 20; }

	for (i=0 ; i<tics*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0xff;
	for ( ; i<20*2 ; i+=2)
	    screens[0][ (SCREENHEIGHT-1)*SCREENWIDTH + i] = 0x0;
    
    }

    // scales the screen size before blitting it
    if (multiply == 2)
    {
		I_Error("Err: Switch not implemented: 2\n");
    }
    else if (multiply == 3)
    {
		I_Error("Err: Switch not implemented: 3\n");
    }
    else if (multiply == 4)
    {
		I_Error("Err: Switch not implemented: 4\n");
    }

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}

void UploadNewPalette()
{

}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
    int x;
	int y;
	SDL_Palette *s_palette;

	s_palette = surface->format->palette;

	for(x = 0; x< s_palette->ncolors; x++){

		y = gammatable[usegamma][*palette++];
		s_palette->colors[x].r = (y << 8) + y;

		y = gammatable[usegamma][*palette++];
		s_palette->colors[x].g = (y << 8) + y;

		y = gammatable[usegamma][*palette++];
		s_palette->colors[x].b = (y << 8) + y;
	}
}

void I_InitGraphics(void)
{
    int			n;
    int			pnum;
    int			x=0;
    int			y=0;
    
    // warning: char format, different type arg
    char		xsign=' ';
    char		ysign=' ';
    
	uint32_t windowFlags = SDL_WINDOW_SHOWN;

    if(window != NULL){
		return;
	}

    signal(SIGINT, (void (*)(int)) I_Quit);

    if (M_CheckParm("-2")){ multiply = 2; }	

    if (M_CheckParm("-3")) { multiply = 3; }

    if (M_CheckParm("-4")) { multiply = 4; }

    Width = SCREENWIDTH * multiply;
    Height = SCREENHEIGHT * multiply;

	Center_X = Width / 2;
	Center_Y= Height / 2;

    // check for command-line display name
	/* 
    if ( (pnum=M_CheckParm("-disp")) ) // suggest parentheses around assignment
	displayname = myargv[pnum+1];
    else
	displayname = 0;*/

    // check if the user wants to grab the mouse (quite unnice)
    grabMouse = !!M_CheckParm("-grabmouse");

    // check for command-line geometry
	/* 
    if ( (pnum=M_CheckParm("-geom")) ) // suggest parentheses around assignment
    {
	// warning: char format, different type arg 3,5
	n = sscanf(myargv[pnum+1], "%c%d%c%d", &xsign, &x, &ysign, &y);
	
	if (n==2)
	    x = y = 0;
	else if (n==6)
	{
	    if (xsign == '-')
		x = -x;
	    if (ysign == '-')
		y = -y;
	}
	else
	    I_Error("bad -geom parameter");
    } */ //Add this to the list: things I dont understand

	if(grabMouse)
	{
		SDL_SetRelativeMouseMode(true);
	} else 
	{
		SDL_ShowCursor(false);
	}

	window = SDL_CreateWindow("DOOM", x, y, Width, Height, windowFlags);
	if(window == NULL){
		I_Error(SDL_GetError());
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if(renderer == NULL){
		I_Error(SDL_GetError());
	}

	surface = SDL_CreateRGBSurface(0, Width, Height, 8, 0, 0, 0, 0);

    if (multiply == 1)
	screens[0] = (unsigned char *) (surface->pixels);
    else
	screens[0] = (unsigned char *) malloc (SCREENWIDTH * SCREENHEIGHT);

}


unsigned	exptable[256];

void InitExpand (void)
{
    int		i;
	
    for (i=0 ; i<256 ; i++)
	exptable[i] = i | (i<<8) | (i<<16) | (i<<24);
}

double		exptable2[256*256];

void InitExpand2 (void)
{
    int		i;
    int		j;
    // UNUSED unsigned	iexp, jexp;
    double*	exp;
    union
    {
	double 		d;
	unsigned	u[2];
    } pixel;
	
    printf ("building exptable2...\n");
    exp = exptable2;
    for (i=0 ; i<256 ; i++)
    {
	pixel.u[0] = i | (i<<8) | (i<<16) | (i<<24);
	for (j=0 ; j<256 ; j++)
	{
	    pixel.u[1] = j | (j<<8) | (j<<16) | (j<<24);
	    *exp++ = pixel.d;
	}
    }
    printf ("done.\n");
}

int	inited;

void
Expand4
( unsigned*	lineptr,
  double*	xline )
{
    double	dpixel;
    unsigned	x;
    unsigned 	y;
    unsigned	fourpixels;
    unsigned	step;
    double*	exp;
	
    exp = exptable2;
    if (!inited)
    {
	inited = 1;
	InitExpand2 ();
    }
		
		
    step = 3*SCREENWIDTH/2;
	
    y = SCREENHEIGHT-1;
    do
    {
	x = SCREENWIDTH;

	do
	{
	    fourpixels = lineptr[0];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[0] = dpixel;
	    xline[160] = dpixel;
	    xline[320] = dpixel;
	    xline[480] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[1] = dpixel;
	    xline[161] = dpixel;
	    xline[321] = dpixel;
	    xline[481] = dpixel;

	    fourpixels = lineptr[1];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[2] = dpixel;
	    xline[162] = dpixel;
	    xline[322] = dpixel;
	    xline[482] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[3] = dpixel;
	    xline[163] = dpixel;
	    xline[323] = dpixel;
	    xline[483] = dpixel;

	    fourpixels = lineptr[2];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[4] = dpixel;
	    xline[164] = dpixel;
	    xline[324] = dpixel;
	    xline[484] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[5] = dpixel;
	    xline[165] = dpixel;
	    xline[325] = dpixel;
	    xline[485] = dpixel;

	    fourpixels = lineptr[3];
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff0000)>>13) );
	    xline[6] = dpixel;
	    xline[166] = dpixel;
	    xline[326] = dpixel;
	    xline[486] = dpixel;
			
	    dpixel = *(double *)( (int)exp + ( (fourpixels&0xffff)<<3 ) );
	    xline[7] = dpixel;
	    xline[167] = dpixel;
	    xline[327] = dpixel;
	    xline[487] = dpixel;

	    lineptr+=4;
	    xline+=8;
	} while (x-=16);
	xline += step;
    } while (y--);
}


