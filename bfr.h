#ifndef BFR_H
#define BFR_H

#include <stdlib.h>
#include <string.h>

struct Font_config {
  int ch_w,
      ch_h,
      margin_x,
      margin_y,
      ch_sp,
      line_sp,
      ch_line,
      nm_line,
      nm_ch,
      scale;
};

int bfr_init(struct Font_config conf);
// Initializes the font system with the given configuration.
void bfr_destroy();
// Frees memory used by the font system.
int load_font_image(SDL_Renderer* renderer, char* file, SDL_Texture** font_texture);
// Loads the font image
void load_ascii(char* file, int syntaxValid);
// Loads ASCII codes from a file into memory.
void render_ch(SDL_Renderer* renderer, SDL_Texture* font_texture, char ch, int ch_in_line_i, int line_i);
// Renders a single character at the specified grid position.
void render_line(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch, int line_i);
// Renders a line of text at the given line index.
void render_lines(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch);
// Renders multiple lines of text separated by '\n'.

#ifdef BFR_IMPLEMENTATION

int* ascii = NULL;
struct Font_config* BFRfont_config = NULL;

int bfr_init(struct Font_config conf){

  BFRfont_config = malloc(sizeof(struct Font_config));
  *BFRfont_config = conf;

 ascii = malloc(sizeof(int) * BFRfont_config->nm_ch);

  if (!BFRfont_config || !ascii) return 1;
  return 0;
}

void bfr_destroy(){
  free(ascii);
  free(BFRfont_config);
}

int load_font_image(SDL_Renderer* renderer, char* file, SDL_Texture** font_texture){
  *font_texture =  IMG_LoadTexture(renderer, file);
  if (*font_texture == NULL) return 1;

  SDL_SetTextureBlendMode(*font_texture, SDL_BLENDMODE_BLEND);
  return 0;
}

void parse_ascii_file(char* file){
  FILE* file_ptr;
  file_ptr = fopen(file, "r");

  char line[20];
  int jump_ascii_by = 1;
  int prev_jump_ascii_by = 1;
  int curr_ascii = 0;
  int prev_ascii = 0;
  int curr_i = 0;
  int jump_i = 0;
  while(fgets(line, 20, file_ptr) != NULL){
    char* token = strtok(line, ":"); // the index start points
    jump_i = atoi(token);
    token = strtok(NULL, ":"); // the ascii values
    if(strchr(token, 'x') != NULL){
      char* ascii_ = strtok(token, "x");
      prev_ascii = curr_ascii;
      curr_ascii = atoi(ascii_);
      prev_jump_ascii_by = jump_ascii_by;
      jump_ascii_by = atoi(strtok(NULL, "x"));
    } else {
      prev_ascii = curr_ascii;
      curr_ascii = atoi(token);
      prev_jump_ascii_by = jump_ascii_by;
      jump_ascii_by = 1;
    }
    if (curr_i < jump_i-1){
      int gap = jump_i - curr_i;
      for (int i = 0; i < gap; i++){
        ascii[curr_i - 1] = prev_ascii + prev_jump_ascii_by*i;
        curr_i++;
      }
      continue;
    }
    ascii[curr_i] = curr_ascii;
    curr_i++;
  }

  fclose(file_ptr);
}

void load_ascii(char* file, int syntaxValid){
  if (syntaxValid == 1){
    parse_ascii_file(file);
    return;
  }
  FILE* file_ptr;
  file_ptr = fopen(file, "r");

  char line[20];
  int i = 0;
  while (fgets(line, 20, file_ptr) != NULL) {
    if (i >= BFRfont_config->nm_ch) break;
    ascii[i] = atoi(line);
    i++;
  }

  fclose(file_ptr);
}

void render_ch(SDL_Renderer* renderer, SDL_Texture* font_texture, char ch, int ch_in_line_i, int line_i){
  int i = 0;
  while (i < BFRfont_config->nm_ch){
    if (ch == ascii[i]) break;
    i++;
  }
  if (i == BFRfont_config->nm_ch) return;

  int line_num = i / BFRfont_config->ch_line;
  int ch_in_line_num = i % BFRfont_config->ch_line;

  int x = BFRfont_config->margin_x + ch_in_line_num * BFRfont_config->ch_sp + ch_in_line_num * BFRfont_config->ch_w;
  int y = BFRfont_config->margin_y + line_num * BFRfont_config->line_sp + line_num * BFRfont_config->ch_h;

  SDL_Rect r_src = {x, y, BFRfont_config->ch_w, BFRfont_config->ch_h};
  SDL_Rect r_dst = {
    BFRfont_config->ch_w * ch_in_line_i + ch_in_line_i * 3,
    BFRfont_config->ch_h * line_i + 8 * line_i,
    BFRfont_config->ch_w * BFRfont_config->scale,
    BFRfont_config->ch_h * BFRfont_config->scale
  };

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

#endif

#endif
