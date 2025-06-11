// DenteDefensor
// Linguagem de programação: C++
// Biblioteca: Raylib
// Autor: Andrey de Souza Setúbal Destro

// --- Inclusão de Bibliotecas ---
// Esta seção inclui todas as bibliotecas necessárias para o funcionamento do jogo.
#include "raylib.h"      // Biblioteca principal da Raylib para gráficos, som, entrada, etc. É a base para todas as funcionalidades visuais e interativas.
#include <vector>        // Para usar std::vector, que permite criar arrays dinâmicos (listas) de objetos, como dentes, inimigos e projéteis.
#include <string>        // Para usar std::string, útil para manipular e formatar texto, como a exibição da pontuação.
#include <cmath>         // Necessário para funções matemáticas como sqrt (raiz quadrada) e pow (potência), usadas na normalização de vetores.
#include <raymath.h>     // Biblioteca específica da Raylib para operações matemáticas com vetores 2D (Vector2Normalize, Vector2Subtract, Vector2Add, Vector2Scale).
#include <algorithm>     // Para usar std::remove_if, uma função que permite remover elementos de vetores de forma eficiente, baseada em uma condição.
#include <limits>        // Para usar numeric_limits, que fornece informações sobre limites de tipos numéricos (como o valor máximo de um int), utilizado na IA dos inimigos.

// --- Constantes Globais ---
// Definem valores fixos que são usados em todo o jogo.
const int screenWidth = 800;  // Define a largura da janela do jogo em pixels.
const int screenHeight = 600; // Define a altura da janela do jogo em pixels.

// Variável global para a textura do projétil.
// É global para ser facilmente acessível por diferentes classes, como Player.
Texture2D globalProjectileTexture;

//------------------------------------------------------------------------------------
// Classe Tooth (Dente)
// Representa um dos dentes que o jogador deve proteger.
//------------------------------------------------------------------------------------
class Tooth {
public:
    // Propriedades (membros) da classe Tooth:
    Rectangle rect;    // Um 'Rectangle' da Raylib que define a posição (x, y) e as dimensões (largura, altura) do dente para detecção de colisão e desenho.
    int health;        // A saúde atual do dente. Quando chega a 0, o dente é considerado destruído.
    Texture2D texture; // A textura (imagem) que será desenhada para representar o dente.

    // Construtor da classe Tooth:
    // Chamado quando um novo objeto Tooth é criado.
    Tooth(float x, float y, Texture2D toothTexture) {
        // Inicializa o retângulo de colisão e posição do dente com base nas coordenadas e dimensões da textura.
        rect = {x, y, (float)toothTexture.width, (float)toothTexture.height};
        health = 3;        // Dentes começam com 3 pontos de saúde.
        texture = toothTexture; // Armazena a textura fornecida.
    }

    // Método Draw:
    // Responsável por desenhar o dente na tela.
    void Draw() {
        Color tintColor = WHITE; // Cor padrão para o dente (branco).
        // Altera a cor de matiz do dente com base na sua saúde para indicar o nível de dano.
        switch (health) {
            case 3: tintColor = WHITE; break;    // Saúde total: branco.
            case 2: tintColor = YELLOW; break;   // Saúde média: amarelo.
            case 1: tintColor = BROWN; break;    // Saúde baixa: marrom (para simular cárie/dano grave).
            default: tintColor = GRAY; break;    // Dente destruído (saúde <= 0): cinza.
        }
        // Desenha a textura do dente nas coordenadas especificadas pelo 'rect', aplicando a cor de matiz.
        DrawTexture(texture, (int)rect.x, (int)rect.y, tintColor);
    }

    // Método Damage:
    // Reduz a saúde do dente.
    void Damage() {
        if (health > 0) { // Verifica se o dente ainda tem saúde antes de diminuir.
            health--;     // Decrementa a saúde.
        }
    }

