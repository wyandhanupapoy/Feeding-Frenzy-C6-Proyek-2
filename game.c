#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Ukuran layar
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Struktur untuk karakter ikan
typedef struct {
    int x, y;
    int width, height;
    SDL_Texture* texture;
    double angle; // Untuk rotasi ikan
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

// Fungsi untuk menggambar ikan dengan rotasi
void renderFish(SDL_Renderer* renderer, Fish* fish) {
    SDL_Rect destRect = { fish->x, fish->y, fish->width, fish->height };
    SDL_RenderCopyEx(renderer, fish->texture, NULL, &destRect, fish->angle, NULL, SDL_FLIP_NONE);
}

// Fungsi untuk menghitung jarak antara dua titik
float distance(int x1, int y1, int x2, int y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// Fungsi untuk menggerakkan predator (AI)
void movePredator(Fish* predator, Fish* player) {
    // Prediksi arah menuju pemain
    float dist = distance(predator->x, predator->y, player->x, player->y);
    if (dist > 0) {
        int dx = player->x - predator->x;
        int dy = player->y - predator->y;
        float angle = atan2(dy, dx);
        
        // Gerakkan predator ke arah pemain
        predator->x += (int)(cos(angle) * 3); // Kecepatan predator 3
        predator->y += (int)(sin(angle) * 3);
        
        // Set angle untuk rotasi predator
        predator->angle = angle * 180 / M_PI; // Mengkonversi radian ke derajat
    }
}

// Fungsi untuk menggerakkan prey (AI acak)
void movePrey(Fish* prey) {
    // Gerakkan prey acak dalam batas layar
    int randomMove = rand() % 4;
    switch (randomMove) {
        case 0: prey->x += 2; break; // bergerak ke kanan
        case 1: prey->x -= 2; break; // bergerak ke kiri
        case 2: prey->y += 2; break; // bergerak ke bawah
        case 3: prey->y -= 2; break; // bergerak ke atas
    }

    // Pastikan prey tetap berada dalam layar
    if (prey->x < 0) prey->x = 0;
    if (prey->x > SCREEN_WIDTH - prey->width) prey->x = SCREEN_WIDTH - prey->width;
    if (prey->y < 0) prey->y = 0;
    if (prey->y > SCREEN_HEIGHT - prey->height) prey->y = SCREEN_HEIGHT - prey->height;
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
    Fish player = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50, loadTexture("player.png", renderer), 0 };
    Fish prey = { rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 30, 30, loadTexture("prey.png", renderer), 0 };
    Fish predator = { rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, 70, 70, loadTexture("predator.png", renderer), 0 };
    
    if (player.texture == NULL || prey.texture == NULL || predator.texture == NULL) {
        printf("Gagal memuat gambar ikan.\n");
        return -1;
    }

    SDL_Event e;
    int quit = 0;

    // Loop game
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Mengambil posisi mouse untuk pergerakan player
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        player.x = mouseX - player.width / 2;
        player.y = mouseY - player.height / 2;

        // Menggerakkan AI
        movePredator(&predator, &player);
        movePrey(&prey);

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
