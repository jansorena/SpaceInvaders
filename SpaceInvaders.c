#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 680;
#define MAX_BALAS 1000
SDL_Rect* arrayBalas[MAX_BALAS] = {NULL};
SDL_Rect* arrayBalasEnemigos[MAX_BALAS] = {NULL};
SDL_Rect* arrayMisiles[MAX_BALAS] = {NULL};
char Score[100];
char mejoresPuntajes[6][100] = {"AAA","AAA","AAA","AAA","AAA","AAA"};
char mejoresScore[6][100] = {"000000","000000","000000","000000","000000","000000"};
int contadorGlobal = 0;
  
// Funciones 

SDL_Texture* cargarTexturas(const char* imagen,SDL_Surface* superficie, SDL_Renderer* renderer);
void posObjects(SDL_Rect* marciano, int posX, int posY,int width, int height);
void moveMarciano(SDL_Rect* marciano, int posicionX, int* direccion, int* contador);
void moveAlienJefe(SDL_Rect* boss, int play, bool* movimiento, bool* batallaFinal, int* der, int* izq);
void crearBalas(int a, int b);
void crearBalasEnemigas(int a, int b);
void crearMisiles(int a, int b);
void eliminarBalas(int i);
void eliminarBalasEnemigos(int i);
void eliminarMisiles(int i);
void movimientoBalas();
void movimientoBalasEnemigos();
void eliminarEnemigos(SDL_Rect* enemigo, int i);
void eliminarEscudos(SDL_Rect* escudo, int i);
void menu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key, int* dificultad);
void dificultadMenu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key,int* dificultad);
void comoJugarMenu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key);
void scoreMenu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key);
void juegoPrincipal(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char *key,int dificultad);
void gameOver(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char *key);
void menuVictoria(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char *key);
void pause(bool* run, bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key);
void guardarPuntajes(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key);

int main() {

  // aleatoridad
  srand(time(NULL));

  // Inicializar SDL

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  TTF_Init();

  // Crear una ventana

  SDL_Window* window = NULL;
  window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  // Crear un renderizador

  SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  //Teclado

  const unsigned char *key;
  key = SDL_GetKeyboardState(NULL);

  //Audio

  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  Mix_Music *musica_Menu = Mix_LoadMUS("Audio/menuMusica.wav");

  // Variable de evento e inicializacion

  SDL_Event event;

  // dificultad y flag del menu

  int dificultad = 60;
  bool exit = true;
  int flagMenu = 1;

  /***************************** Programa Principal *********************************/

  while(exit){
    while(flagMenu == 1){
      Mix_PlayMusic(musica_Menu, -1);

      menu(&exit, &flagMenu, renderer, &event, key, &dificultad);
      SDL_Delay(250);
    }
    while(flagMenu == 3){
      Mix_HaltMusic();

      juegoPrincipal(&exit, &flagMenu, renderer, &event, key, dificultad);
    }
    while(flagMenu == 4){
      Mix_HaltMusic();

      gameOver(&exit, &flagMenu, renderer, &event, key);
      SDL_Delay(250);
    }
    while(flagMenu == 6){
      Mix_HaltMusic();

      menuVictoria(&exit, &flagMenu, renderer, &event, key);
    }
  }
  /**************************** FIN Programa Principal ******************************/

  printf("Liberacion recursos final\n");
  SDL_Delay(250);
  Mix_HaltMusic();
  Mix_Quit();
  Mix_CloseAudio();
  Mix_FreeMusic(musica_Menu);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_Quit();
  SDL_Quit();
}

// Función del juego principal