    // Método IsDestroyed:
    // Retorna verdadeiro se o dente foi destruído (saúde <= 0).
    bool IsDestroyed() {
        return health <= 0;
    }

    // Método GetCenter:
    // Retorna as coordenadas do centro do dente. Útil para inimigos que miram nos dentes.
    Vector2 GetCenter() {
        return {rect.x + rect.width / 2, rect.y + rect.height / 2};
    }
};

//------------------------------------------------------------------------------------
// Enum EnemyType: Define os diferentes tipos de inimigos.
//------------------------------------------------------------------------------------
enum EnemyType {
    NORMAL, // Inimigo padrão.
    FAST    // Inimigo mais rápido.
};

//------------------------------------------------------------------------------------
// Classe Enemy (Inimigo)
// Representa as bactérias que atacam os dentes.
//------------------------------------------------------------------------------------
class Enemy {
public:
    // Propriedades (membros) da classe Enemy:
    Vector2 position;  // Posição (x, y) do inimigo na tela.
    float speed;       // Velocidade de movimento do inimigo.
    EnemyType type;    // O tipo de inimigo (NORMAL ou FAST).
    Texture2D texture; // A textura (imagem) do inimigo.

    // Construtor da classe Enemy:
    // Chamado quando um novo objeto Enemy é criado.
    Enemy(float x, float y, Texture2D enemyTexture, EnemyType enemyType = NORMAL) {
        position = {x, y};     // Inicializa a posição do inimigo.
        type = enemyType;      // Define o tipo de inimigo.
        texture = enemyTexture; // Armazena a textura.
        // Define a velocidade do inimigo com base no seu tipo.
        switch (type) {
            case NORMAL: speed = 50; break;  // Inimigo normal tem velocidade 50.
            case FAST:   speed = 100; break; // Inimigo rápido tem velocidade 100.
            default:     speed = 50; break;  // Padrão é 50 se o tipo não for reconhecido.
        }
    }

    // Método Update:
    // Atualiza a lógica do inimigo a cada quadro do jogo.
    void Update(float delta, Vector2 target) {
        // 'delta' é o tempo decorrido desde o último quadro, garantindo movimento consistente em diferentes taxas de quadros.
        // 'target' é o ponto para onde o inimigo deve se mover (geralmente o centro de um dente).
        // Calcula o vetor de direção do inimigo para o alvo.
        Vector2 direction = Vector2Normalize(Vector2Subtract(target, position));
        // Atualiza a posição do inimigo, movendo-o na direção do alvo com base na sua velocidade e no tempo 'delta'.
        position = Vector2Add(position, Vector2Scale(direction, speed * delta));
    }

    // Método Draw:
    // Desenha o inimigo na tela.
    void Draw() {
        // Desenha a textura do inimigo. Subtrai metade da largura/altura para centralizar a textura na 'position'.
        DrawTexture(texture, (int)position.x - texture.width / 2, (int)position.y - texture.height / 2, WHITE);
    }
};

//------------------------------------------------------------------------------------
// Classe Projectile (Projétil)
// Representa os "tiros" disparados pelo jogador.
//------------------------------------------------------------------------------------
class Projectile {
public:
    // Propriedades (membros) da classe Projectile:
    Vector2 position;  // Posição (x, y) do projétil.
    Vector2 velocity;  // Velocidade (direção e magnitude) do projétil.
    bool active;       // Indica se o projétil está ativo (visível e em movimento).
    Texture2D texture; // A textura (imagem) do projétil.

    // Construtor da classe Projectile:
    // Chamado quando um novo objeto Projectile é criado.
    Projectile(Vector2 pos, Vector2 dir, Texture2D projectileTexture) {
        position = pos;         // Inicializa a posição do projétil (geralmente a partir do jogador).
        // Calcula a velocidade do projétil: normaliza a direção e a escala por 400 (velocidade do projétil).
        velocity = Vector2Scale(dir, 400);
        active = true;          // O projétil começa ativo.
        texture = projectileTexture; // Armazena a textura.
    }

