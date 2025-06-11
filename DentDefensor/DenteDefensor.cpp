// jogo.cpp
// Implementa a l�gica dos m�todos declarados em jogo.h.

#include "jogo.h"
#include <iostream> // Para mensagens de depura��o (opcional).

// --- Construtor ---
Jogo::Jogo(int largura, int altura) :
    larguraTela(largura),
    alturaTela(altura),
    estadoAtual(EstadoJogo::MENU),
    jogador(
        {(float)largura / 2.0f, (float)altura - 50.0f}, // Posi��o inicial do jogador
        {100.0f, 30.0f}                                 // Tamanho do jogador
    ),
    temporizadorSpawnOrbe(0.0f),
    intervaloSpawnOrbe(1.5f), // Novo orbe a cada 1.5 segundos inicialmente
    pontuacao(0)
{
    // A fonte � carregada em Iniciar().
}

// --- Iniciar ---
void Jogo::Iniciar() {
    fonte = GetFontDefault(); // Carrega a fonte padr�o do Raylib.
                              // Para uma fonte customizada: fonte = LoadFont("caminho/fonte.ttf");
    estadoAtual = EstadoJogo::MENU;
    std::cout << "Jogo iniciado. Estado: MENU" << std::endl;
}

// --- Finalizar ---
void Jogo::Finalizar() {
    // Se uma fonte customizada foi carregada, descarregue-a aqui:
    // if (fonte.texture.id != GetFontDefault().texture.id) UnloadFont(fonte);
    std::cout << "Jogo finalizado." << std::endl;
}

// --- ProcessarInput ---
void Jogo::ProcessarInput() {
    switch (estadoAtual) {
        case EstadoJogo::MENU:
            ProcessarInputMenu();
            break;
        case EstadoJogo::JOGANDO:
            jogador.ProcessarInput(larguraTela); // Passa a largura da tela para limites
            break;
        case EstadoJogo::FIM_DE_JOGO:
            ProcessarInputFimDeJogo();
            break;
    }
}

// --- Atualizar ---
void Jogo::Atualizar() {
    float deltaTime = GetFrameTime(); // Tempo passado desde o �ltimo frame.

    switch (estadoAtual) {
        case EstadoJogo::MENU:
            AtualizarMenu();
            break;
        case EstadoJogo::JOGANDO:
            AtualizarJogando(deltaTime); // Passa deltaTime para a l�gica do jogo.
            break;
        case EstadoJogo::FIM_DE_JOGO:
            AtualizarFimDeJogo();
            break;
    }
}

// --- Desenhar ---
void Jogo::Desenhar() {
    switch (estadoAtual) {
        case EstadoJogo::MENU:
            DesenharMenu();
            break;
        case EstadoJogo::JOGANDO:
            DesenharJogando();
            break;
        case EstadoJogo::FIM_DE_JOGO:
            DesenharFimDeJogo();
            break;
    }
}

// --- SpawnsOrbe ---
void Jogo::SpawnsOrbe() {
    // Define uma posi��o X aleat�ria para o novo orbe.
    float posX = static_cast<float>(GetRandomValue(20, larguraTela - 20)); // Margem
    float posY = -20.0f; // Come�a um pouco acima da tela.
    float raio = 15.0f;

    // Escolhe uma cor aleat�ria para o orbe.
    int corAleatoriaId = GetRandomValue(0, 2); // 0: Vermelho, 1: Verde, 2: Azul
    Color corVisual;
    TipoCor tipoCorLogica;

    if (corAleatoriaId == 0) {
        corVisual = RED;
        tipoCorLogica = TipoCor::VERMELHO;
    } else if (corAleatoriaId == 1) {
        corVisual = GREEN;
        tipoCorLogica = TipoCor::VERDE;
    } else { // corAleatoriaId == 2
        corVisual = BLUE;
        tipoCorLogica = TipoCor::AZUL;
    }

    // Cria o novo orbe e o adiciona � lista.
    orbes.emplace_back(Orbe({posX, posY}, raio, corVisual, tipoCorLogica));
}