void juegoPrincipal(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char *key, int dificultad){

  // declaracion de variables (Ventana y las superficies de las texturas)

  SDL_Window*  window = NULL;
  SDL_Surface* fondo = NULL;
  SDL_Surface* titulo_Menu = NULL;
  SDL_Surface* modelo_nave = NULL;
  SDL_Surface* image_Enemigos0 = NULL;
  SDL_Surface* image_Enemigos1 = NULL;
  SDL_Surface* image_Enemigos2 = NULL;
  SDL_Surface* image_Enemigos3 = NULL;
  SDL_Surface* shield_1 = NULL;
  SDL_Surface* shield_2 = NULL;
  SDL_Surface* shield_3 = NULL;
  SDL_Surface* shield_4 = NULL;
  SDL_Surface* shield_grieta_1 = NULL;
  SDL_Surface* shield_grieta_2 = NULL;
  SDL_Surface* shield_grieta_3 = NULL;
  SDL_Surface* shield_grieta_4 = NULL;
  SDL_Surface* balas = NULL;
  SDL_Surface* balas_Enemigos = NULL;
  SDL_Surface* explosion = NULL;
  SDL_Surface* misiles = NULL;
  SDL_Surface* modelo_nave_blanca = NULL;

  // Variables de control

  bool run = true;
  bool disparar_hack = true;
  bool flagExplosion = false;
  bool batallaFinal = false;
  bool movimiento = true;
  bool pararRandom = false;
  bool parpadeo = false;
  bool condicion_musica = false;

  // Flags movimiento y enteros de control

  int totalEnemigos = 55;
  int totalJefe = 1;
  int play = 0;
  int play2 = 1;
  int direccion = 0;
  int contador = 0;
  int frameActual = 0;
  int velocidad = 60;
  int duracionEscudos1[3][24];
  int cantidadVidas = 3;
  int movimientoJefeDer = 2000;
  int movimientoJefeIzq = -1000;

  // Duracion de los escudos

  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 24; j++){
      duracionEscudos1[i][j] = 1;
    }
  }

  // Audio

  Mix_Music *musica_fondo = Mix_LoadMUS("Audio/temaPrincipal.wav");
  Mix_Music *musica_jefe = Mix_LoadMUS("Audio/batallaJefe.wav");
  Mix_Chunk *disparo1 = Mix_LoadWAV("Audio/Disparo1.wav");
  Mix_Chunk *disparo2 = Mix_LoadWAV("Audio/Disparo2.wav");

  Mix_Volume(-1,MIX_MAX_VOLUME/2); // Volumen de los efectos
  //Mix_VolumeMusic(MIX_MAX_VOLUME/2)); para variar el volumen de la musica
  Mix_PlayMusic(musica_fondo, -1);

  // cargar Imagenes

  SDL_Texture* fondo_textura = cargarTexturas("Imagenes/fondoSpace.bmp",fondo,renderer);
  SDL_Texture* nave_textura = cargarTexturas("Imagenes/spaceship.bmp",modelo_nave,renderer);
  SDL_Texture* image_Enemigos0_textura = cargarTexturas("Imagenes/alienNaveJefe.bmp",image_Enemigos0,renderer);
  SDL_Texture* image_Enemigos1_textura = cargarTexturas("Imagenes/starship1.bmp",image_Enemigos1,renderer);
  SDL_Texture* image_Enemigos2_textura = cargarTexturas("Imagenes/starship2.bmp",image_Enemigos2,renderer);
  SDL_Texture* image_Enemigos3_textura = cargarTexturas("Imagenes/starship3.bmp",image_Enemigos3,renderer);
  SDL_Texture* balas_textura = cargarTexturas("Imagenes/disparoNave.bmp",balas,renderer);
  SDL_Texture* balas_enemigos_textura = cargarTexturas("Imagenes/disparoAlien.bmp",balas_Enemigos,renderer);
  SDL_Texture* explosion_textura = cargarTexturas("Imagenes/explosion.bmp",explosion,renderer);
  SDL_Texture* misiles_textura = cargarTexturas("Imagenes/missile.bmp",explosion,renderer);
  SDL_Texture* nave_blanca_textura = cargarTexturas("Imagenes/spaceship_white.bmp",modelo_nave_blanca,renderer);
  SDL_Texture* ptrNaveTextura = nave_textura;

  // Texturas escudos

  SDL_Texture* shield_textura1 = cargarTexturas("Imagenes/shield_1.bmp",shield_grieta_1,renderer);
  SDL_Texture* shield_textura2 = cargarTexturas("Imagenes/shield_2.bmp",shield_grieta_2,renderer);
  SDL_Texture* shield_textura3 = cargarTexturas("Imagenes/shield_3.bmp",shield_grieta_3,renderer);
  SDL_Texture* shield_textura4 = cargarTexturas("Imagenes/shield_4.bmp",shield_grieta_4,renderer);

  SDL_Texture* shield_grieta_textura1 = cargarTexturas("Imagenes/shieldGrieta_1.bmp",shield_1,renderer);
  SDL_Texture* shield_grieta_textura2 = cargarTexturas("Imagenes/shieldGrieta_2.bmp",shield_1,renderer);
  SDL_Texture* shield_grieta_textura3 = cargarTexturas("Imagenes/shieldGrieta_3.bmp",shield_1,renderer);
  SDL_Texture* shield_grieta_textura4 = cargarTexturas("Imagenes/shieldGrieta_4.bmp",shield_1,renderer);

  SDL_Texture* arregloEscudos[3][24];
  SDL_Texture* arregloEscudosGrietas[3][24];

  // punteros a texturas de los escudos con grietas

  for (int j = 0; j < 3 ; ++j){
    for (int i = 0; i < 24; i++) {
      if(i<6){
        arregloEscudos[j][i] = shield_textura1;
        arregloEscudosGrietas[j][i] = shield_grieta_textura1;
      }else{
        if(i<12){
          arregloEscudos[j][i] = shield_textura2;
          arregloEscudosGrietas[j][i] = shield_grieta_textura2;
        }else{
          if(i<18){
            arregloEscudos[j][i] = shield_textura3;
            arregloEscudosGrietas[j][i] = shield_grieta_textura3;
          }else{
            if(i<24){
              arregloEscudos[j][i] = shield_textura4;
              arregloEscudosGrietas[j][i] = shield_grieta_textura4;
            }
          }
        }
      }
    }
  }

  //Vidas

  SDL_Color color = {244, 236, 247};
  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);
  SDL_Surface* text1 = TTF_RenderText_Solid(font, "Vidas: ", color);
  SDL_Texture* mensaje1 = SDL_CreateTextureFromSurface(renderer, text1);

  SDL_Rect vidas_pantalla[4];

  posObjects(&vidas_pantalla[0],1000,640,100,30);
  posObjects(&vidas_pantalla[1],1135,635,34,29);
  posObjects(&vidas_pantalla[2],1169,635,34,29);
  posObjects(&vidas_pantalla[3],1203,635,34,29);

  //Score

  strcpy(Score,"000000"); // Score inicial

  SDL_Surface* text2 = TTF_RenderText_Solid(font, "Score: ", color);
  SDL_Texture* mensaje2 = SDL_CreateTextureFromSurface(renderer, text2);

  SDL_Surface* text3 = TTF_RenderText_Solid(font, Score, color);
  SDL_Texture* mensaje3 = SDL_CreateTextureFromSurface(renderer, text3);

  SDL_Rect score_pantalla[2];

  posObjects(&score_pantalla[0],10,640,100,30);
  posObjects(&score_pantalla[1],140,640,100,30);

  //Nave principal

  SDL_Rect nave;
  posObjects(&nave,605,565,68,58);

  //Nave Jefe

  SDL_Rect AlienJefe;
  posObjects(&AlienJefe,-92,20,80,30);

  //Naves Enemigas

  SDL_Rect Alien[5][11];

  int contadorPosicion = 69;
  for (int i = 0; i < 11; i++) {
    posObjects(&Alien[0][i],92+(i*contadorPosicion),100,50,47);
    posObjects(&Alien[1][i],92+(i*contadorPosicion)+2,150,45,47);
    posObjects(&Alien[2][i],92+(i*contadorPosicion)+2,200,45,47);
    posObjects(&Alien[3][i],92+(i*contadorPosicion)+3,250,40,43);
    posObjects(&Alien[4][i],92+(i*contadorPosicion)+3,300,40,43);
  }

  //Escudos

  SDL_Rect escudo1[3][24];

  for (int i = 1; i < 5; ++i){
    posObjects(&escudo1[0][i],150+((i-1)*50),476,25,17);
    posObjects(&escudo1[1][i],540+((i-1)*50),476,25,17);
    posObjects(&escudo1[2][i],930+((i-1)*50),476,25,17);
  }
  for (int i = 7; i < 11; ++i){
    posObjects(&escudo1[0][i],175+((i-7)*50),476,25,17);
    posObjects(&escudo1[1][i],565+((i-7)*50),476,25,17);
    posObjects(&escudo1[2][i],955+((i-7)*50),476,25,17);
  }
  for (int i = 13; i < 17; ++i){
    posObjects(&escudo1[0][i],150+((i-13)*50),493,25,17);
    posObjects(&escudo1[1][i],540+((i-13)*50),493,25,17);
    posObjects(&escudo1[2][i],930+((i-13)*50),493,25,17);
  }
  for (int i = 19; i < 23; ++i){
    posObjects(&escudo1[0][i],175+((i-19)*50),493,25,17);
    posObjects(&escudo1[1][i],565+((i-19)*50),493,25,17);
    posObjects(&escudo1[2][i],955+((i-19)*50),493,25,17);
  }

  for (int i = 0; i < 3; ++i){
    posObjects(&escudo1[i][0],150+(i*390),510,25,17), posObjects(&escudo1[i][5],300+(i*390),510,25,17);
    posObjects(&escudo1[i][6],175+(i*390),510,25,17),  posObjects(&escudo1[i][11],325+(i*390),510,25,17);
    posObjects(&escudo1[i][12],150+(i*390),527,25,17), posObjects(&escudo1[i][17],300+(i*390),527,25,17);
    posObjects(&escudo1[i][18],175+(i*390),527,25,17), posObjects(&escudo1[i][23],325+(i*390),527,25,17);
  }

  //Animacion de la explosion 960x768 pixels, 5x4 frames, 192x192

  int FramePosX = 0;
  int FramePosY = 0;

  SDL_Rect explosionFrame[20];
  for (int i = 0; i < 20; i++){

    if(FramePosX > 4) FramePosX = 0;
    if(i == 5) FramePosY = 192;
    if(i == 10) FramePosY = 384;
    if(i == 15) FramePosY = 576;

    explosionFrame[i].x = FramePosX*192;
    explosionFrame[i].y = FramePosY;
    explosionFrame[i].w = 192;
    explosionFrame[i].h = 192;

    FramePosX++;

  }
  // Estructura que muestra la explosion

  SDL_Rect explosion_pantalla;
  explosion_pantalla.x = 0;
  explosion_pantalla.y = 0;
  explosion_pantalla.w = 48;
  explosion_pantalla.h = 48;

  // juego principal

  while(run){
    // controles de tiempo
    if(play % 100 == 0){
      velocidad-= 1;
      if(velocidad <= 1) velocidad = 1;
    }

    if(play % velocidad == 0){
      if(contador < 40){
        contador++;
      }else{
        direccion = 1;
        contador++;
        if(contador == 80){
          direccion = 0;
          contador = 0;
        }
      }
      // movimiento de los aliens si estan en el area visible
      for (int i = 0; i < 5; i++){
        for (int j = 0; j < 11; j++) {
          if (Alien[i][j].x > 0 && Alien[i][j].x < SCREEN_WIDTH && Alien[i][j].y > 0 && Alien[i][j].y < SCREEN_HEIGHT) {
            moveMarciano(&Alien[i][j],782-(j*contadorPosicion),&direccion,&contador);
          }
        }
      }
    }

    // aleatoriedad de los disparos

    if(play % dificultad == 0 && !pararRandom){
      int i = rand()%4;
      int j = rand()%11;

      while(Alien[i][j].x == -2000){
        i = rand()%4;
        j = rand()%11;
      }
      crearBalasEnemigas(Alien[i][j].x,Alien[i][j].y);
    }

    // Movimiento de la nave Jefe

    if (play % 3 == 0) {
      moveAlienJefe(&AlienJefe,play,&movimiento,&batallaFinal,&movimientoJefeDer,&movimientoJefeIzq);
    }
    // cuando empieza a disparar la nave Jefe
    if(batallaFinal){
      if(play % dificultad == 0) crearMisiles(AlienJefe.x,AlienJefe.y);
    }

    // timing explosiones
    if(play%2==0){
      frameActual++;
      if(frameActual > 19){
        flagExplosion = false;
      }
    }

    play++; // tiempo total del juego

    // eventos del teclado

    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        run = false;
        *exit = false;
        *flagMenu = 0;

      }
      if(event->type == SDL_KEYDOWN){
        if(key[SDL_SCANCODE_LEFT]){
          if(nave.x >= 30) nave.x -= 30;
        }else if(key[SDL_SCANCODE_RIGHT]){
          if(nave.x <= 1170) nave.x += 30;
        }else if(key[SDL_SCANCODE_SPACE] && disparar_hack == true){
          crearBalas(nave.x,nave.y);
          Mix_PlayChannel(-1,disparo1,0);
          disparar_hack = false;
        }
        if(key[SDL_SCANCODE_RETURN]){
          Mix_PauseMusic();
          *flagMenu=5;
          pause(&run, exit, flagMenu, renderer, event, key);
          Mix_ResumeMusic();
        }
        // Para fines de la presentación: 

        // Muestra la pantalla de Victoria 

        if(key[SDL_SCANCODE_V]){
          *flagMenu=6;
          run=false;
        }
        // Muestra la pantalla de Game Over
        
        if(key[SDL_SCANCODE_G]){
          *flagMenu=4;
          run=false;
        }
      }
    }

    // impedir disparos consecutivos
    if(play % 50 == 0) disparar_hack = true;

    // condicion para ganar
    if(totalEnemigos == 0 && totalJefe == 0){
      run = false;
      *flagMenu=6;
    }

    // condicion para batallaFinal
    if(totalEnemigos == 5 && !condicion_musica){
      batallaFinal = true;
      Mix_PlayMusic(musica_jefe, -1);
      condicion_musica = true;
    }

    // condicion perder
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 11; j++) {
        if(Alien[i][j].y >=430){
          run = false;
          *flagMenu = 4;
        }
      }
    }

    if(cantidadVidas <= 0){
      run = false;
      *flagMenu = 4;
    }

    // parar aleatoriedad (evita el crasheo por un loop infinito)

    if(totalEnemigos == 0) pararRandom = true;

    // Limpiar la pantalla

    SDL_RenderClear(renderer);

    // Dibujar

    SDL_RenderCopy(renderer, fondo_textura, NULL, NULL);
    SDL_RenderCopy(renderer, nave_textura, NULL, &nave);

    // Animacion de parpadeo

    if(parpadeo){
      play2++;
      if(play2 % 10 == 0) nave_textura = nave_blanca_textura;
      if(play2 % 20 == 0) nave_textura = ptrNaveTextura;
      if(play2 % 30 == 0) nave_textura = nave_blanca_textura;
      if(play2 % 40 == 0){
        play2 = 1;
        parpadeo = false;
      }
    }

    // Dibujar vidas

    SDL_RenderCopy(renderer, mensaje1, NULL, &vidas_pantalla[0]);
    if(cantidadVidas >= 1) SDL_RenderCopy(renderer, ptrNaveTextura, NULL, &vidas_pantalla[1]);
    if(cantidadVidas >= 2) SDL_RenderCopy(renderer, ptrNaveTextura, NULL, &vidas_pantalla[2]);
    if(cantidadVidas >= 3) SDL_RenderCopy(renderer, ptrNaveTextura, NULL, &vidas_pantalla[3]);

    // Dibujar score

    text3 = TTF_RenderText_Solid(font, Score, color);
    mensaje3 = SDL_CreateTextureFromSurface(renderer, text3);

    SDL_RenderCopy(renderer, mensaje2, NULL, &score_pantalla[0]);
    SDL_RenderCopy(renderer, mensaje3, NULL, &score_pantalla[1]);

    // Dibujar enemigos

    SDL_RenderCopy(renderer, image_Enemigos0_textura, NULL, &AlienJefe);

    for (int i = 0; i < 11; i++) {
      SDL_RenderCopy(renderer, image_Enemigos1_textura, NULL, &Alien[0][i]);
      SDL_RenderCopy(renderer, image_Enemigos2_textura, NULL, &Alien[1][i]);
      SDL_RenderCopy(renderer, image_Enemigos2_textura, NULL, &Alien[2][i]);
      SDL_RenderCopy(renderer, image_Enemigos3_textura, NULL, &Alien[3][i]);
      SDL_RenderCopy(renderer, image_Enemigos3_textura, NULL, &Alien[4][i]);
    }

    // Dibujar escudos

    for (int j = 0; j < 3 ; ++j){
      for (int i = 0; i < 24; i++) {
        if(i<6){
          SDL_RenderCopy(renderer, arregloEscudos[j][i], NULL, &escudo1[j][i]);
        }else{
          if(i<12){
            SDL_RenderCopy(renderer, arregloEscudos[j][i], NULL, &escudo1[j][i]);
          }else{
            if(i<18){
              SDL_RenderCopy(renderer, arregloEscudos[j][i], NULL, &escudo1[j][i]);
            }else{
              if(i<24){
                SDL_RenderCopy(renderer, arregloEscudos[j][i], NULL, &escudo1[j][i]);
              }
            }
          }
        }
      }
    }

    // Dibujar balas

    for (int i = 0; i < MAX_BALAS; i++) {
      if(arrayBalas[i]!=NULL){
        SDL_Rect balas_pantalla;
        posObjects(&balas_pantalla,arrayBalas[i]->x+29,arrayBalas[i]->y,10,15);
        SDL_RenderCopy(renderer, balas_textura, NULL, &balas_pantalla);
        // colision bala nave y aliens
        for (int j = 0; j < 5; j++) {
          for (int k = 0; k < 11; k++) {
            if(SDL_HasIntersection(&Alien[j][k],&balas_pantalla)){
              explosion_pantalla.x = Alien[j][k].x;
              explosion_pantalla.y = Alien[j][k].y;
              totalEnemigos--;
              frameActual = 0;
              flagExplosion = true;
              eliminarEnemigos(&Alien[j][k],i);
              Mix_PlayChannel(-1,disparo2,0);
              int puntaje = atoi(Score);
              if(j==0){
                if( dificultad == 60) puntaje += 80;
                if( dificultad == 40) puntaje += 100;
                if( dificultad == 20) puntaje += 120;
              }
              if(j==1 || j==2){
                if( dificultad == 60) puntaje += 60;
                if( dificultad == 40) puntaje += 80;
                if( dificultad == 20) puntaje += 100;
              }
              if(j==3 || j==4){
                if( dificultad == 60) puntaje += 20;
                if( dificultad == 40) puntaje += 40;
                if( dificultad == 20) puntaje += 60;
              }
              sprintf(Score,"%06d",puntaje);
            }
          }
        }
        // colision bala nave y escudos
        for (int j = 0; j < 3; j++) {
          for (int k = 0; k < 24; k++) {
            if(SDL_HasIntersection(&escudo1[j][k],&balas_pantalla)){
              if(duracionEscudos1[j][k]==0){
                eliminarEnemigos(&escudo1[j][k],i);
              }else{
                arregloEscudos[j][k] = arregloEscudosGrietas[j][k];
                duracionEscudos1[j][k]--;
                eliminarBalas(i);
              }
            }
          }
        }
        // colisiones para la batalla contra el jefeFinal
        if(batallaFinal){
          if(SDL_HasIntersection(&AlienJefe,&balas_pantalla)){
            explosion_pantalla.x = AlienJefe.x;
            explosion_pantalla.y = AlienJefe.y;
            frameActual = 0;
            flagExplosion = true;
            eliminarEnemigos(&AlienJefe,i);
            Mix_PlayChannel(-1,disparo2,0);
            totalJefe = 0;
            int puntaje = atoi(Score);
            if( dificultad == 60) puntaje += 160;
            if( dificultad == 40) puntaje += 180;
            if( dificultad == 60) puntaje += 200;
            sprintf(Score,"%06d",puntaje);
          }
        }else{
          if(SDL_HasIntersection(&AlienJefe,&balas_pantalla)){
            int puntaje = atoi(Score);
            if( dificultad == 60) puntaje += 40;
            if( dificultad == 40) puntaje += 60;
            if( dificultad == 20) puntaje += 80;
            sprintf(Score,"%06d",puntaje);
            eliminarBalas(i);
          }
        }
      }
    }

    // Dibujar balas enemigos
    for (int i = 0; i < MAX_BALAS; i++) {
      if(arrayBalasEnemigos[i]!=NULL){
        SDL_Rect balas_pantalla_enemigos;
        posObjects(&balas_pantalla_enemigos,arrayBalasEnemigos[i]->x+20,arrayBalasEnemigos[i]->y,10,15);
        SDL_RenderCopy(renderer, balas_enemigos_textura, NULL, &balas_pantalla_enemigos);
        // colision con nave principal
        if(SDL_HasIntersection(&nave,&balas_pantalla_enemigos)){
          cantidadVidas--;
          eliminarBalasEnemigos(i);
          parpadeo = true;
        }
        // colision balas enemigas con escudo
        for (int j = 0; j < 3; j++) {
          for (int k = 0; k < 24; k++) {
            if(SDL_HasIntersection(&escudo1[j][k],&balas_pantalla_enemigos)){
              if(duracionEscudos1[j][k]==0){
                eliminarEscudos(&escudo1[j][k],i);
              }else{
                duracionEscudos1[j][k]--;
                eliminarBalasEnemigos(i);
                arregloEscudos[j][k] = arregloEscudosGrietas[j][k];
              }
            }
          }
        }
      }
    }

    // Dibujar misiles nave Jefe
    for (int i = 0; i < MAX_BALAS; i++){
      if(batallaFinal) {
        if(arrayMisiles[i]!=NULL){
          SDL_Rect misiles_pantalla;
          posObjects(&misiles_pantalla,arrayMisiles[i]->x+20,arrayMisiles[i]->y,14,25);
          SDL_RenderCopy(renderer, misiles_textura, NULL, &misiles_pantalla);
          //colision misil y nave
          if(SDL_HasIntersection(&nave,&misiles_pantalla)){
            cantidadVidas--;
            eliminarMisiles(i);
            parpadeo = true;
          }
          //colision misil y escudos
          for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 24; k++) {
              if(SDL_HasIntersection(&escudo1[j][k],&misiles_pantalla)){
                posObjects(&escudo1[j][k],-2000,-2000,0,0);
              }
            }
          }
        }
      }
    }

    // dibujar explosion solo si booleano = true

    if(flagExplosion) SDL_RenderCopy(renderer,explosion_textura,&explosionFrame[frameActual], &explosion_pantalla);

    // Mostrar lo que es dibujado

    SDL_RenderPresent(renderer);
    movimientoBalas();
    movimientoBalasEnemigos();

  }

  // Liberar recursos
  printf("Liberacion recursos del juego\n");
  Mix_HaltMusic();
  Mix_FreeMusic(musica_fondo);
  Mix_FreeMusic(musica_jefe);
  Mix_FreeChunk(disparo1);
  Mix_FreeChunk(disparo2);
  SDL_FreeSurface(fondo);
  SDL_FreeSurface(modelo_nave);
  SDL_FreeSurface(image_Enemigos0);
  SDL_FreeSurface(image_Enemigos1);
  SDL_FreeSurface(image_Enemigos2);
  SDL_FreeSurface(image_Enemigos3);
  SDL_FreeSurface(shield_1);
  SDL_FreeSurface(shield_2);
  SDL_FreeSurface(shield_3);
  SDL_FreeSurface(shield_4);
  SDL_FreeSurface(shield_grieta_1);
  SDL_FreeSurface(shield_grieta_2);
  SDL_FreeSurface(shield_grieta_3);
  SDL_FreeSurface(shield_grieta_4);
  SDL_FreeSurface(balas);
  SDL_FreeSurface(balas_Enemigos);
  SDL_FreeSurface(explosion);
  SDL_FreeSurface(misiles);
  SDL_FreeSurface(text1);
  SDL_FreeSurface(text2);
  SDL_FreeSurface(text3);
  SDL_FreeSurface(modelo_nave_blanca);
  SDL_DestroyTexture(fondo_textura);
  SDL_DestroyTexture(nave_textura);
  SDL_DestroyTexture(image_Enemigos0_textura);
  SDL_DestroyTexture(image_Enemigos1_textura);
  SDL_DestroyTexture(image_Enemigos2_textura);
  SDL_DestroyTexture(image_Enemigos3_textura);
  SDL_DestroyTexture(shield_textura1);
  SDL_DestroyTexture(balas_textura);
  SDL_DestroyTexture(balas_enemigos_textura);
  SDL_DestroyTexture(explosion_textura);
  SDL_DestroyTexture(misiles_textura);
  SDL_DestroyTexture(shield_textura1);
  SDL_DestroyTexture(shield_textura2);
  SDL_DestroyTexture(shield_textura3);
  SDL_DestroyTexture(shield_textura4);
  SDL_DestroyTexture(shield_grieta_textura1);
  SDL_DestroyTexture(shield_grieta_textura2);
  SDL_DestroyTexture(shield_grieta_textura3);
  SDL_DestroyTexture(shield_grieta_textura4);
  SDL_DestroyTexture(mensaje1);
  SDL_DestroyTexture(mensaje2);
  SDL_DestroyTexture(mensaje3);
  SDL_DestroyTexture(nave_blanca_textura);
  SDL_DestroyTexture(ptrNaveTextura);
  TTF_CloseFont(font);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 24; j++) {
      SDL_DestroyTexture(arregloEscudos[i][j]);
      SDL_DestroyTexture(arregloEscudosGrietas[i][j]);
    }
  }

  for (int i = 0; i < MAX_BALAS; i++) {
    eliminarBalas(i);
  }
  for (int i = 0; i < MAX_BALAS; i++) {
    eliminarBalasEnemigos(i);
  }
  for (int i = 0; i < MAX_BALAS; i++) {
    eliminarMisiles(i);
  }

}