    // Método Update:
    // Atualiza a lógica do projétil a cada quadro do jogo.
    void Update(float delta) {
        if (!active) return; // Se o projétil não estiver ativo, não faz nada.
        // Atualiza a posição do projétil com base em sua velocidade e no tempo 'delta'.
        position = Vector2Add(position, Vector2Scale(velocity, delta));
        // Verifica se o projétil saiu da tela. Se sim, desativa-o.
        if (position.x < -texture.width || position.x > screenWidth + texture.width ||
            position.y < -texture.height || position.y > screenHeight + texture.height) {
            active = false; // Desativa o projétil para que possa ser removido.
        }
    }

    // Método Draw:
    // Desenha o projétil na tela.
    void Draw() {
        if (active) { // Desenha o projétil apenas se estiver ativo.
            // Desenha a textura do projétil, centralizando-a na 'position'.
            DrawTexture(texture, (int)position.x - texture.width / 2, (int)position.y - texture.height / 2, WHITE);
        }
    }
};

//------------------------------------------------------------------------------------
// Classe Player (Jogador)
// Representa o personagem controlado pelo jogador.
//------------------------------------------------------------------------------------
class Player {
public:
    // Propriedades (membros) da classe Player:
    Vector2 position;  // Posição (x, y) do jogador.
    float speed;       // Velocidade de movimento do jogador.
    // Enum para definir os tipos de tiro disponíveis para o jogador.
    enum ShotType {
        SINGLE_SHOT, // Tiro único.
        TRIPLE_SHOT  // Tiro triplo.
    };
    ShotType currentShotType; // O tipo de tiro atualmente selecionado pelo jogador.
    Texture2D texture;        // A textura (imagem) do jogador.

    // Construtor da classe Player:
    // Chamado quando um novo objeto Player é criado.
    Player(Texture2D playerTexture) {
        // Inicializa a posição do jogador no centro inferior da tela.
        position = {(float)screenWidth / 2.0f - (float)playerTexture.width / 2.0f,
                    (float)screenHeight - playerTexture.height - 50.0f};
        speed = 200;                  // Define a velocidade de movimento do jogador.
        currentShotType = SINGLE_SHOT; // Começa com o tiro único.
        texture = playerTexture;      // Armazena a textura do jogador.
    }

    // Método Update:
    // Atualiza a lógica do jogador a cada quadro do jogo.
    void Update(float delta) {
        // Movimento do jogador com base nas teclas pressionadas.
        if (IsKeyDown(KEY_RIGHT)) position.x += speed * delta; // Move para a direita.
        if (IsKeyDown(KEY_LEFT)) position.x -= speed * delta;  // Move para a esquerda.
        if (IsKeyDown(KEY_UP)) position.y -= speed * delta;    // Move para cima.
        if (IsKeyDown(KEY_DOWN)) position.y += speed * delta;  // Move para baixo.

        // Restringe a posição do jogador para que ele não saia da tela.
        if (position.x < 0) position.x = 0; // Limite esquerdo.
        if (position.x > screenWidth - texture.width) position.x = (float)screenWidth - texture.width; // Limite direito.
        if (position.y < 0) position.y = 0; // Limite superior.
        if (position.y > screenHeight - texture.height) position.y = (float)screenHeight - texture.height; // Limite inferior.

        // Troca o tipo de tiro (SINGLE_SHOT para TRIPLE_SHOT e vice-versa) ao pressionar a tecla 'C'.
        if (IsKeyPressed(KEY_C)) {
            currentShotType = (currentShotType == SINGLE_SHOT) ? TRIPLE_SHOT : SINGLE_SHOT;
        }
    }