// --- AtualizarJogando ---
void Jogo::AtualizarJogando(float deltaTime) {
    // L�gica de spawn de orbes.
    temporizadorSpawnOrbe += deltaTime;
    if (temporizadorSpawnOrbe >= intervaloSpawnOrbe) {
        SpawnsOrbe();
        temporizadorSpawnOrbe = 0.0f; // Reseta o temporizador.

        // (Opcional) Aumenta a dificuldade diminuindo o intervalo de spawn.
        if (intervaloSpawnOrbe > 0.5f) { // Limite m�nimo para o intervalo.
            intervaloSpawnOrbe *= 0.99f; // Diminui 1% a cada spawn.
        }
    }

    // Atualiza a posi��o de cada orbe.
    for (size_t i = 0; i < orbes.size(); ++i) {
        orbes[i].Atualizar(deltaTime); // Passa deltaTime para o m�todo Atualizar do Orbe.
    }

    VerificarColisoes();
    LimparOrbesInativos();

    // Condi��o de Fim de Jogo (exemplo: pontua��o negativa).
    if (pontuacao < -20) {
        estadoAtual = EstadoJogo::FIM_DE_JOGO;
        std::cout << "Fim de jogo! Pontua��o muito baixa." << std::endl;
    }
}

// --- VerificarColisoes ---
void Jogo::VerificarColisoes() {
    Rectangle retanguloJogador = jogador.GetRetangulo();

    for (size_t i = 0; i < orbes.size(); ++i) {
        if (orbes[i].estaAtivo) {
            // CheckCollisionCircleRec verifica colis�o entre c�rculo e ret�ngulo.
            if (CheckCollisionCircleRec(orbes[i].posicao, orbes[i].raio, retanguloJogador)) {
                if (orbes[i].tipo == jogador.GetCorAlvo()) {
                    pontuacao += 10;
                    std::cout << "Acertou! Cor Orbe: " << static_cast<int>(orbes[i].tipo)
                              << ". Pontos: " << pontuacao << std::endl;
                } else {
                    pontuacao -= 5;
                    std::cout << "Errou! Cor Orbe: " << static_cast<int>(orbes[i].tipo)
                              << ", Cor Jogador: " << static_cast<int>(jogador.GetCorAlvo())
                              << ". Pontos: " << pontuacao << std::endl;
                }
                orbes[i].estaAtivo = false; // Orbe coletado (ou errado) se torna inativo.
            }
        }
    }
}

// --- LimparOrbesInativos ---
void Jogo::LimparOrbesInativos() {
    for (int i = orbes.size() - 1; i >= 0; --i) {
        if (!orbes[i].estaAtivo || orbes[i].posicao.y - orbes[i].raio > alturaTela) {
            // L�gica para penalizar se um orbe da cor certa passou (opcional):
            /*
            if (orbes[i].estaAtivo && orbes[i].tipo == jogador.GetCorAlvo()){
                pontuacao -= 2; // Exemplo de penalidade
                std::cout << "Orbe da cor certa perdido! Pontos: " << pontuacao << std::endl;
            }
            */
            orbes.erase(orbes.begin() + i); // Remove o orbe da lista.
        }
    }
}

// --- DesenharJogando ---
void Jogo::DesenharJogando() {
    jogador.Desenhar();

    for (const auto& orbe : orbes) { // Usa const auto& pois orbe.Desenhar() � const
        if (orbe.estaAtivo) {
            orbe.Desenhar();
        }
    }

    std::string textoPontuacao = "Pontos: " + std::to_string(pontuacao);
    DrawTextEx(fonte, textoPontuacao.c_str(), Vector2{10, 10}, 24, 2, WHITE);

    std::string textoCorAlvo = "Coletar: ";
    Color corVisualAlvo = WHITE; // Cor do texto que indica o alvo
    switch(jogador.GetCorAlvo()){
        case TipoCor::VERMELHO: textoCorAlvo += "VERMELHO"; corVisualAlvo = RED; break;
        case TipoCor::VERDE:    textoCorAlvo += "VERDE";    corVisualAlvo = GREEN; break;
        case TipoCor::AZUL:     textoCorAlvo += "AZUL";     corVisualAlvo = BLUE; break;
    }
    Vector2 tamTextoCorAlvo = MeasureTextEx(fonte, textoCorAlvo.c_str(), 20, 1);
    DrawTextEx(fonte, textoCorAlvo.c_str(), Vector2{(float)larguraTela - tamTextoCorAlvo.x - 10, 10}, 20, 1, corVisualAlvo);
}

