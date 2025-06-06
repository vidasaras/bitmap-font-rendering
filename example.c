#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <string.h>
#define BFR_IMPLEMENTATION
#include "bfr.h"

struct Font_config font_config = {
  .ch_w = 40,
  .ch_h = 72,
  .margin_x = 48,
  .margin_y = 48,
  .ch_sp = 8,
  .line_sp = 8,
  .ch_line = 13,
  .nm_line = 7,
  .nm_ch = 88
};

int main(){
  if (SDL_Init(SDL_INIT_VIDEO) < 0){
    return 1;
  }

  SDL_Window *win =  SDL_CreateWindow("Bitmap Font Renderer",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800,
      600, 0);

  SDL_Renderer *renderer =  SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  if (bfr_init(font_config) != 0) printf("Bitmap font init failed!\n");

  SDL_Texture* font_texture = NULL;
  load_font_image(renderer, "font.png", &font_texture);

  load_ascii("input2", 1);

  SDL_Rect rect = {100, 100, 40*12+11*3, 300};
  SDL_SetRenderDrawColor(renderer, 250, 0, 0, 250);

  SDL_RenderDrawRect(renderer, &rect);

  Font_Render_Params params  = {
    100, 100, //dst
    10, 10,   // margin
    3, 8,     // padding of characters and lines
    1, 1,     // scale in w and h
    12         // wrap length
  };

  //render_line(renderer, font_texture, "Hello, World!\n", 0, &params);
  char ch[] = "Is this thing even working?\n Who knows.";
  render_lines_wrapped(renderer, font_texture, ch, &params);

  SDL_RenderPresent(renderer);
  sleep(10);

  bfr_destroy();
  SDL_DestroyTexture(font_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
