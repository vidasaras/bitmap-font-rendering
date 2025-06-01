#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
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
  .nm_ch = 88,
  .scale = 1
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

  render_lines(renderer, font_texture, "Hello, World!");

  SDL_RenderPresent(renderer);
  sleep(3);

  bfr_destroy();
  SDL_DestroyTexture(font_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