    // Método Shoot:
    // Cria e retorna um vetor de projéteis com base no tipo de tiro atual.
    std::vector<Projectile> Shoot() {
        std::vector<Projectile> newShots; // Vetor para armazenar os novos projéteis.
        // Define o ponto de origem do tiro (parte superior central do jogador).
        Vector2 shootOrigin = {position.x + texture.width / 2, position.y};

        // Lógica para diferentes tipos de tiro:
        if (currentShotType == SINGLE_SHOT) {
            // Adiciona um único projétil indo reto para cima.
            newShots.push_back(Projectile(shootOrigin, {0, -1}, globalProjectileTexture));
        } else if (currentShotType == TRIPLE_SHOT) {
            // Adiciona três projéteis: um ligeiramente para a esquerda, um reto e um ligeiramente para a direita.
            newShots.push_back(Projectile(shootOrigin, Vector2Normalize({-0.2f, -1}), globalProjectileTexture));
            newShots.push_back(Projectile(shootOrigin, {0, -1}, globalProjectileTexture));
            newShots.push_back(Projectile(shootOrigin, Vector2Normalize({0.2f, -1}), globalProjectileTexture));
        }
        return newShots; // Retorna os projéteis criados.
    }

    // Método Draw:
    // Desenha o jogador na tela.
    void Draw() {
        DrawTexture(texture, (int)position.x, (int)position.y, WHITE);
    }
};

//------------------------------------------------------------------------------------
// Enum GameScreen: Gerencia os diferentes estados (telas) do jogo.
//------------------------------------------------------------------------------------
enum GameScreen {
    TITLE,    // Tela de título/introdução.
    GAMEPLAY, // Tela principal do jogo.
    GAMEOVER  // Tela de fim de jogo.
};

