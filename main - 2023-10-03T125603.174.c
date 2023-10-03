
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char *loadPng(const char *filename, int *w, int *h) {

  unsigned char *picture = NULL;
  int error = lodepng_decode32_file(&picture, w, h, filename);
  if (error) {
    printf("error %u: %s\n", error, lodepng_error_text(error));
  }

  return (picture);
}

void writePng(const char *inputfile, const unsigned char *picture, unsigned w,
              unsigned h) {
  unsigned char *png;
  long unsigned int pngsize;

  int error = lodepng_encode32(&png, &pngsize, picture, w, h);
  if (!error) {
    lodepng_save_file(png, pngsize, inputfile);
  }
  if (error)
    printf("errOOOOOOr %u: %s\n", error, lodepng_error_text(error));

  free(png);
}

void remove_colors(int w, int h, unsigned char *picture,
                   unsigned char *picture1) {
  int i;
  for (i = 0; i < w * h; i++) {
    picture1[i] = 0.3 * picture[4 * i] + 0.6 * picture[4 * i + 1] +
                  0.130 * picture[4 * i + 2];
  }
  return;
}
void pi_rotation(int w, int h, unsigned char *picture2,
                 unsigned char *picture3) {
  int i, j;
  for (i = 0; i < h; i++) {
    for (j = 0; j < w; j++) {
      picture3[w * i + j] = picture2[w * (h - i - 1) + w - 1 - j];
    }
  }
  return;
}
void color(int w, int h, unsigned char *picture3, unsigned char *picture4) {
  int i;
  for (i = 0; i < w * h; i++) {
    picture4[i * 4] = 2 * picture3[i] + 2 * picture3[i - 1];
    picture4[i * 4 + 1] = picture3[i];
    picture4[i * 4 + 2] = picture3[i] + 2 * picture3[i - 1];
    picture4[i * 4 + 3] = 255;
  }
  return;
}
void green_wall(int w, int h, unsigned char *picture3,
                unsigned char *picture4) {
  int i;
  for (i = 0; i < w * h; i++) {
    if (picture3[i] < 20) {
      picture4[i * 4] = 0;
      picture4[i * 4 + 1] = 255;
      picture4[i * 4 + 2] = 0;
      picture4[i * 4 + 3] = 255;
    }
  }
  return;
}

void Gauss(int w, int h, unsigned char *picture1, unsigned char *picture2) {
  int i, j;
  for (i = 1; i < h - 1; i++) {
    for (j = 1; j < w - 1; j++) {
      picture2[w * i + j] +=
          0.15 * picture1[w * i + j] +
          0.09 * (picture1[w * (i + 1) + (j - 1)] +
                  picture1[w * (i + 1) + (j + 1)] +
                  picture1[w * (i - 1) + (j - 1)] +
                  picture1[w * (i - 1) + (j + 1)]) +
          0.12 * (picture1[w * (i + 1) + j] + picture1[w * i + (j - 1)] +
                  picture1[w * i + (j + 1)] + picture1[w * (i - 1) + j]);
    }
  }
  return;
}

int main() {
  char *inputfile = "hand.png";
  int w, h;
  unsigned char *picture = loadPng(inputfile, &w, &h);
  char *modified = "hand_modified.png";
  if (!picture) {
    printf("I can't read this file %s. Error.\n", inputfile);
    return -1;
  }
  unsigned char *picture1 =
      (unsigned char *)malloc(h * w * sizeof(unsigned char));
  unsigned char *picture2 =
      (unsigned char *)malloc(h * w * sizeof(unsigned char));
  unsigned char *picture3 =
      (unsigned char *)malloc(h * w * sizeof(unsigned char));
  unsigned char *picture4 =
      (unsigned char *)malloc(4 * h * w * sizeof(unsigned char));
  remove_colors(w, h, picture, picture1);
  Gauss(w, h, picture1, picture2);
  pi_rotation(w, h, picture2, picture3);
  color(w, h, picture3, picture4);
  green_wall(w, h, picture3, picture4);
  writePng(modified, picture4, w, h);
  return 0;
}