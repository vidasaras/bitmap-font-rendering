#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <stdlib.h>

const int ch_w = 40,
    ch_h = 72,
    margin_x = 48,
    margin_y = 48,
    ch_sp = 8,
    line_sp = 8,
    ch_line = 13,
    nm_line = 7;

int scale = 1;

#define NM_CH 88

int ascii[NM_CH];

void load_ascii(char* file){
  FILE* file_ptr;
  file_ptr = fopen(file, "r");

  char line[20];
  int i = 0;
  while (fgets(line, 20, file_ptr) != NULL) {
    ascii[i] = atoi(line);
    i++;
  }

  fclose(file_ptr);
}

void render_ch(SDL_Renderer* renderer, SDL_Texture* font_texture, char ch, int ch_in_line_i, int line_i){
  int i = 0;
  while (i < NM_CH){
    if (ch == ascii[i]) break;
    i++;
  }

  int line_num = i / ch_line;
  int ch_in_line_num = i % ch_line;


  int x = margin_x + ch_in_line_num*ch_sp + ch_in_line_num*ch_w;
  int y = margin_y + line_num*line_sp + line_num*ch_h;

  SDL_Rect r_src = {x, y, ch_w, ch_h};
  SDL_Rect r_dst = {ch_w*ch_in_line_i + ch_in_line_i*3, ch_h*line_i + 8*line_i, ch_w*scale, ch_h*scale};
  //A: <char id="65" x="0" y="0" width="6" height="10" page="0" xadvance="6" xoffset="0" yoffset="0"/>

  SDL_RenderCopy(renderer, font_texture, &r_src, &r_dst);
}

void render_line(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch, int line_i){
  int i = 0;
  while (ch[i] != '\0'){
    render_ch(renderer, font_texture, ch[i], (int)i+1, line_i);
    i++;
  }
}

void render_lines(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch){
  int i = 0;
  int ch_i = 1;
  int line_i = 1;
  while (ch[i] != '\0'){
    if (ch[i] == '\n'){
      line_i++;
      ch_i = 1;
      i++;
      continue;
    }
    render_ch(renderer, font_texture, ch[i], ch_i, line_i);
    ch_i++;
    i++;
  }
}

int main(){
  if (SDL_Init(SDL_INIT_VIDEO) < 0){
    return 1;
  }

  SDL_Window *win =  SDL_CreateWindow("Bitmap Font Renderer",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800,
      600, 0);

  SDL_Renderer *renderer =  SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  SDL_Texture *font_texture =  IMG_LoadTexture(renderer, "font.png");
  if (font_texture == NULL) return 1;

  SDL_SetTextureBlendMode(font_texture, SDL_BLENDMODE_BLEND);

  load_ascii("input");

  render_lines(renderer, font_texture, "Hello, World!\n& Good, Bye();");

  SDL_RenderPresent(renderer);
  sleep(3);

  SDL_DestroyTexture(font_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