// Funciones de los Menus


void menu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key, int* dificultad){

  SDL_Surface* selectBoton = NULL;
  SDL_Surface* titulo_Menu = NULL;
  SDL_Surface* image_Enemigos0 = NULL;
  SDL_Surface* image_Enemigos1 = NULL;
  SDL_Surface* image_Enemigos2 = NULL;
  SDL_Surface* image_Enemigos3 = NULL;

  SDL_Texture* boton_Texture = cargarTexturas("Imagenes/selectButton.bmp", selectBoton, renderer);
  SDL_Texture* titulo_textura = cargarTexturas("Imagenes/fondoMenu.bmp",titulo_Menu,renderer);
  SDL_Texture* image_Enemigos0_textura = cargarTexturas("Imagenes/alienNaveJefe.bmp",image_Enemigos0,renderer);
  SDL_Texture* image_Enemigos1_textura = cargarTexturas("Imagenes/starship1.bmp",image_Enemigos1,renderer);
  SDL_Texture* image_Enemigos2_textura = cargarTexturas("Imagenes/starship2.bmp",image_Enemigos2,renderer);
  SDL_Texture* image_Enemigos3_textura = cargarTexturas("Imagenes/starship3.bmp",image_Enemigos3,renderer);

  SDL_Rect aliens[4];
  posObjects(&aliens[0], 800, 400, 85, 30);
  posObjects(&aliens[1], 820, 450, 50, 47);
  posObjects(&aliens[2], 820, 500, 55, 47);
  posObjects(&aliens[3], 825, 550, 40, 43);


  int boton = 0;
  SDL_Rect botonS;
  posObjects(&botonS, 120, 370, 30, 30);

  SDL_Color color = {244, 236, 247};

  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);

  SDL_Surface* text1 = TTF_RenderText_Solid(font, "Comenzar a jugar ", color);
  SDL_Surface* text2 = TTF_RenderText_Solid(font, "Como jugar ", color);
  SDL_Surface* text3 = TTF_RenderText_Solid(font, "Dificultad ", color);
  SDL_Surface* text4 = TTF_RenderText_Solid(font, "Puntajes ", color);
  SDL_Surface* text5 = TTF_RenderText_Solid(font, "Salir ", color);
  SDL_Surface* text6 = TTF_RenderText_Solid(font, "= ??? ", color);
  SDL_Surface* text7 = TTF_RenderText_Solid(font, "= 80 ", color);
  SDL_Surface* text8 = TTF_RenderText_Solid(font, "= 60 ", color);
  SDL_Surface* text9 = TTF_RenderText_Solid(font, "= 20 ", color);

  SDL_Texture* mensaje1 = SDL_CreateTextureFromSurface(renderer, text1);
  SDL_Texture* mensaje2 = SDL_CreateTextureFromSurface(renderer, text2);
  SDL_Texture* mensaje3 = SDL_CreateTextureFromSurface(renderer, text3);
  SDL_Texture* mensaje4 = SDL_CreateTextureFromSurface(renderer, text4);
  SDL_Texture* mensaje5 = SDL_CreateTextureFromSurface(renderer, text5);
  SDL_Texture* mensaje6 = SDL_CreateTextureFromSurface(renderer, text6);
  SDL_Texture* mensaje7 = SDL_CreateTextureFromSurface(renderer, text7);
  SDL_Texture* mensaje8 = SDL_CreateTextureFromSurface(renderer, text8);
  SDL_Texture* mensaje9 = SDL_CreateTextureFromSurface(renderer, text9);


  SDL_Rect msg[9];
  posObjects(&msg[0], 200, 370, 350, 30);
  posObjects(&msg[1], 200, 420, 200, 30);
  posObjects(&msg[2], 200, 470, 200, 30);
  posObjects(&msg[3], 200, 520, 175, 30);
  posObjects(&msg[4], 200, 570, 100, 30);
  posObjects(&msg[5], 900, 400, 125, 30);
  posObjects(&msg[6], 900, 455, 100, 30);
  posObjects(&msg[7], 900, 505, 100, 30);
  posObjects(&msg[8], 900, 560, 100, 30);

  while(*flagMenu == 1){
    while(SDL_PollEvent(event)){

      if(event->type == SDL_QUIT){
        *exit = false;
        *flagMenu = 0;
      }
      if(event->type == SDL_KEYDOWN){
        if((key[SDL_SCANCODE_DOWN]) && boton < 4){
          botonS.y=botonS.y+50;
          boton++;
        }
        if((key[SDL_SCANCODE_UP]) && boton > 0){
          botonS.y=botonS.y-50;
          boton--;
        }
        if(key[SDL_SCANCODE_RETURN]){
          if(boton == 0){
            *flagMenu = 3;
          }
          if(boton == 1){
            *flagMenu=7;
            comoJugarMenu(exit, flagMenu, renderer, event, key);
          }
          if(boton == 2){
            *flagMenu=2;
            dificultadMenu(exit, flagMenu, renderer, event, key, dificultad);
          }
          if(boton == 3){
            *flagMenu = 8;
            scoreMenu(exit, flagMenu, renderer, event, key);
          }
          if(boton == 4){
            *exit = false;
            *flagMenu = 0;
          }
        }
      }
    }

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, titulo_textura, NULL, NULL);
    SDL_RenderCopy(renderer, boton_Texture, NULL, &botonS);
    SDL_RenderCopy(renderer, mensaje1, NULL, &msg[0]);
    SDL_RenderCopy(renderer, mensaje2, NULL, &msg[1]);
    SDL_RenderCopy(renderer, mensaje3, NULL, &msg[2]);
    SDL_RenderCopy(renderer, mensaje4, NULL, &msg[3]);
    SDL_RenderCopy(renderer, mensaje5, NULL, &msg[4]);
    SDL_RenderCopy(renderer, mensaje6, NULL, &msg[5]);
    SDL_RenderCopy(renderer, mensaje7, NULL, &msg[6]);
    SDL_RenderCopy(renderer, mensaje8, NULL, &msg[7]);
    SDL_RenderCopy(renderer, mensaje9, NULL, &msg[8]);

    SDL_RenderCopy(renderer, image_Enemigos0_textura, NULL, &aliens[0]);
    SDL_RenderCopy(renderer, image_Enemigos1_textura, NULL, &aliens[1]);
    SDL_RenderCopy(renderer, image_Enemigos2_textura, NULL, &aliens[2]);
    SDL_RenderCopy(renderer, image_Enemigos3_textura, NULL, &aliens[3]);
    SDL_RenderPresent(renderer);
  }
  printf("Liberacion recursos menu1\n");
  SDL_FreeSurface(titulo_Menu);
  SDL_FreeSurface(selectBoton);
  SDL_FreeSurface(image_Enemigos0);
  SDL_FreeSurface(image_Enemigos1);
  SDL_FreeSurface(image_Enemigos2);
  SDL_FreeSurface(image_Enemigos3);
  SDL_FreeSurface(text1);
  SDL_FreeSurface(text2);
  SDL_FreeSurface(text3);
  SDL_FreeSurface(text4);
  SDL_FreeSurface(text5);
  SDL_FreeSurface(text6);
  SDL_FreeSurface(text7);
  SDL_FreeSurface(text8);
  SDL_FreeSurface(text9);

  SDL_DestroyTexture(titulo_textura);
  SDL_DestroyTexture(boton_Texture);
  SDL_DestroyTexture(image_Enemigos0_textura);
  SDL_DestroyTexture(image_Enemigos1_textura);
  SDL_DestroyTexture(image_Enemigos2_textura);
  SDL_DestroyTexture(image_Enemigos3_textura);
  SDL_DestroyTexture(mensaje1);
  SDL_DestroyTexture(mensaje2);
  SDL_DestroyTexture(mensaje3);
  SDL_DestroyTexture(mensaje4);
  SDL_DestroyTexture(mensaje5);
  SDL_DestroyTexture(mensaje6);
  SDL_DestroyTexture(mensaje7);
  SDL_DestroyTexture(mensaje8);
  SDL_DestroyTexture(mensaje9);
  TTF_CloseFont(font);
}

