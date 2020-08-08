/*

 mosaic image effect
 Last modified 17th of January 2019
 Copyright (c) 2019 Arthur Ferreira

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

// usage  : mosaic <bmp filename> <numbers of blocks> <max block size> [collide]
// last argument is optionnal and can be any string - if set the blocks won't overlap
// example : mosaic lena.bmp 200000 6
//           mosaic lena.bmp 70000 6 whyNot


#include <stdlib.h>
#include <SDL2/SDL.h>


SDL_bool collide(SDL_Rect r1, SDL_Rect r2){
  if (r1.x + r1.w < r2.x || r1.x > r2.x + r2.w) return SDL_FALSE;
  if (r1.y + r1.h < r2.y || r1.y > r2.y + r2.h) return SDL_FALSE;
  return SDL_TRUE;
}


Uint32 getPixel(SDL_Surface *surface, int x, int y){
  int depth = surface->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * depth;

  if (depth == 3) {
   if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
     return p[0] << 16 | p[1] << 8 | p[2];
   else
     return p[0] | p[1] << 8 | p[2] << 16;
  }
  if (depth == 1) return *p;
  if (depth == 2) return *(Uint16 *)p;
  if (depth == 4) return *(Uint32 *)p;
  return 0;
}


int main(int argc, char **argv){

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *surface = SDL_LoadBMP(argv[1]);
  Uint32 width = surface->w;
  Uint32 height = surface->h;
  SDL_Window *wdo = SDL_CreateWindow(argv[1], SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
  SDL_Renderer *rdr = SDL_CreateRenderer(wdo, -1, SDL_RENDERER_ACCELERATED);
  SDL_Event event;

  Uint32 squaresNum = atoi(argv[2]);
  Uint8  squareSize = atoi(argv[3]) ;
  if (squareSize < 2)
    squareSize = 2;

  Uint8 noColide = (argc==5) ? 1 : 0;

  SDL_Rect *squares = malloc(squaresNum * sizeof(SDL_Rect));
  Uint8 R, G, B;
  Uint32 color;

  for (int i=0; i<squaresNum; i++){
    squares[i].w = rand() % (squareSize -1) + 1;
    squares[i].h = squares[i].w;
    squares[i].x = rand() % width - squares[i].w / 2;
    squares[i].y = rand() % height - squares[i].h / 2;

    if (noColide)  // user requested no collision
      for (int j=0; j<i; j++)  // test collision with all existing squares
        if (collide(squares[i], squares[j])) {
          squaresNum--;
          i--;
          break;
        }

    color = getPixel(surface, squares[i].x + (squares[i].w / 2), squares[i].y + (squares[i].h / 2));
    SDL_GetRGB(color, surface->format, &R, &G, &B);
    SDL_SetRenderDrawColor(rdr, R, G, B, 255);
    SDL_RenderFillRect(rdr, &squares[i]);

    if (!(i%1000)){ // render progress every thousand of squares
      SDL_RenderPresent(rdr);
      while(SDL_PollEvent(&event))
        if (event.type==SDL_QUIT || event.type==SDL_KEYDOWN)
          i = squaresNum;  // stop adding squares
    }
  }
  SDL_RenderPresent(rdr);  // render final result

  while(SDL_TRUE)
    if (SDL_PollEvent(&event) && (event.type==SDL_QUIT || event.type==SDL_KEYDOWN))
      break;
    else
      SDL_Delay(256);

  SDL_DestroyRenderer(rdr);
  SDL_DestroyWindow(wdo);
  SDL_Quit();
  exit(EXIT_SUCCESS);
}
