#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640; 
const int SCREEN_HEIGHT = 480; 
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;

SDL_Surface *load_image( std::string filename ) { 
  SDL_Surface* loadedImage = NULL; 
  SDL_Surface* optimizedImage = NULL;
  loadedImage = SDL_LoadBMP( filename.c_str() );
  if( loadedImage != NULL ) { 
    optimizedImage = SDL_ConvertSurfaceFormat( loadedImage, SDL_PIXELFORMAT_UNKNOWN, 0); 
    SDL_FreeSurface( loadedImage ); 
  }
  return optimizedImage; 
}

int init_sdl() {
  if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 ) { 
    return EXIT_FAILURE; 
  }

  screen = SDL_CreateWindow("My Game Window",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          SCREEN_WIDTH, SCREEN_HEIGHT,
                          SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);

  if( screen == NULL ) { 
    return EXIT_FAILURE; 
  }

}