void dificultadMenu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key,int* dificultad){

  SDL_Surface* titulo_Menu = NULL;
  SDL_Surface* selectBoton = NULL;

  SDL_Texture* titulo_textura = cargarTexturas("Imagenes/fondoMenu.bmp",titulo_Menu,renderer);
  SDL_Texture* boton_Texture = cargarTexturas("Imagenes/selectButton.bmp", selectBoton, renderer);

  TTF_Font* font = TTF_OpenFont("fuente.TTF",20);
  SDL_Color color2 = {217, 237, 237};

  SDL_Surface* dificultad_1 = TTF_RenderText_Solid(font, "Facil ", color2);
  SDL_Surface* dificultad_2 = TTF_RenderText_Solid(font, "Medio ", color2);
  SDL_Surface* dificultad_3 = TTF_RenderText_Solid(font, "Dificil ", color2);

  SDL_Texture* msjDificultad_1 = SDL_CreateTextureFromSurface(renderer, dificultad_1);
  SDL_Texture* msjDificultad_2 = SDL_CreateTextureFromSurface(renderer, dificultad_2);
  SDL_Texture* msjDificultad_3 = SDL_CreateTextureFromSurface(renderer, dificultad_3);


  SDL_Rect msg2[3];
  posObjects(&msg2[0], 200, 370, 125, 30);
  posObjects(&msg2[1], 200, 420, 125, 30);
  posObjects(&msg2[2], 200, 470, 175, 30);

  int boton = 0;
  SDL_Rect botonS;
  posObjects(&botonS, 120, 370, 30, 30);


  while(*flagMenu == 2){

    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        *exit = false;
        *flagMenu = 0;
      }

      if(event->type == SDL_KEYDOWN){
        if((key[SDL_SCANCODE_DOWN]) && boton<2){
          botonS.y=botonS.y+50;
          boton++;
        }
        if((key[SDL_SCANCODE_UP]) && boton>0){
          botonS.y=botonS.y-50;
          boton--;
        }
        if(key[SDL_SCANCODE_RETURN]){
          if(boton == 0){
            *dificultad = 60;
            *flagMenu = 1;
          }
          if(boton == 1){
            *dificultad = 40;
            *flagMenu = 1;
          }
          if(boton == 2){
            *dificultad = 20;
            *flagMenu = 1;
          }
        }
      }
    }

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, titulo_textura, NULL, NULL);
    SDL_RenderCopy(renderer, boton_Texture, NULL, &botonS);
    SDL_RenderCopy(renderer, msjDificultad_1, NULL, &msg2[0]);
    SDL_RenderCopy(renderer, msjDificultad_2, NULL, &msg2[1]);
    SDL_RenderCopy(renderer, msjDificultad_3, NULL, &msg2[2]);

    SDL_RenderPresent(renderer);
  }
  printf("Liberacion recursos dificultad\n");
  SDL_FreeSurface(titulo_Menu);
  SDL_FreeSurface(dificultad_1);
  SDL_FreeSurface(dificultad_2);
  SDL_FreeSurface(dificultad_3);
  SDL_FreeSurface(selectBoton);

  SDL_DestroyTexture(msjDificultad_1);
  SDL_DestroyTexture(msjDificultad_2);
  SDL_DestroyTexture(msjDificultad_3);
  SDL_DestroyTexture(boton_Texture);
  SDL_DestroyTexture(titulo_textura);
  TTF_CloseFont(font);

}

void gameOver(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char *key){

  SDL_Surface* overTitulo = NULL;
  SDL_Surface* selectBoton= NULL;

  SDL_Texture* overTitulo_texture = cargarTexturas("Imagenes/fondoGameOver.bmp", overTitulo, renderer);
  SDL_Texture* boton_Texture = cargarTexturas("Imagenes/selectButton.bmp", selectBoton, renderer);

  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);
  SDL_Color color3 = {217, 237, 237};

  SDL_Surface* opcionesOver1 = TTF_RenderText_Solid(font, "Volver a Jugar ", color3);
  SDL_Surface* opcionesOver2 = TTF_RenderText_Solid(font, "Volver al Menu Principal ", color3);
  SDL_Surface* opcionesOver3 = TTF_RenderText_Solid(font, "Salir del Juego ", color3);

  SDL_Texture* msgGameOver1 = SDL_CreateTextureFromSurface(renderer, opcionesOver1);
  SDL_Texture* msgGameOver2 = SDL_CreateTextureFromSurface(renderer, opcionesOver2);
  SDL_Texture* msgGameOver3 = SDL_CreateTextureFromSurface(renderer, opcionesOver3);

  SDL_Rect msg3[3];
  posObjects(&msg3[0], 520, 450, 250, 30);
  posObjects(&msg3[1], 485, 500, 315, 30);
  posObjects(&msg3[2], 520, 550, 250, 30);


  int boton=0;
  SDL_Rect botonS;
  posObjects(&botonS, 350, 450, 30, 30);

  Mix_Music *gameover1 = Mix_LoadMUS("Audio/gameOverMusica.wav");
  Mix_Music *gameover2 = Mix_LoadMUS("Audio/gameOverBucle.wav");

  Mix_PlayMusic(gameover1, 1);
  SDL_Delay(250);
  int contador=0;

  while(*flagMenu == 4){
    //printf("%d\n", contador );
    if(contador==1910)Mix_PlayMusic(gameover2, -1);
    if(contador<=1910)contador++;


    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        *exit = false;
        *flagMenu = 0;
      }
      if(event->type == SDL_KEYDOWN){
        if((key[SDL_SCANCODE_DOWN]) && boton<2){
          botonS.y=botonS.y+50;
          boton++;
        }
        if((key[SDL_SCANCODE_UP]) && boton>0){
          botonS.y=botonS.y-50;
          boton--;
        }
        if(key[SDL_SCANCODE_RETURN]){
          if(boton == 0){
            *flagMenu = 3;
          }
          if(boton == 1){
            *flagMenu = 1;
          }
          if(boton == 2){
            *exit = false;
            *flagMenu = 0;
          }
        }
      }
    }

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, overTitulo_texture, NULL, NULL);
    SDL_RenderCopy(renderer, boton_Texture, NULL, &botonS);
    SDL_RenderCopy(renderer, msgGameOver1, NULL, &msg3[0]);
    SDL_RenderCopy(renderer, msgGameOver2, NULL, &msg3[1]);
    SDL_RenderCopy(renderer, msgGameOver3, NULL, &msg3[2]);

    SDL_RenderPresent(renderer);
  }


  printf("Liberacion recursos gameOver\n");
  Mix_HaltMusic();
  Mix_FreeMusic(gameover1);

  SDL_FreeSurface(overTitulo);
  SDL_FreeSurface(opcionesOver1);
  SDL_FreeSurface(opcionesOver2);
  SDL_FreeSurface(opcionesOver3);
  SDL_FreeSurface(selectBoton);

  SDL_DestroyTexture(overTitulo_texture);
  SDL_DestroyTexture(msgGameOver1);
  SDL_DestroyTexture(msgGameOver2);
  SDL_DestroyTexture(msgGameOver3);
  SDL_DestroyTexture(boton_Texture);
  TTF_CloseFont(font);

}