//------------------------------------------------------------------------------------
// Função principal do jogo (main)
// O ponto de entrada do programa. Aqui a Raylib é inicializada e o loop principal do jogo é executado.
//------------------------------------------------------------------------------------
int main() {
    // --- 1. Inicialização da Janela Raylib e Áudio ---
    InitWindow(screenWidth, screenHeight, "Dente Defensor"); // Cria a janela do jogo com as dimensões e título definidos.
    InitAudioDevice();                                       // INCLUSÃO: Inicializa o dispositivo de áudio. ESSENCIAL para tocar música e efeitos sonoros.
    SetTargetFPS(60);                                        // Define o Frame Rate Per Second (FPS) alvo para 60, garantindo uma taxa de quadros consistente.

    // --- 2. Carregamento de Recursos (Texturas e Músicas) ---
    // O carregamento de recursos é feito uma única vez no início para otimização.

    // Carregamento de Texturas:
    Texture2D playerTexture = LoadTexture("images/player_pixel.png");      // Carrega a imagem do jogador.
    Texture2D enemyTexture = LoadTexture("images/bacteria_pixel.png");      // Carrega a imagem do inimigo (bactéria).
    Texture2D toothTexture = LoadTexture("images/tooth_pixel.png");        // Carrega a imagem do dente.
    globalProjectileTexture = LoadTexture("images/projectile_pixel.png");  // Carrega a imagem do projétil na variável global.
    Texture2D backgroundTexture = LoadTexture("images/mouth_background_pixel.png"); // Carrega a imagem de fundo.

    // Carregamento de Músicas:
    Music introMusic = LoadMusicStream("audio/intro_music.mp3"); // Carrega a música para a tela de título como um stream.
    Music gameMusic = LoadMusicStream("audio/game_music.mp3");   // Carrega a música para a gameplay como um stream.

    // --- 3. Verificação de Carregamento de Recursos ---
    // É crucial verificar se todos os recursos foram carregados com sucesso. Se um recurso falhar, o jogo pode travar.
    if (playerTexture.id == 0) { TraceLog(LOG_ERROR, "Falha ao carregar player_pixel.png"); CloseWindow(); return 1; } // Verifica se a textura do jogador foi carregada. Se id for 0, houve falha.
    if (enemyTexture.id == 0) { TraceLog(LOG_ERROR, "Falha ao carregar bacteria_pixel.png"); CloseWindow(); return 1; } // Verifica textura do inimigo.
    if (toothTexture.id == 0) { TraceLog(LOG_ERROR, "Falha ao carregar tooth_pixel.png"); CloseWindow(); return 1; }     // Verifica textura do dente.
    if (globalProjectileTexture.id == 0) { TraceLog(LOG_ERROR, "Falha ao carregar projectile_pixel.png"); CloseWindow(); return 1; } // Verifica textura do projétil.
    if (backgroundTexture.id == 0) { TraceLog(LOG_ERROR, "Falha ao carregar mouth_background_pixel.png"); CloseWindow(); return 1; } // Verifica textura de fundo.
    if (introMusic.frameCount == 0) { TraceLog(LOG_ERROR, "Falha ao carregar intro_music.mp3"); CloseWindow(); return 1; } // Verifica se a música da introdução foi carregada (frameCount 0 indica falha).
    if (gameMusic.frameCount == 0) { TraceLog(LOG_ERROR, "Falha ao carregar game_music.mp3"); CloseWindow(); return 1; }   // Verifica se a música do jogo foi carregada.


    // --- 4. Inicialização do Estado do Jogo e Objetos ---
    GameScreen currentScreen = TITLE; // O jogo começa na tela de título, definindo o estado inicial do jogo.

    // Inicia a música da tela de título.
    PlayMusicStream(introMusic);    // INCLUSÃO: Começa a tocar a música da introdução.
    SetMusicVolume(introMusic, 0.5f); // Define o volume da música de introdução para 50%.

    Player player = Player(playerTexture);        // Cria um objeto Player, passando a textura carregada.
    std::vector<Tooth> teeth;                     // Vetor para armazenar os objetos Tooth.
    std::vector<Enemy> enemies;                   // Vetor para armazenar os objetos Enemy.
    std::vector<Projectile> shots;                // Vetor para armazenar os objetos Projectile.

    float spawnTimer = 0.0f;           // Contador de tempo para o surgimento de inimigos.
    const float spawnInterval = 2.0f;  // Intervalo de tempo em segundos para o surgimento de novos inimigos.
    int score = 0;                     // Pontuação do jogador.

    // --- 5. Loop Principal do Jogo ---
    // Este é o coração do jogo, onde toda a lógica e o desenho acontecem repetidamente.
    while (!WindowShouldClose()) { // O loop continua enquanto o usuário não tenta fechar a janela.
        float delta = GetFrameTime(); // Obtém o tempo decorrido desde o último quadro. Essencial para movimentos baseados em tempo.

        // INCLUSÃO: Atualiza o stream de música a cada frame para que a música continue tocando.
        // ESSENCIAL para músicas carregadas com LoadMusicStream, pois elas precisam ser atualizadas manualmente.
        UpdateMusicStream(introMusic);
        UpdateMusicStream(gameMusic);

        // --- Lógica de Atualização (Update Logic) ---
        // A lógica do jogo é dividida por telas (estados) para gerenciar o fluxo do jogo.
        switch (currentScreen) {
            case TITLE: {
                // Lógica para a tela de título.
                if (IsKeyPressed(KEY_ENTER)) { // Se a tecla ENTER for pressionada...
                    currentScreen = GAMEPLAY;     // Mude para a tela de gameplay.
                    StopMusicStream(introMusic);  // INCLUSÃO: Para a música da introdução.
                    PlayMusicStream(gameMusic);   // INCLUSÃO: Começa a tocar a música do jogo.
                    SetMusicVolume(gameMusic, 0.3f); // Define o volume da música do jogo para 30%.

                    // Reinicializa todos os elementos do jogo para uma nova partida.
                    player = Player(playerTexture); // Recria o jogador.
                    teeth.clear(); // Limpa o vetor de dentes.
                    for (int i = 0; i < 8; i++) { // Cria 8 novos dentes em posições específicas.
                        float toothX = 80.0f + i * (toothTexture.width + 10.0f); // Calcula a posição X do dente.
                        float toothY = 400.0f; // Posição Y fixa para os dentes.
                        teeth.push_back(Tooth(toothX, toothY, toothTexture)); // Adiciona um novo dente ao vetor.
                    }
                    enemies.clear(); // Limpa o vetor de inimigos.
                    shots.clear();   // Limpa o vetor de projéteis.
                    score = 0;       // Zera a pontuação.
                    spawnTimer = 0;  // Zera o contador de surgimento de inimigos.
                }
            } break; // Fim do case TITLE.

            case GAMEPLAY: {
                // Lógica para a tela de jogo.

                // Lógica de surgimento de inimigos:
                spawnTimer += delta; // Incrementa o contador de tempo de surgimento.
                if (spawnTimer >= spawnInterval) { // Se o tempo de surgimento for atingido...
                    // Gera uma posição X aleatória para o inimigo no topo da tela.
                    float randomX = (float)GetRandomValue(enemyTexture.width / 2, screenWidth - enemyTexture.width / 2);
                    if (GetRandomValue(0, 100) < 25) { // 25% de chance de surgir um inimigo FAST.
                        enemies.push_back(Enemy(randomX, 0 - enemyTexture.height / 2, enemyTexture, FAST));
                    } else { // 75% de chance de surgir um inimigo NORMAL.
                        enemies.push_back(Enemy(randomX, 0 - enemyTexture.height / 2, enemyTexture, NORMAL));
                    }
                    spawnTimer = 0; // Reseta o contador de tempo de surgimento.
                }

                player.Update(delta); // Atualiza a lógica do jogador (movimento, troca de tiro).

                // Lógica de tiro do jogador:
                if (IsKeyPressed(KEY_SPACE)) { // Se a tecla ESPAÇO for pressionada...
                    std::vector<Projectile> newShots = player.Shoot(); // Obtém os novos projéteis disparados pelo jogador.
                    shots.insert(shots.end(), newShots.begin(), newShots.end()); // Adiciona os novos projéteis ao vetor principal de projéteis.
                }

                // Atualiza todos os projéteis ativos.
                for (auto &shot : shots) {
                    shot.Update(delta);
                }
                // Remove projéteis inativos (que saíram da tela ou colidiram) do vetor.
                // std::remove_if move os elementos a serem removidos para o final do vetor, e erase os remove fisicamente.
                shots.erase(std::remove_if(shots.begin(), shots.end(),
                                             [](const Projectile& s){ return !s.active; }),
                                        shots.end());

                // Atualiza a lógica de cada inimigo:
                for (auto &enemy : enemies) {
                    Tooth* targetTooth = nullptr; // Ponteiro para o dente que será o alvo do inimigo.
                    int minHealth = std::numeric_limits<int>::max(); // Inicializa com o valor máximo possível de int para encontrar o dente com menor saúde.

                    // Encontra o dente com a menor saúde (o mais danificado) para o inimigo atacar.
                    for (auto &tooth : teeth) {
                        if (!tooth.IsDestroyed()) { // Se o dente não estiver destruído...
                            if (tooth.health < minHealth) { // E se a saúde dele for menor que a saúde mínima encontrada até agora...
                                minHealth = tooth.health;    // Atualiza a saúde mínima.
                                targetTooth = &tooth;        // Define este dente como o novo alvo.
                            }
                        }
                    }
                    if (targetTooth) { // Se um dente alvo foi encontrado...
                        enemy.Update(delta, targetTooth->GetCenter()); // O inimigo se move em direção ao centro desse dente.
                    } else { // Se não houver dentes ativos (todos destruídos), os inimigos continuam descendo.
                        enemy.Update(delta, {enemy.position.x, (float)screenHeight + 100}); // Move para fora da tela.
                    }
                }

                // Detecção de colisão entre projéteis e inimigos:
                for (auto &shot : shots) {
                    if (!shot.active) continue; // Pula projéteis inativos.
                    // Itera sobre os inimigos de trás para frente para permitir a remoção segura durante a iteração.
                    for (int i = enemies.size() - 1; i >= 0; --i) {
                        Enemy& enemy = enemies.at(i); // Pega o inimigo atual.
                        // Verifica a colisão entre o projétil (como círculo) e o inimigo (como círculo).
                        if (CheckCollisionCircles(shot.position, globalProjectileTexture.width / 2.0f,
                                                  enemy.position, enemy.texture.width / 2.0f)) {
                            enemy.position = {-1000, -1000}; // Move o inimigo para fora da tela para ser removido posteriormente.
                            shot.active = false;             // Desativa o projétil.
                            score += 10;                     // Aumenta a pontuação.
                            break;                           // Um projétil só atinge um inimigo por vez, então sai deste loop interno.
                        }
                    }
                }
                // Remove inimigos que foram "destruídos" (movidos para fora da tela) do vetor.
                enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                             [](const Enemy& e){ return e.position.x < -500; }), // Condição para remoção: inimigo fora da tela.
                                         enemies.end());

                // Detecção de colisão entre inimigos e dentes:
                for (auto &enemy : enemies) {
                    for (auto &tooth : teeth) {
                        if (tooth.IsDestroyed()) continue; // Pula dentes já destruídos.
                        // Verifica a colisão entre o inimigo (como círculo) e o dente (como retângulo).
                        if (CheckCollisionCircleRec(enemy.position, enemy.texture.width / 2.0f, tooth.rect)) {
                            tooth.Damage();                  // Causa dano ao dente.
                            enemy.position = {-1000, -1000}; // Move o inimigo para fora da tela para remoção.
                            break;                           // Um inimigo só atinge um dente por vez.
                        }
                    }
                }

                // Verifica a condição de Game Over:
                int lostTeethCount = 0;
                for (auto &tooth : teeth) {
                    if (tooth.IsDestroyed()) {
                        lostTeethCount++; // Conta quantos dentes foram destruídos.
                    }
                }
                if (lostTeethCount >= 3) { // Se 3 ou mais dentes forem perdidos...
                    currentScreen = GAMEOVER;     // Mude para a tela de Game Over.
                    StopMusicStream(gameMusic);   // INCLUSÃO: Para a música do jogo ao ir para Game Over.
                }
            } break; // Fim do case GAMEPLAY.

            case GAMEOVER: {
                // Lógica para a tela de Game Over.
                if (IsKeyPressed(KEY_R)) { // Se a tecla 'R' for pressionada...
                    currentScreen = GAMEPLAY;     // Mude para a tela de gameplay (reinicia o jogo).
                    // Opcional: Parar música de Game Over aqui se houver uma.
                    PlayMusicStream(gameMusic); // INCLUSÃO: Reinicia a música do jogo ao recomeçar.
                    SetMusicVolume(gameMusic, 0.3f); // Mantém o volume.

                    // Reinicializa todos os elementos do jogo para uma nova partida, similar ao que acontece na tela de título.
                    player = Player(playerTexture);
                    teeth.clear();
                    for (int i = 0; i < 8; i++) {
                        float toothX = 80.0f + i * (toothTexture.width + 10.0f);
                        float toothY = 400.0f;
                        teeth.push_back(Tooth(toothX, toothY, toothTexture));
                    }
                    enemies.clear();
                    shots.clear();
                    score = 0;
                    spawnTimer = 0;
                }
            } break; // Fim do case GAMEOVER.

            default: break; // Caso padrão, não faz nada.
        }

        // --- Seção de Desenho (Drawing) ---
        // Tudo o que é desenhado na tela deve estar entre BeginDrawing() e EndDrawing().
        BeginDrawing();         // Inicia o modo de desenho da Raylib.
        ClearBackground(RAYWHITE); // Limpa a tela com uma cor de fundo (branco claro).

        DrawTexture(backgroundTexture, 0, 0, WHITE); // Desenha a imagem de fundo em (0,0) com matiz branco.

        // A lógica de desenho também é dividida por telas.
        switch (currentScreen) {
            case TITLE: {
                // Desenha os textos da tela de título.
                // MeasureText é usado para centralizar o texto.
                DrawText("DENTE DEFENSOR", screenWidth / 2 - MeasureText("DENTE DEFENSOR", 50) / 2, screenHeight / 2 - 80, 50, BLUE);
                DrawText("Pressione ENTER para comecar", screenWidth / 2 - MeasureText("Pressione ENTER para comecar", 20) / 2, screenHeight / 2, 20, DARKGRAY);
            } break;

            case GAMEPLAY: {
                // Desenha todos os elementos do jogo.
                for (auto &tooth : teeth) tooth.Draw(); // Desenha cada dente.
                for (auto &enemy : enemies) {
                    if (enemy.position.x > -500) { // Desenha o inimigo apenas se ele não foi "removido" (movido para -1000).
                        enemy.Draw();
                    }
                }
                for (auto &shot : shots) shot.Draw(); // Desenha cada projétil.
                player.Draw(); // Desenha o jogador por último para que ele apareça por cima de outros elementos.

                // Desenha a interface do usuário (HUD).
                DrawText("Proteja os dentes das bacterias!", 10, 10, 20, DARKGRAY);
                DrawText(TextFormat("Pontuacao: %d", score), 10, 40, 20, DARKGRAY); // Exibe a pontuação.
                DrawText(TextFormat("Tiro: %s (C para mudar)", (player.currentShotType == Player::SINGLE_SHOT ? "Unico" : "Triplo")), 10, 70, 20, DARKGRAY); // Exibe o tipo de tiro e instrução.
            } break;

            case GAMEOVER: {
                // Desenha os textos da tela de Game Over.
                DrawText("FIM DE JOGO", screenWidth / 2 - MeasureText("FIM DE JOGO", 30) / 2, screenHeight / 2 - 40, 30, RED);
                DrawText(TextFormat("Pontuacao final: %d", score), screenWidth / 2 - MeasureText(TextFormat("Pontuacao final: %d", score), 20) / 2, screenHeight / 2, 20, DARKGRAY);
                DrawText("Pressione R para reiniciar", screenWidth / 2 - MeasureText("Pressione R para reiniciar", 20) / 2, screenHeight / 2 + 40, 20, DARKGRAY);
            } break;
            default: break;
        }

        EndDrawing(); // Finaliza o modo de desenho, mostrando tudo o que foi desenhado na tela.
    }

    // --- 6. Limpeza de Recursos e Encerramento ---
    // É crucial descarregar todos os recursos carregados para evitar vazamentos de memória e garantir o fechamento limpo do programa.
    UnloadTexture(playerTexture);          // Descarrega a textura do jogador.
    UnloadTexture(enemyTexture);           // Descarrega a textura do inimigo.
    UnloadTexture(toothTexture);           // Descarrega a textura do dente.
    UnloadTexture(globalProjectileTexture); // Descarrega a textura do projétil global.
    UnloadTexture(backgroundTexture);      // Descarrega a textura de fundo.

    // Descarrega as músicas da memória.
    UnloadMusicStream(introMusic); // INCLUSÃO: Descarrega a música da introdução.
    UnloadMusicStream(gameMusic);  // INCLUSÃO: Descarrega a música do jogo.

    CloseAudioDevice(); // INCLUSÃO: Encerra o dispositivo de áudio. ESSENCIAL para liberar recursos de áudio.
    CloseWindow();      // Fecha a janela do jogo e libera todos os recursos da Raylib.

    return 0; // Indica que o programa terminou com sucesso.
}
