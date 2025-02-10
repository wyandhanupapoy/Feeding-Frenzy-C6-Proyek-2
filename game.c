#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Ukuran layar
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Struktur untuk karakter ikan
typedef struct {
    int x, y;
    int width, height;
    SDL_Texture* texture;
} Fish;

// Fungsi untuk menginisialisasi SDL dan membuat window
int init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL gagal diinisialisasi! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    *window = SDL_CreateWindow("Feeding Frenzy Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window tidak dapat dibuat! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("Renderer tidak dapat dibuat! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Menginisialisasi SDL_image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("SDL_image gagal diinisialisasi! SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }

    return 0;
}

// Fungsi untuk memuat gambar sebagai texture
SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = NULL;
    newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == NULL) {
        printf("Gagal memuat gambar! SDL_image Error: %s\n", IMG_GetError());
    }
    return newTexture;
}

// Fungsi untuk menggambar ikan
void renderFish(SDL_Renderer* renderer, Fish* fish) {
    SDL_Rect destRect = { fish->x, fish->y, fish->width, fish->height };
    SDL_RenderCopy(renderer, fish->texture, NULL, &destRect);
}

// Fungsi utama
int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Inisialisasi SDL
    if (init(&window, &renderer) < 0) {
        printf("Inisialisasi gagal!\n");
        return -1;
    }

    // Memuat gambar
    Fish player = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50, loadTexture("player.png", renderer) };
    Fish prey = { rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 30, 30, loadTexture("prey.png", renderer) };
    Fish predator = { rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 70, 70, loadTexture("predator.png", renderer) };
    
    if (player.texture == NULL || prey.texture == NULL || predator.texture == NULL) {
        printf("Gagal memuat gambar ikan.\n");
        return -1;
    }

    SDL_Event e;
    int quit = 0;
    const int SPEED = 5;
    const int PREDATOR_SPEED = 3;

    // Loop game
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Menggerakkan ikan pemain dengan tombol panah
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_UP]) {
            player.y -= SPEED;
        }
        if (currentKeyStates[SDL_SCANCODE_DOWN]) {
            player.y += SPEED;
        }
        if (currentKeyStates[SDL_SCANCODE_LEFT]) {
            player.x -= SPEED;
        }
        if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
            player.x += SPEED;
        }

        // Menggerakkan ikan predator (untuk contoh, bergerak acak)
        predator.x += (rand() % 3 - 1) * PREDATOR_SPEED;
        predator.y += (rand() % 3 - 1) * PREDATOR_SPEED;

        // Mengecek apakah ikan pemain memakan ikan prey
        SDL_Rect playerRect = { player.x, player.y, player.width, player.height };
        SDL_Rect preyRect = { prey.x, prey.y, prey.width, prey.height };
        if (SDL_HasIntersection(&playerRect, &preyRect)) {
            prey.x = rand() % SCREEN_WIDTH;
            prey.y = rand() % SCREEN_HEIGHT;
        }

        // Mengecek apakah ikan predator menangkap pemain
        SDL_Rect predatorRect = { predator.x, predator.y, predator.width, predator.height };
        if (SDL_HasIntersection(&playerRect, &predatorRect)) {
            printf("Game Over! Anda dimakan oleh predator.\n");
            quit = 1;
        }

        // Membersihkan layar
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Menampilkan objek
        renderFish(renderer, &player);
        renderFish(renderer, &prey);
        renderFish(renderer, &predator);

        // Menyajikan render
        SDL_RenderPresent(renderer);

        SDL_Delay(16);  // Menunggu untuk frame berikutnya
    }

    // Membersihkan dan menutup
    SDL_DestroyTexture(player.texture);
    SDL_DestroyTexture(prey.texture);
    SDL_DestroyTexture(predator.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