void menuVictoria(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char *key){

  SDL_Surface* tituloVictoria = NULL;
  SDL_Surface* selectBoton= NULL;

  SDL_Texture* tituloVictoria_texture = cargarTexturas("Imagenes/fondoVictoria.bmp", tituloVictoria, renderer);

  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);
  SDL_Color color4 = {217, 237, 237};
  SDL_Color color5 = {22, 160, 133};

  SDL_Surface* opcionesVictoria1 = TTF_RenderText_Solid(font, "Volver a Jugar ", color4);
  SDL_Surface* opcionesVictoria2 = TTF_RenderText_Solid(font, "Volver al Menu Principal ", color4);
  SDL_Surface* opcionesVictoria3 = TTF_RenderText_Solid(font, "Salir del Juego ", color4);
  SDL_Surface* opcionesVictoria4 = TTF_RenderText_Solid(font, "Guardar puntaje", color4);
  SDL_Surface* msgVictorial = TTF_RenderText_Solid(font, "Se detuvo con exito la invasion alienigena", color5);

  SDL_Texture* msgVictoria1 = SDL_CreateTextureFromSurface(renderer, opcionesVictoria1);
  SDL_Texture* msgVictoria2 = SDL_CreateTextureFromSurface(renderer, opcionesVictoria2);
  SDL_Texture* msgVictoria3 = SDL_CreateTextureFromSurface(renderer, opcionesVictoria3);
  SDL_Texture* msgVictoria5 = SDL_CreateTextureFromSurface(renderer, msgVictorial);
  SDL_Texture* msgVictoria4 = SDL_CreateTextureFromSurface(renderer, opcionesVictoria4);

  SDL_Rect msg4[5];
  posObjects(&msg4[0], 350, 570, 250, 30);
  posObjects(&msg4[1], 350, 620, 315, 30);
  posObjects(&msg4[2], 750, 620, 250, 30);
  posObjects(&msg4[3], 750, 570, 250, 30);
  posObjects(&msg4[4], 150, 50, 1000, 50);


  SDL_Texture* boton_Texture = cargarTexturas("Imagenes/selectButton.bmp", selectBoton, renderer);
  int boton=0, boton2=0;
  SDL_Rect botonS;
  posObjects(&botonS, 300, 570, 30, 30);

  Mix_Chunk *victoria1 = Mix_LoadWAV("Audio/victoriaSonido.wav");
  Mix_Music *victoria2 = Mix_LoadMUS("Audio/victoriaMusica.wav");

  Mix_PlayChannel(-1, victoria1, 0);
  SDL_Delay(250);
  int contador=0;

  while(*flagMenu == 6){
    if(contador==400)Mix_PlayMusic(victoria2, -1);
    if(contador<=400)contador++;

    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        *exit = false;
        *flagMenu = 0;
      }
      if(event->type == SDL_KEYDOWN){
        if((key[SDL_SCANCODE_DOWN]) && boton<1){
          botonS.y=botonS.y+50;
          boton++;
        }
        if((key[SDL_SCANCODE_LEFT]) && boton2>0){
          botonS.x=botonS.x-400;
          boton2--;
        }
        if((key[SDL_SCANCODE_RIGHT]) && boton2<1){
          botonS.x=botonS.x+400;
          boton2++;
        }
        if((key[SDL_SCANCODE_UP]) && boton>0){
          botonS.y=botonS.y-50;
          boton--;
        }
        if(key[SDL_SCANCODE_RETURN]){
          if(boton == 0 && boton2 == 0){
            *flagMenu = 3;
          }
          if(boton == 1 && boton2 == 0){
            *flagMenu = 1;
          }
          if(boton == 0 && boton2 == 1){
            guardarPuntajes(exit,flagMenu,renderer,event,key);
          }
          if(boton == 1 && boton2 == 1){
            *exit = false;
            *flagMenu = 0;
          }

        }
      }
    }

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, tituloVictoria_texture, NULL, NULL);
    SDL_RenderCopy(renderer, boton_Texture, NULL, &botonS);
    SDL_RenderCopy(renderer, msgVictoria1, NULL, &msg4[0]);
    SDL_RenderCopy(renderer, msgVictoria2, NULL, &msg4[1]);
    SDL_RenderCopy(renderer, msgVictoria3, NULL, &msg4[2]);
    SDL_RenderCopy(renderer, msgVictoria4, NULL, &msg4[3]);
    SDL_RenderCopy(renderer, msgVictoria5, NULL, &msg4[4]);

    SDL_RenderPresent(renderer);
  }

  printf("Liberacion recursos Victoria\n");
  Mix_HaltMusic();
  Mix_FreeMusic(victoria2);
  Mix_FreeChunk(victoria1);

  SDL_FreeSurface(tituloVictoria);
  SDL_FreeSurface(selectBoton);
  SDL_FreeSurface(msgVictorial);
  SDL_FreeSurface(opcionesVictoria1);
  SDL_FreeSurface(opcionesVictoria2);
  SDL_FreeSurface(opcionesVictoria3);
  SDL_FreeSurface(opcionesVictoria4);

  SDL_DestroyTexture(tituloVictoria_texture);
  SDL_DestroyTexture(boton_Texture);
  SDL_DestroyTexture(msgVictoria1);
  SDL_DestroyTexture(msgVictoria2);
  SDL_DestroyTexture(msgVictoria3);
  SDL_DestroyTexture(msgVictoria4);
  SDL_DestroyTexture(msgVictoria5);

  TTF_CloseFont(font);

}

void pause(bool* run, bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key){
  SDL_Surface* fondoPause = NULL;
  SDL_Surface* selectBoton = NULL;

  SDL_Texture* fondoPause_textura = cargarTexturas("Imagenes/fondoPause.bmp", fondoPause, renderer);
  SDL_Texture* boton_Texture = cargarTexturas("Imagenes/selectButton.bmp", selectBoton, renderer);

  int boton = 0;
  SDL_Rect botonS;
  posObjects(&botonS, 450, 375, 30, 30);

  SDL_Color color = {244, 236, 247};
  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);
  SDL_Surface* texto = TTF_RenderText_Solid(font,  "PAUSE ", color);
  SDL_Surface* texto1 = TTF_RenderText_Solid(font, "Reanudar ", color);
  SDL_Surface* texto2 = TTF_RenderText_Solid(font, "Reiniciar ", color);
  SDL_Surface* texto3 = TTF_RenderText_Solid(font, "Volver al menu principal ", color);
  SDL_Surface* texto4 = TTF_RenderText_Solid(font, "Salir del juego ", color);

  SDL_Texture* mensaje1_1 = SDL_CreateTextureFromSurface(renderer, texto);
  SDL_Texture* mensaje2_2 = SDL_CreateTextureFromSurface(renderer, texto1);
  SDL_Texture* mensaje3_3 = SDL_CreateTextureFromSurface(renderer, texto2);
  SDL_Texture* mensaje4_4 = SDL_CreateTextureFromSurface(renderer, texto3);
  SDL_Texture* mensaje5_5 = SDL_CreateTextureFromSurface(renderer, texto4);

  SDL_Rect msg4[5];
  posObjects(&msg4[0], 400, 100, 550, 100);
  posObjects(&msg4[1], 565, 375, 150, 30);
  posObjects(&msg4[2], 565, 425, 150, 30);
  posObjects(&msg4[3], 500, 475, 300, 30);
  posObjects(&msg4[4], 550, 525, 200, 30);



  Mix_Chunk *musica_PauseI = Mix_LoadWAV("Audio/PauseInicio.wav");
  Mix_Chunk *musica_PauseF = Mix_LoadWAV("Audio/PauseFinal.wav");
  Mix_PlayChannel(-1,musica_PauseI,0);
  SDL_Delay(500);

  while(*flagMenu==5){
    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        *run=false;
        *exit=false;
        *flagMenu=0;
      }
      if(event->type == SDL_KEYDOWN){
        if((key[SDL_SCANCODE_DOWN]) && boton < 3){
          botonS.y=botonS.y+50;
          boton++;
        }
        if((key[SDL_SCANCODE_UP]) && boton > 0){
          botonS.y=botonS.y-50;
          boton--;
        }
        if(key[SDL_SCANCODE_RETURN]){
          if(boton == 0){
            *flagMenu=2;
          }
          if(boton == 1){
            *run=false;

            *flagMenu=3;
          }
          if(boton == 2){
            *flagMenu=1;
            *run=false;
          }
          if(boton == 3){
            *flagMenu=0;
            *run=false;
            *exit=false;
          }
        }
      }
    }

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, fondoPause_textura, NULL, NULL);
    SDL_RenderCopy(renderer, boton_Texture, NULL, &botonS);
    SDL_RenderCopy(renderer, mensaje1_1, NULL, &msg4[0]);
    SDL_RenderCopy(renderer, mensaje2_2, NULL, &msg4[1]);
    SDL_RenderCopy(renderer, mensaje3_3, NULL, &msg4[2]);
    SDL_RenderCopy(renderer, mensaje4_4, NULL, &msg4[3]);
    SDL_RenderCopy(renderer, mensaje5_5, NULL, &msg4[4]);



    SDL_RenderPresent(renderer);
    if(boton==0 && *flagMenu==2){
      Mix_PlayChannel(-1,musica_PauseF,0);
      SDL_Delay(550);
    }
    if(boton==1 && *flagMenu==3){
      while(boton<=3){
        Mix_PlayChannel(-1,musica_PauseF,0);

        SDL_Delay(550);
        boton++;
      }
    }
  }
  printf("Liberacion recursos Pause\n");
  Mix_FreeChunk(musica_PauseI);
  Mix_FreeChunk(musica_PauseF);

  SDL_FreeSurface(fondoPause);
  SDL_FreeSurface(selectBoton);
  SDL_FreeSurface(texto);
  SDL_FreeSurface(texto1);
  SDL_FreeSurface(texto2);
  SDL_FreeSurface(texto3);
  SDL_FreeSurface(texto4);

  SDL_DestroyTexture(fondoPause_textura);
  SDL_DestroyTexture(boton_Texture);
  SDL_DestroyTexture(mensaje1_1);
  SDL_DestroyTexture(mensaje2_2);
  SDL_DestroyTexture(mensaje3_3);
  SDL_DestroyTexture(mensaje4_4);
  SDL_DestroyTexture(mensaje5_5);
  TTF_CloseFont(font);
}

