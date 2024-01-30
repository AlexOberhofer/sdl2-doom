//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
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


#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "config.h"
#include "deh_str.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_swap.h"
#include "i_timer.h"
#include "i_video.h"
#include "i_scale.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "tables.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;


//TODO: Convert to key-map
static unsigned char toDoomKey(unsigned int key)
{
  switch (key)
  {
    case SDLK_RETURN:
      key = KEY_ENTER;
      break;
    case SDLK_F1:
      key = KEY_F1;
      break;
    case SDLK_F2:
      key = KEY_F2;
      break;
    case SDLK_F3:
      key = KEY_F3;
      break;
    case SDLK_LALT:
    case SDLK_RALT:
      key = KEY_LALT;
      break;
    case SDLK_ESCAPE:
      key = KEY_ESCAPE;
      break;
    case SDLK_a:
    case SDLK_LEFT:
      key = KEY_LEFTARROW;
      break;
    case SDLK_d:
    case SDLK_RIGHT:
      key = KEY_RIGHTARROW;
      break;
    case SDLK_w:
    case SDLK_UP:
      key = KEY_UPARROW;
      break;
    case SDLK_s:
    case SDLK_DOWN:
      key = KEY_DOWNARROW;
      break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
      key = KEY_FIRE;
      break;
    case SDLK_SPACE:
      key = KEY_USE;
      break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
      key = KEY_RSHIFT;
      break;
    case SDL_BUTTON_RIGHT:
    case SDL_BUTTON_LEFT:
      key = KEY_FIRE;
      break;
    case SDL_BUTTON_MIDDLE:
      key = KEY_USE;
      break;
    default:
      key = tolower(key);
      break;
    }

  return key;
}

static void queueKeyPress(int pressed, unsigned int keyCode)
{
  unsigned char key = toDoomKey(keyCode);
  unsigned short keyData = (pressed << 8) | key;

  s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
  s_KeyQueueWriteIndex++;
  s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

static void SDL_PollEvents()
{
  SDL_Event e;

  while (SDL_PollEvent(&e))
  {
    if (e.type == SDL_QUIT)
    {
      atexit(SDL_Quit);
      exit(1);
    }

    if (e.type == SDL_WINDOWEVENT) {
      if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
        I_RescaleWindow(e.window.data1, e.window.data2);
      }
    }

    else if (e.type == SDL_KEYDOWN)
    {
      //printf("KeyPress:%d sym:%d\n", e.xkey.keycode, sym);
      queueKeyPress(1, e.key.keysym.sym);
    }
    else if (e.type == SDL_KEYUP)
    {
      /* Toggle fullscreen with `ALT+F` */
      if (e.key.keysym.sym == SDLK_f
       && e.key.keysym.mod & KMOD_ALT) {
         I_ToggleWindowFullscreenDesktop();
      }

      //printf("KeyRelease:%d sym:%d\n", e.xkey.keycode, sym);
      queueKeyPress(0, e.key.keysym.sym);
    }
    else if(e.type == SDL_MOUSEBUTTONDOWN)
    {
      //printf("SDL_MOUSE_PRESSED: %d\n", e.button.button);
      queueKeyPress(1, e.button.button);
    }
    else if(e.type == SDL_MOUSEBUTTONUP)
    {
      //printf("SDL_MOUSE_RELEASED: %d\n", e.button.button);
      queueKeyPress(0, e.button.button);
    }

  }

}

int GetKey(int* pressed, unsigned char* doomKey)
{
    SDL_PollEvents();

    uint8_t k_pressed = 0;

    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
    {
        k_pressed = 0;
    }
    else
    {
        unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
        s_KeyQueueReadIndex++;
        s_KeyQueueReadIndex %= KEYQUEUE_SIZE;
        *pressed = keyData >> 8;
        *doomKey = keyData & 0xFF;

        k_pressed = 1;
    }

    return k_pressed;
}

int vanilla_keyboard_mapping = 1;

// Is the shift key currently down?

static int shiftdown = 0;

// Lookup table for mapping ASCII characters to their equivalent when
// shift is pressed on an American layout keyboard:
static const char shiftxform[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(', ')', '*', '+',
    '<', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    ')', // shift-0
    '!', // shift-1
    '@', // shift-2
    '#', // shift-3
    '$', // shift-4
    '%', // shift-5
    '^', // shift-6
    '&', // shift-7
    '*', // shift-8
    '(', // shift-9
    ':',
    ':', // shift-;
    '<',
    '+', // shift-=
    '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"', '_',
    '\'', // shift-`
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '{', '|', '}', '~', 127
};

// Get the equivalent ASCII (Unicode?) character for a keypress.

static unsigned char GetTypedChar(unsigned char key)
{
    // Is shift held down?  If so, perform a translation.
    if (shiftdown > 0)
    {
        if (key >= 0 && key < arrlen(shiftxform))
        {
            key = shiftxform[key];
        }
        else
        {
            key = 0;
        }
    }

    return key;
}

static void UpdateShiftStatus(int pressed, unsigned char key)
{
    int change;

    if (pressed) {
        change = 1;
    } else {
        change = -1;
    }

    if (key == KEY_RSHIFT) {
        shiftdown += change;
    }
}


void I_GetEvent(void)
{
    event_t event;
    int pressed;
    unsigned char key;

	while (GetKey(&pressed, &key))
    {
        UpdateShiftStatus(pressed, key);

        // process event
        if(pressed)
        {
            // data1 has the key pressed, data2 has the character
            // (shift-translated, etc)
            event.type = ev_keydown;
            event.data1 = key;
            event.data2 = GetTypedChar(key);

            if (event.data1 != 0)
            {
                D_PostEvent(&event);
            }
        }
        else
        {
            event.type = ev_keyup;
            event.data1 = key;

            // data2 is just initialized to zero for ev_keyup.
            // For ev_keydown it's the shifted Unicode character
            // that was typed, but if something wants to detect
            // key releases it should do so based on data1
            // (key ID), not the printable char.

            event.data2 = 0;

            if (event.data1 != 0)
            {
                D_PostEvent(&event);
            }
            break;
        }
    }

}

void I_InitInput(void)
{
}
