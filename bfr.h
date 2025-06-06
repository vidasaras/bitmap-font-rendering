#ifndef BFR_H
#define BFR_H

#include <stdlib.h>
#include <string.h>

typedef struct Font_config {
  int ch_w,
      ch_h,
      margin_x,
      margin_y,
      ch_sp,
      line_sp,
      ch_line,
      nm_line,
      nm_ch;
} Font_config;

typedef struct Font_Render_Params {
  int dst_x, dst_y,
      margin_x, margin_y,
      text_padding_ch, text_padding_ln,
      scale_w, scale_h,
      line_width;
} Font_Render_Params;

int bfr_init(struct Font_config conf);
// Initializes the font system with the given configuration.
void bfr_destroy();
// Frees memory used by the font system.
int load_font_image(SDL_Renderer* renderer, char* file, SDL_Texture** font_texture);
// Loads the font image
void load_ascii(char* file, int syntaxValid);
// Loads ASCII codes from a file into memory.
void render_ch(SDL_Renderer* renderer, SDL_Texture* font_texture, char ch, int ch_in_line_i, int line_i, Font_Render_Params* params);
// Renders a single character at the specified grid position.
void render_line(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch, int line_i, Font_Render_Params* params);
// Renders a line of text at the given line index.
void render_lines(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch, Font_Render_Params* params);
// Renders multiple lines of text separated by '\n'.
void render_lines_wrapped(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch, Font_Render_Params* params);

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

void render_ch(SDL_Renderer* renderer, SDL_Texture* font_texture, char ch, int ch_in_line_i, int line_i, Font_Render_Params* params){
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
    params->dst_x + params->margin_x + BFRfont_config->ch_w * ch_in_line_i + ch_in_line_i * params->text_padding_ch,
    params->dst_y + params->margin_y + BFRfont_config->ch_h * line_i + params->text_padding_ln * line_i,
    BFRfont_config->ch_w * params->scale_w,
    BFRfont_config->ch_h * params->scale_h
  };

  SDL_RenderCopy(renderer, font_texture, &r_src, &r_dst);
}

void render_line(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch, int line_i, Font_Render_Params* params){
  int i = 0;
  while (ch[i] != '\0'){
    render_ch(renderer, font_texture, ch[i], (int)i, line_i, params);
    i++;
  }
}

void render_lines(SDL_Renderer* renderer, SDL_Texture* font_texture, char* ch, Font_Render_Params* params){
  int i = 0;
  int ch_i = 0;
  int line_i = 0;
  while (ch[i] != '\0'){
    if (ch[i] == '\n'){
      line_i++;
      ch_i = 1;
      i++;
      continue;
    }
    render_ch(renderer, font_texture, ch[i], ch_i, line_i, params);
    ch_i++;
    i++;
  }
}

void render_lines_wrapped(SDL_Renderer* renderer, SDL_Texture* font_texture, char ch[], Font_Render_Params* params){
  char* token = strtok(ch, " ");
  int curr_ln_w = 0;
  int max_ln_w = BFRfont_config->ch_w*params->scale_w*params->line_width + params->text_padding_ch*(params->line_width-1);
  int line_i = 0;
  int doContainN = 0;
  int dst_x = params->dst_x;
  while (token != NULL){
    int token_l = strlen(token);
    int token_w = BFRfont_config->ch_w*params->scale_w*token_l + (params->text_padding_ch*(token_l-1));
    int loc_doContainN = 0;
    if(strchr(token, '\n') != NULL){
      token_l -= 1;
      loc_doContainN = 1;
      token_w -= BFRfont_config->ch_w + params->text_padding_ch;
    }
    params->dst_x += curr_ln_w;
    if ((max_ln_w - curr_ln_w) < token_w || max_ln_w <= curr_ln_w || doContainN == 1){
      curr_ln_w = 0;
      params->dst_x = dst_x;
      line_i++;
    }
    render_line(renderer, font_texture, token, line_i, params);
    curr_ln_w += token_w + BFRfont_config->ch_w*params->scale_w + params->text_padding_ch;
    doContainN = loc_doContainN;
    token = strtok(NULL, " ");
  }
}

#endif

#endif