void comoJugarMenu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key){

  SDL_Surface* fondo = NULL;
  SDL_Surface* modelo_nave = NULL;
  SDL_Surface* balas = NULL;
  SDL_Surface* selectBoton =NULL;
  SDL_Surface* teclaRight = NULL;
  SDL_Surface* teclaLeft = NULL;
  SDL_Surface* teclaSpace = NULL;
  SDL_Surface* cuadrado = NULL;
  SDL_Surface* cuadrado2 =NULL;

  SDL_Texture* fondo_textura = cargarTexturas("Imagenes/fondoSpace.bmp",fondo,renderer);
  SDL_Texture* nave_textura = cargarTexturas("Imagenes/spaceship.bmp",modelo_nave,renderer);
  SDL_Texture* cuadro_textura = cargarTexturas("Imagenes/neonCuadro.bmp",cuadrado,renderer);
  SDL_Texture* cuadro2_textura = cargarTexturas("Imagenes/fondoCuadro.bmp",cuadrado2,renderer);
  SDL_Texture* balas_textura = cargarTexturas("Imagenes/disparoNave.bmp",balas,renderer);
  SDL_Texture* teclaRight_textura = cargarTexturas("Imagenes/teclaRight.bmp", teclaRight, renderer);
  SDL_Texture* teclaLeft_textura = cargarTexturas("Imagenes/teclaLeft.bmp", teclaRight, renderer);
  SDL_Texture* teclaSpace_textura = cargarTexturas("Imagenes/teclaSpace.bmp", teclaSpace, renderer);
  SDL_Texture* boton_Texture = cargarTexturas("Imagenes/selectButton.bmp", selectBoton, renderer);

  SDL_Color gray = {0,0,0};
  SDL_Color color6 = {204, 209, 209 };
  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);

  SDL_Surface* text1 = TTF_RenderText_Solid(font, "Como Jugar ", color6);
  SDL_Surface* text2 = TTF_RenderText_Solid(font, "Mover ", gray);
  SDL_Surface* text3 = TTF_RenderText_Solid(font, "Disparar ", gray);
  SDL_Surface* text4 = TTF_RenderText_Solid(font, "Volver al Menu Principal ", color6);
  SDL_Surface* text5 = TTF_RenderText_Solid(font, "Probar controles ", color6);

  SDL_Texture* mensaje1 = SDL_CreateTextureFromSurface(renderer, text1);
  SDL_Texture* mensaje2 = SDL_CreateTextureFromSurface(renderer, text2);
  SDL_Texture* mensaje3 = SDL_CreateTextureFromSurface(renderer, text3);
  SDL_Texture* mensaje4 = SDL_CreateTextureFromSurface(renderer, text4);
  SDL_Texture* mensaje5 = SDL_CreateTextureFromSurface(renderer, text5);

  SDL_Rect msg5[5];
  posObjects(&msg5[0], 250, 50, 300, 50);
  posObjects(&msg5[1], 700, 150, 100, 30);
  posObjects(&msg5[2], 900, 150, 150, 30);
  posObjects(&msg5[3], 150, 150, 300, 30);
  posObjects(&msg5[4], 150, 250, 250, 30);

  int boton = 0;
  SDL_Rect botonS;
  posObjects(&botonS, 100, 150, 30, 30);

  SDL_Rect teclas[3];
  posObjects(&teclas[0], 675, 240, 60, 45);
  posObjects(&teclas[1], 775, 240, 60, 45);
  posObjects(&teclas[2], 900, 240, 125, 40);

  bool probarControles=false;
  bool disparar_hack=true;
  int play=0;

  SDL_Rect nave;
  posObjects(&nave,605,565,68,58);

  SDL_Rect cuadro, cuadro2;
  posObjects(&cuadro, -50, 300, 1400, 400);
  posObjects(&cuadro2, 102, 350, 1100, 300);
  Mix_Chunk *disparo1 = Mix_LoadWAV("Audio/Disparo1.wav");


  while(*flagMenu==7){
    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        *exit=false;
        *flagMenu=0;
      }
      if(event->type == SDL_KEYDOWN){
        if((key[SDL_SCANCODE_DOWN]) && boton < 1){
          botonS.y=botonS.y+100;
          boton++;
        }
        if((key[SDL_SCANCODE_UP]) && boton > 0){
          botonS.y=botonS.y-100;
          boton--;
        }
        if(key[SDL_SCANCODE_RETURN]){
          if(boton==0){
            *flagMenu=1;
          }
          if(boton==1){
            probarControles=true;
          }
        }
        if(probarControles){
          if(key[SDL_SCANCODE_LEFT]){
            if(nave.x >= 150) nave.x -= 30;
          }else if(key[SDL_SCANCODE_RIGHT]){
            if(nave.x <= 1075) nave.x += 30;
          }else if(key[SDL_SCANCODE_SPACE] && disparar_hack == true){
            crearBalas(nave.x,nave.y);
            Mix_PlayChannel(-1,disparo1,0);
            disparar_hack = false;
          }
        }
      }
    }

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, fondo_textura, NULL, NULL);
    SDL_RenderCopy(renderer, boton_Texture, NULL, &botonS);
    SDL_RenderCopy(renderer, mensaje1, NULL, &msg5[0]);
    SDL_RenderCopy(renderer, mensaje2, NULL, &msg5[1]);
    SDL_RenderCopy(renderer, mensaje3, NULL, &msg5[2]);
    SDL_RenderCopy(renderer, mensaje4, NULL, &msg5[3]);
    SDL_RenderCopy(renderer, mensaje5, NULL, &msg5[4]);
    SDL_RenderCopy(renderer, teclaLeft_textura, NULL, &teclas[0]);
    SDL_RenderCopy(renderer, teclaRight_textura, NULL, &teclas[1]);
    SDL_RenderCopy(renderer, teclaSpace_textura, NULL, &teclas[2]);

    if(probarControles){
      if(play % 50 == 0) disparar_hack = true;
      SDL_RenderCopy(renderer, cuadro2_textura, NULL, &cuadro2);
      SDL_RenderCopy(renderer, nave_textura, NULL, &nave);
      SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro);

      for (int i = 0; i < MAX_BALAS; i++) {

        if(arrayBalas[i]!=NULL){
          SDL_Rect balas_pantalla;
          posObjects(&balas_pantalla,arrayBalas[i]->x+29,arrayBalas[i]->y,10,15);
          if(arrayBalas[i]->y >350 ){
            SDL_RenderCopy(renderer, balas_textura, NULL, &balas_pantalla);

          }else{
            eliminarBalas(i);
          }
        }

      }
      movimientoBalas();
      play++;
    }
    SDL_RenderPresent(renderer);
  }

  printf("Liberacion recursos como Jugar\n");

  Mix_FreeChunk(disparo1);
  SDL_FreeSurface(fondo);
  SDL_FreeSurface(modelo_nave);
  SDL_FreeSurface(balas);
  SDL_FreeSurface(selectBoton);
  SDL_FreeSurface(teclaRight);
  SDL_FreeSurface(teclaLeft);
  SDL_FreeSurface(teclaSpace);
  SDL_FreeSurface(cuadrado);
  SDL_FreeSurface(cuadrado2);
  SDL_FreeSurface(text1);
  SDL_FreeSurface(text2);
  SDL_FreeSurface(text3);
  SDL_FreeSurface(text4);
  SDL_FreeSurface(text5);
  SDL_DestroyTexture(fondo_textura);
  SDL_DestroyTexture(nave_textura);
  SDL_DestroyTexture(cuadro_textura);
  SDL_DestroyTexture(cuadro2_textura);
  SDL_DestroyTexture(balas_textura);
  SDL_DestroyTexture(teclaRight_textura);
  SDL_DestroyTexture(teclaLeft_textura);
  SDL_DestroyTexture(teclaSpace_textura);
  SDL_DestroyTexture(boton_Texture);
  SDL_DestroyTexture(mensaje1);
  SDL_DestroyTexture(mensaje2);
  SDL_DestroyTexture(mensaje3);
  SDL_DestroyTexture(mensaje4);
  SDL_DestroyTexture(mensaje5);
  TTF_CloseFont(font);
}