// --- ResetarPartida ---
void Jogo::ResetarPartida() {
    pontuacao = 0;
    orbes.clear();
    temporizadorSpawnOrbe = 0.0f;
    intervaloSpawnOrbe = 1.5f;
    jogador.Resetar({(float)larguraTela / 2.0f, (float)alturaTela - 50.0f});
    std::cout << "Partida resetada." << std::endl;
}

// --- M�todos de Menu e Fim de Jogo ---
void Jogo::ProcessarInputMenu() {
    if (IsKeyPressed(KEY_ENTER)) {
        estadoAtual = EstadoJogo::JOGANDO;
        ResetarPartida(); // Prepara para uma nova partida.
        std::cout << "Input Menu: ENTER. Mudando para JOGANDO." << std::endl;
    }
}

void Jogo::AtualizarMenu() {
    // Nenhuma l�gica de atualiza��o complexa para o menu neste exemplo.
}

void Jogo::DesenharMenu() {
    ClearBackground(DARKBLUE);
    const char* titulo = "Coletor de Orbes Coloridas";
    Vector2 tamTitulo = MeasureTextEx(fonte, titulo, 40, 2); // Fonte, texto, tamanho, espa�amento
    DrawTextEx(fonte, titulo, Vector2{(larguraTela - tamTitulo.x) / 2.0f, alturaTela / 3.0f}, 40, 2, YELLOW);

    const char* instrucao = "Pressione [ENTER] para Iniciar";
    Vector2 tamInstrucao = MeasureTextEx(fonte, instrucao, 20, 1);
    DrawTextEx(fonte, instrucao, Vector2{(larguraTela - tamInstrucao.x) / 2.0f, alturaTela / 2.0f + 20}, 20, 1, WHITE);

    const char* controles = "Setas <- -> para Mover | [ESPACO] para Mudar Cor";
    Vector2 tamControles = MeasureTextEx(fonte, controles, 18, 1);
    DrawTextEx(fonte, controles, Vector2{(larguraTela - tamControles.x) / 2.0f, alturaTela * 0.7f}, 18, 1, LIGHTGRAY);
}

void Jogo::ProcessarInputFimDeJogo() {
    if (IsKeyPressed(KEY_ENTER)) {
        estadoAtual = EstadoJogo::MENU; // Volta para o menu.
        std::cout << "Input Fim: ENTER. Mudando para MENU." << std::endl;
    }
}

void Jogo::AtualizarFimDeJogo() {
    // Nenhuma l�gica de atualiza��o complexa.
}

void Jogo::DesenharFimDeJogo() {
    ClearBackground(BLACK);
    const char* gameOverText = "FIM DE JOGO";
    Vector2 tamGameOver = MeasureTextEx(fonte, gameOverText, 50, 2);
    DrawTextEx(fonte, gameOverText, Vector2{(larguraTela - tamGameOver.x) / 2.0f, alturaTela / 3.0f}, 50, 2, RED);

    std::string textoPontuacaoFinal = "Pontuacao Final: " + std::to_string(pontuacao);
    Vector2 tamPontFinal = MeasureTextEx(fonte, textoPontuacaoFinal.c_str(), 30, 1);
    DrawTextEx(fonte, textoPontuacaoFinal.c_str(), Vector2{(larguraTela - tamPontFinal.x) / 2.0f, alturaTela / 2.0f}, 30, 1, WHITE);

    const char* instrucao = "Pressione [ENTER] para Voltar ao Menu";
    Vector2 tamInstrucao = MeasureTextEx(fonte, instrucao, 20, 1);
    DrawTextEx(fonte, instrucao, Vector2{(larguraTela - tamInstrucao.x) / 2.0f, alturaTela * 0.7f}, 20, 1, LIGHTGRAY);
}
