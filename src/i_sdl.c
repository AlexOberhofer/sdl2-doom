
#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

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

static void handleKeyInput() 
{
  SDL_Event e;

  while (SDL_PollEvent(&e))
  {
    if (e.type == SDL_QUIT)
    {
      atexit(SDL_Quit);
      exit(1);
    }

    if (e.type == SDL_KEYDOWN) 
    {
      //printf("KeyPress:%d sym:%d\n", e.xkey.keycode, sym);
      queueKeyPress(1, e.key.keysym.sym);
    } 
    else if (e.type == SDL_KEYUP) 
    {
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

void DG_SleepMs(uint32_t ms)
{
    SDL_Delay(ms);
}

uint32_t DG_GetTicksMs()
{
    return SDL_GetTicks();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
    handleKeyInput();
    
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