void scoreMenu(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key){

  SDL_Surface* fondo = NULL;
  SDL_Surface* selectBoton =NULL;
  SDL_Surface* cuadrado = NULL;
  SDL_Surface* cuadrado2 = NULL;

  SDL_Texture* fondo_textura = cargarTexturas("Imagenes/fondoSpace.bmp",fondo,renderer);
  SDL_Texture* boton_Texture = cargarTexturas("Imagenes/selectButton.bmp", selectBoton, renderer);
  SDL_Texture* cuadro_textura = cargarTexturas("Imagenes/neonCuadro.bmp",cuadrado,renderer);
  SDL_Texture* cuadro2_textura = cargarTexturas("Imagenes/fondoCuadro.bmp",cuadrado2,renderer);

  SDL_Color color7 = {204, 209, 209 };
  SDL_Color color8 = {39, 174, 96};
  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);

  SDL_Surface* text1 = TTF_RenderText_Solid(font, "Volver al menu principal ", color7);
  SDL_Surface* text2 = TTF_RenderText_Solid(font, "NOMBRE ", color7);
  SDL_Surface* text3 = TTF_RenderText_Solid(font, "PUNTAJE ", color7);

  SDL_Texture* mensaje1 = SDL_CreateTextureFromSurface(renderer, text1);
  SDL_Texture* mensaje2 = SDL_CreateTextureFromSurface(renderer, text2);
  SDL_Texture* mensaje3 = SDL_CreateTextureFromSurface(renderer, text3);

  SDL_Surface* num1 = TTF_RenderText_Solid(font, "1", color8);
  SDL_Surface* num2 = TTF_RenderText_Solid(font, "2", color8);
  SDL_Surface* num3 = TTF_RenderText_Solid(font, "3", color8);
  SDL_Surface* num4 = TTF_RenderText_Solid(font, "4", color8);
  SDL_Surface* num5 = TTF_RenderText_Solid(font, "5", color8);
  SDL_Surface* num6 = TTF_RenderText_Solid(font, "6", color8);

  SDL_Texture* msgNum1 = SDL_CreateTextureFromSurface(renderer, num1);
  SDL_Texture* msgNum2 = SDL_CreateTextureFromSurface(renderer, num2);
  SDL_Texture* msgNum3 = SDL_CreateTextureFromSurface(renderer, num3);
  SDL_Texture* msgNum4 = SDL_CreateTextureFromSurface(renderer, num4);
  SDL_Texture* msgNum5 = SDL_CreateTextureFromSurface(renderer, num5);
  SDL_Texture* msgNum6 = SDL_CreateTextureFromSurface(renderer, num6);

  SDL_Rect cuadro[8], cuadro2;
  posObjects(&cuadro[0], -50, 270, 1400, 80);
  posObjects(&cuadro[1], -50, 330, 1400, 80);
  posObjects(&cuadro[2], -50, 390, 1400, 80);
  posObjects(&cuadro[3], -50, 450, 1400, 80);
  posObjects(&cuadro[4], -50, 510, 1400, 80);
  posObjects(&cuadro[5], 110, 130, 650, 600);
  posObjects(&cuadro[6], 615, 130, 650, 600);
  posObjects(&cuadro[7], -50, 130, 1400, 600);

  posObjects(&cuadro2, 102, 205, 1100, 450);

  SDL_Rect msg6[3];
  posObjects(&msg6[0], 150, 100, 400, 50);
  posObjects(&msg6[1], 300, 225, 200, 50);
  posObjects(&msg6[2], 800, 225, 200, 50);

  SDL_Rect numeros[6];
  posObjects(&numeros[0], 150, 300, 20, 30);
  posObjects(&numeros[1], 150, 360, 20, 30);
  posObjects(&numeros[2], 150, 420, 20, 30);
  posObjects(&numeros[3], 150, 480, 25, 30);
  posObjects(&numeros[4], 150, 540, 20, 30);
  posObjects(&numeros[5], 150, 600, 20, 30);

  SDL_Rect botonS;
  posObjects(&botonS, 100, 109, 30, 30);


  SDL_Surface* puntaje1 = TTF_RenderText_Solid(font, mejoresPuntajes[0], color7);
  SDL_Surface* puntaje2 = TTF_RenderText_Solid(font, mejoresPuntajes[1], color7);
  SDL_Surface* puntaje3 = TTF_RenderText_Solid(font, mejoresPuntajes[2], color7);
  SDL_Surface* puntaje4 = TTF_RenderText_Solid(font, mejoresPuntajes[3], color7);
  SDL_Surface* puntaje5 = TTF_RenderText_Solid(font, mejoresPuntajes[4], color7);
  SDL_Surface* puntaje6 = TTF_RenderText_Solid(font, mejoresPuntajes[5], color7);

  SDL_Surface* score1 = TTF_RenderText_Solid(font, mejoresScore[0], color7);
  SDL_Surface* score2 = TTF_RenderText_Solid(font, mejoresScore[1], color7);
  SDL_Surface* score3 = TTF_RenderText_Solid(font, mejoresScore[2], color7);
  SDL_Surface* score4 = TTF_RenderText_Solid(font, mejoresScore[3], color7);
  SDL_Surface* score5 = TTF_RenderText_Solid(font, mejoresScore[4], color7);
  SDL_Surface* score6 = TTF_RenderText_Solid(font, mejoresScore[5], color7);

  SDL_Texture* puntaje_textura1 = SDL_CreateTextureFromSurface(renderer, puntaje1);
  SDL_Texture* puntaje_textura2 = SDL_CreateTextureFromSurface(renderer, puntaje2);
  SDL_Texture* puntaje_textura3 = SDL_CreateTextureFromSurface(renderer, puntaje3);
  SDL_Texture* puntaje_textura4 = SDL_CreateTextureFromSurface(renderer, puntaje4);
  SDL_Texture* puntaje_textura5 = SDL_CreateTextureFromSurface(renderer, puntaje5);
  SDL_Texture* puntaje_textura6 = SDL_CreateTextureFromSurface(renderer, puntaje6);

  SDL_Texture* score_textura1 = SDL_CreateTextureFromSurface(renderer, score1);
  SDL_Texture* score_textura2 = SDL_CreateTextureFromSurface(renderer, score2);
  SDL_Texture* score_textura3 = SDL_CreateTextureFromSurface(renderer, score3);
  SDL_Texture* score_textura4 = SDL_CreateTextureFromSurface(renderer, score4);
  SDL_Texture* score_textura5 = SDL_CreateTextureFromSurface(renderer, score5);
  SDL_Texture* score_textura6 = SDL_CreateTextureFromSurface(renderer, score6);

  SDL_Rect puntuaciones[12];
  posObjects(&puntuaciones[0], 300, 300, 100, 30);
  posObjects(&puntuaciones[1], 300, 360, 100, 30);
  posObjects(&puntuaciones[2], 300, 420, 100, 30);
  posObjects(&puntuaciones[3], 300, 480, 100, 30);
  posObjects(&puntuaciones[4], 300, 540, 100, 30);
  posObjects(&puntuaciones[5], 300, 600, 100, 30);

  posObjects(&puntuaciones[6], 800, 300, 100, 30);
  posObjects(&puntuaciones[7], 800, 360, 100, 30);
  posObjects(&puntuaciones[8], 800, 420, 100, 30);
  posObjects(&puntuaciones[9], 800, 480, 100, 30);
  posObjects(&puntuaciones[10], 800, 540, 100, 30);
  posObjects(&puntuaciones[11], 800, 600, 100, 30);

  while(*flagMenu==8){
    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        *exit=false;
        *flagMenu=0;
      }
      if(event->type == SDL_KEYDOWN){
        if(key[SDL_SCANCODE_RETURN]){
          *flagMenu=1;
        }
      }
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, fondo_textura, NULL, NULL);
    SDL_RenderCopy(renderer, boton_Texture, NULL, &botonS);
    SDL_RenderCopy(renderer, cuadro2_textura, NULL, &cuadro2);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[0]);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[1]);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[2]);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[3]);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[4]);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[5]);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[6]);
    SDL_RenderCopy(renderer, cuadro_textura, NULL, &cuadro[7]);
    SDL_RenderCopy(renderer, mensaje1, NULL, &msg6[0]);
    SDL_RenderCopy(renderer, mensaje2, NULL, &msg6[1]);
    SDL_RenderCopy(renderer, mensaje3, NULL, &msg6[2]);
    SDL_RenderCopy(renderer, msgNum1, NULL, &numeros[0]);
    SDL_RenderCopy(renderer, msgNum2, NULL, &numeros[1]);
    SDL_RenderCopy(renderer, msgNum3, NULL, &numeros[2]);
    SDL_RenderCopy(renderer, msgNum4, NULL, &numeros[3]);
    SDL_RenderCopy(renderer, msgNum5, NULL, &numeros[4]);
    SDL_RenderCopy(renderer, msgNum6, NULL, &numeros[5]);

    if(contadorGlobal>0){
      SDL_RenderCopy(renderer, puntaje_textura1, NULL, &puntuaciones[0]);
      SDL_RenderCopy(renderer, score_textura1, NULL, &puntuaciones[6]);
    }
    if(contadorGlobal>1){
      SDL_RenderCopy(renderer, puntaje_textura2, NULL, &puntuaciones[1]);
      SDL_RenderCopy(renderer, score_textura2, NULL, &puntuaciones[7]);
    }
    if(contadorGlobal>2){
      SDL_RenderCopy(renderer, puntaje_textura3, NULL, &puntuaciones[2]);
      SDL_RenderCopy(renderer, score_textura3, NULL, &puntuaciones[8]);
    }
    if(contadorGlobal>3){
      SDL_RenderCopy(renderer, puntaje_textura4, NULL, &puntuaciones[3]);
      SDL_RenderCopy(renderer, score_textura4, NULL, &puntuaciones[9]);
    }
    if(contadorGlobal>4){
      SDL_RenderCopy(renderer, puntaje_textura5, NULL, &puntuaciones[4]);
      SDL_RenderCopy(renderer, score_textura5, NULL, &puntuaciones[10]);
    }
    if(contadorGlobal>5){
      SDL_RenderCopy(renderer, puntaje_textura6, NULL, &puntuaciones[5]);
      SDL_RenderCopy(renderer, score_textura6, NULL, &puntuaciones[11]);
    }
    SDL_RenderPresent(renderer);
  }

  printf("Liberacion recursos tabla de puntuaciones\n");

  SDL_FreeSurface(fondo);
  SDL_FreeSurface(cuadrado);
  SDL_FreeSurface(cuadrado2);
  SDL_FreeSurface(selectBoton);
  SDL_FreeSurface(text1);
  SDL_FreeSurface(text2);
  SDL_FreeSurface(text3);
  SDL_FreeSurface(num1);
  SDL_FreeSurface(num2);
  SDL_FreeSurface(num3);
  SDL_FreeSurface(num4);
  SDL_FreeSurface(num5);
  SDL_FreeSurface(num6);
  SDL_FreeSurface(puntaje1);
  SDL_FreeSurface(puntaje2);
  SDL_FreeSurface(puntaje3);
  SDL_FreeSurface(puntaje4);
  SDL_FreeSurface(puntaje5);
  SDL_FreeSurface(puntaje6);
  SDL_FreeSurface(score1);
  SDL_FreeSurface(score2);
  SDL_FreeSurface(score3);
  SDL_FreeSurface(score4);
  SDL_FreeSurface(score5);
  SDL_FreeSurface(score6);

  SDL_DestroyTexture(fondo_textura);
  SDL_DestroyTexture(boton_Texture);
  SDL_DestroyTexture(cuadro_textura);
  SDL_DestroyTexture(cuadro2_textura);
  SDL_DestroyTexture(mensaje1);
  SDL_DestroyTexture(mensaje2);
  SDL_DestroyTexture(mensaje3);
  SDL_DestroyTexture(msgNum1);
  SDL_DestroyTexture(msgNum2);
  SDL_DestroyTexture(msgNum3);
  SDL_DestroyTexture(msgNum4);
  SDL_DestroyTexture(msgNum5);
  SDL_DestroyTexture(msgNum6);
  SDL_DestroyTexture(puntaje_textura1);
  SDL_DestroyTexture(puntaje_textura2);
  SDL_DestroyTexture(puntaje_textura3);
  SDL_DestroyTexture(puntaje_textura4);
  SDL_DestroyTexture(puntaje_textura5);
  SDL_DestroyTexture(puntaje_textura6);
  SDL_DestroyTexture(score_textura1);
  SDL_DestroyTexture(score_textura2);
  SDL_DestroyTexture(score_textura3);
  SDL_DestroyTexture(score_textura4);
  SDL_DestroyTexture(score_textura5);
  SDL_DestroyTexture(score_textura6);

  TTF_CloseFont(font);


}

void guardarPuntajes(bool* exit, int* flagMenu, SDL_Renderer* renderer, SDL_Event* event, const unsigned char* key){

  bool run = true;
  TTF_Font* font= TTF_OpenFont("fuente.TTF",20);
  SDL_Color color4 = {217, 237, 237};

  char string[26][100] = {"A","B","C","D","E","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
  int indice1 = 0;
  int indice2 = 0;
  int indice3 = 0;
  int boton = 0;

  SDL_Surface* instruccion1 = TTF_RenderText_Solid(font, "Ingresa tu nombre ", color4);
  SDL_Surface* ayuda1 = TTF_RenderText_Solid(font, "_", color4);

  SDL_Texture* instruccion_textura1 = SDL_CreateTextureFromSurface(renderer, instruccion1);
  SDL_Texture* ayuda_textura1 = SDL_CreateTextureFromSurface(renderer, ayuda1);

  SDL_Surface* primeraLetra = TTF_RenderText_Solid(font, string[indice1], color4);
  SDL_Surface* segundaLetra = TTF_RenderText_Solid(font, string[indice2], color4);
  SDL_Surface* terceraLetra = TTF_RenderText_Solid(font, string[indice3], color4);

  SDL_Texture* primeraLetra_textura = SDL_CreateTextureFromSurface(renderer, primeraLetra);
  SDL_Texture* segundaLetra_textura = SDL_CreateTextureFromSurface(renderer, segundaLetra);
  SDL_Texture* terceraLetra_textura = SDL_CreateTextureFromSurface(renderer, terceraLetra);

  SDL_Surface* tituloVictoria = NULL;
  SDL_Texture* tituloVictoria_texture = cargarTexturas("Imagenes/fondoVictoria.bmp", tituloVictoria, renderer);

  SDL_Rect instruccionRect;
  posObjects(&instruccionRect, 500, 550, 300, 30);

  SDL_Rect guionBajo;
  posObjects(&guionBajo, 600, 625, 20, 30);

  SDL_Rect letras_pantallas[3];
  posObjects(&letras_pantallas[0], 600, 600, 20, 30);
  posObjects(&letras_pantallas[1], 630, 600, 20, 30);
  posObjects(&letras_pantallas[2], 660, 600, 20, 30);

  while(run){
    while(SDL_PollEvent(event)){
      if(event->type == SDL_QUIT){
        *exit = false;
        *flagMenu = 0;
        run = false;
      }
      if(event->type == SDL_KEYDOWN){

        if(key[SDL_SCANCODE_DOWN]){

          if(boton == 0){
            indice1--;
            if(indice1 > 24) indice1 = 0;
            if(indice1 < 0) indice1 = 24;
            primeraLetra = TTF_RenderText_Solid(font, string[indice1], color4);
            primeraLetra_textura = SDL_CreateTextureFromSurface(renderer, primeraLetra);
          }else{
            if(boton == 1){
              indice2--;
              if(indice2 > 24) indice2 = 0;
              if(indice2 < 0) indice2 = 24;
              segundaLetra = TTF_RenderText_Solid(font, string[indice2], color4);
              segundaLetra_textura = SDL_CreateTextureFromSurface(renderer, segundaLetra);
            }else{
              if(boton == 2){
                indice3--;
                if(indice3 > 24) indice3 = 0;
                if(indice3 < 0) indice3 = 24;
                terceraLetra = TTF_RenderText_Solid(font, string[indice3], color4);
                terceraLetra_textura = SDL_CreateTextureFromSurface(renderer, terceraLetra);
              }
            }
          }
        }

        if((key[SDL_SCANCODE_RIGHT]) && boton < 2){
          guionBajo.x=guionBajo.x+30;
          boton++;
        }
        if((key[SDL_SCANCODE_LEFT]) && boton > 0){
          guionBajo.x=guionBajo.x-30;
          boton--;
        }

        if(key[SDL_SCANCODE_UP]){

          if(boton == 0){
            indice1++;
            if(indice1 > 24) indice1 = 0;
            if(indice1 < 0) indice1 = 24;
            primeraLetra = TTF_RenderText_Solid(font, string[indice1], color4);
            primeraLetra_textura = SDL_CreateTextureFromSurface(renderer, primeraLetra);
          }else{
            if(boton == 1){
              indice2++;
              if(indice2 > 24) indice2 = 0;
              if(indice2 < 0) indice2 = 24;
              segundaLetra = TTF_RenderText_Solid(font, string[indice2], color4);
              segundaLetra_textura = SDL_CreateTextureFromSurface(renderer, segundaLetra);
            }else{
              if(boton == 2){
                indice3++;
                if(indice3 > 24) indice3 = 0;
                if(indice3 < 0) indice3 = 24;
                terceraLetra = TTF_RenderText_Solid(font, string[indice3], color4);
                terceraLetra_textura = SDL_CreateTextureFromSurface(renderer, terceraLetra);
              }
            }
          }
        }

        if(key[SDL_SCANCODE_RETURN]){
          // copiar en la posicion menor del arreglo


          if(contadorGlobal>=6) contadorGlobal = 6;

          if(contadorGlobal < 6){
            strcpy(mejoresPuntajes[contadorGlobal],string[indice1]);
            strcat(mejoresPuntajes[contadorGlobal],string[indice2]);
            strcat(mejoresPuntajes[contadorGlobal],string[indice3]);

            sprintf(mejoresScore[contadorGlobal],"%06d",atoi(Score));

            contadorGlobal++;
          }else{
            if(atoi(Score) > atoi(mejoresScore[5])){
              strcpy(mejoresPuntajes[5],string[indice1]);
              strcat(mejoresPuntajes[5],string[indice2]);
              strcat(mejoresPuntajes[5],string[indice3]);

              sprintf(mejoresScore[5],"%06d",atoi(Score));
            }
          }

          // ordenar el arreglo

          for (int i = 5; i >= 0; i--){
            for (int j = 4-i; j >= 0; j--){
              if (atoi(mejoresScore[j]) < atoi(mejoresScore[j+1])){
                char temp [100];
                strcpy(temp,mejoresScore[j]);
                strcpy(mejoresScore[j],mejoresScore[j+1]);
                strcpy(mejoresScore[j+1],temp);

                char temp2 [100];
                strcpy(temp2,mejoresPuntajes[j]);
                strcpy(mejoresPuntajes[j],mejoresPuntajes[j+1]);
                strcpy(mejoresPuntajes[j+1],temp2);
              }
            }
          }
          run = false;
        }
      }
    }

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, tituloVictoria_texture, NULL, NULL);
    SDL_RenderCopy(renderer, instruccion_textura1, NULL, &instruccionRect);
    SDL_RenderCopy(renderer, primeraLetra_textura, NULL, &letras_pantallas[0]);
    SDL_RenderCopy(renderer, segundaLetra_textura, NULL, &letras_pantallas[1]);
    SDL_RenderCopy(renderer, terceraLetra_textura, NULL, &letras_pantallas[2]);
    SDL_RenderCopy(renderer, ayuda_textura1, NULL, &guionBajo);

    SDL_RenderPresent(renderer);
  }

  printf("Liberacion recursos guardar puntajes\n");

  SDL_FreeSurface(tituloVictoria);
  SDL_FreeSurface(instruccion1);
  SDL_FreeSurface(primeraLetra);
  SDL_FreeSurface(segundaLetra);
  SDL_FreeSurface(terceraLetra);
  SDL_FreeSurface(ayuda1);

  SDL_DestroyTexture(tituloVictoria_texture);
  SDL_DestroyTexture(instruccion_textura1);
  SDL_DestroyTexture(primeraLetra_textura);
  SDL_DestroyTexture(segundaLetra_textura);
  SDL_DestroyTexture(terceraLetra_textura);
  SDL_DestroyTexture(ayuda_textura1);

  TTF_CloseFont(font);
}


// Funciones  


SDL_Texture* cargarTexturas(const char* imagen,SDL_Surface* superficie,SDL_Renderer* renderer){
  superficie = SDL_LoadBMP(imagen);
  SDL_Texture* textura_imagen = SDL_CreateTextureFromSurface(renderer,superficie);
  SDL_FreeSurface(superficie);
  return textura_imagen;
}

void posObjects(SDL_Rect* marciano, int posX, int posY,int width, int height){
  marciano->x = posX;
  marciano->y = posY;
  marciano->w = width;
  marciano->h = height;
}

void moveMarciano(SDL_Rect* marciano, int posicionX, int* direccion, int* contador){
  if(*contador==40){
    marciano->y=marciano->y +30;
  }
  if(*contador==0){
    marciano->y=marciano->y + 30;
    marciano->x=marciano->x - 10;
  }
  if(*direccion==0){
    if(marciano->x <= SCREEN_WIDTH - posicionX-20){
      marciano->x = marciano->x + 10;
    }
  }
  if(*direccion==1){
    if(marciano->x >= 92){
      marciano->x = marciano->x - 10;
    }
  }
}

void moveAlienJefe(SDL_Rect* boss, int play, bool* movimiento, bool* batallaFinal, int* der, int* izq){

  if(*movimiento){
    if(boss->x < *der){
      boss->x += 10;
    }else *movimiento = false;
  }else{
    if(boss->x > *izq){
      boss->x -= 10;
    }else *movimiento = true;
  }

  if(*batallaFinal) {
    *der = 1200;
    *izq = 0;
  }
}

void crearBalas(int a, int b){
  int encontrado = -1;
  for (int i = 0; i < MAX_BALAS; i++) {
    if(arrayBalas[i] == NULL){
      encontrado = i;
      break;
    }
  }
  if(encontrado>=0){
    arrayBalas[encontrado] = malloc(sizeof(SDL_Rect));
    arrayBalas[encontrado]->x = a;
    arrayBalas[encontrado]->y = b;
  }
}

void crearBalasEnemigas(int a, int b){
  int encontrado = -1;
  for (int i = 0; i < MAX_BALAS; i++) {
    if(arrayBalasEnemigos[i] == NULL){
      encontrado = i;
      break;
    }
  }
  if(encontrado>=0){
    arrayBalasEnemigos[encontrado] = malloc(sizeof(SDL_Rect));
    arrayBalasEnemigos[encontrado]->x = a;
    arrayBalasEnemigos[encontrado]->y = b;
  }
}

void eliminarBalas(int i){
  if(arrayBalas[i]!=NULL){
    free(arrayBalas[i]);
    arrayBalas[i] = NULL;
  }
}

void eliminarBalasEnemigos(int i){
  if(arrayBalasEnemigos[i]!=NULL){
    free(arrayBalasEnemigos[i]);
    arrayBalasEnemigos[i] = NULL;
  }
}

void movimientoBalas(){
  for (int i = 0; i < MAX_BALAS; i++) {
    if(arrayBalas[i]!=NULL){
      arrayBalas[i]->y -= 10;
      if(arrayBalas[i]->y < 0 || arrayBalas[i]->y > SCREEN_HEIGHT){
        eliminarBalas(i);
      }
    }
  }
}

void movimientoBalasEnemigos(){
  for (int i = 0; i < MAX_BALAS; i++) {
    if(arrayBalasEnemigos[i]!=NULL){
      arrayBalasEnemigos[i]->y += 5;
      if(arrayBalasEnemigos[i]->y < 0 || arrayBalasEnemigos[i]->y > SCREEN_HEIGHT){
        eliminarBalasEnemigos(i);
      }
    }
    if(arrayMisiles[i]!=NULL){
      arrayMisiles[i]->y += 5;
      if(arrayMisiles[i]->y < 0 || arrayMisiles[i]->y > SCREEN_HEIGHT){
        eliminarMisiles(i);
      }
    }
  }
}

void eliminarEnemigos(SDL_Rect* enemigo, int i){
  posObjects(enemigo,-2000,-2000,0,0);
  eliminarBalas(i);
}

void eliminarEscudos(SDL_Rect* escudo, int i){
  posObjects(escudo,-2000,-2000,0,0);
  eliminarBalasEnemigos(i);
}

void crearMisiles(int a, int b){
  int encontrado = -1;
  for (int i = 0; i < MAX_BALAS; i++) {
    if(arrayMisiles[i] == NULL){
      encontrado = i;
      break;
    }
  }
  if(encontrado>=0){
    arrayMisiles[encontrado] = malloc(sizeof(SDL_Rect));
    arrayMisiles[encontrado]->x = a;
    arrayMisiles[encontrado]->y = b;
  }
}

void eliminarMisiles(int i){
  if(arrayMisiles[i]!=NULL){
    free(arrayMisiles[i]);
    arrayMisiles[i] = NULL;
  }
}