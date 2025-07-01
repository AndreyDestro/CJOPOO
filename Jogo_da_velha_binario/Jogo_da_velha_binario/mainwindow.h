// --- GUARDAS DE INCLUSÃO (INCLUDE GUARDS) ---
// Estas duas primeiras linhas e a última linha do arquivo (#endif) são um truque
// essencial em C++ para evitar que o mesmo arquivo de cabeçalho seja incluído mais de
// uma vez durante a compilação, o que causaria erros de "definição duplicada".
// Ele basicamente diz: "Se o símbolo MAINWINDOW_H ainda não foi definido..."
#ifndef MAINWINDOW_H
// "...então defina-o agora e processe todo o código abaixo."
#define MAINWINDOW_H

// --- INCLUSÃO DE FERRAMENTAS QT ---
// Aqui, importamos as "plantas" de componentes padrão do Qt que usaremos
// para construir nossa janela. É como pegar as ferramentas da caixa antes de começar.
#include <QMainWindow>   // A planta de uma janela principal padrão (com menus, barras, etc.).
#include <QPushButton>   // A planta de um botão clicável.
#include <QLabel>        // A planta de um rótulo para exibir texto ou imagens.
#include <QVector>       // Uma "lista" ou "vetor" flexível do Qt, muito útil para guardar coleções de itens.

// --- A DECLARAÇÃO DA CLASSE ---
// Aqui começa a planta da nossa própria janela.
// "class MainWindow" diz que estamos criando uma nova classe chamada MainWindow.
// ": public QMainWindow" significa que nossa classe herda todas as características e
// funcionalidades de uma QMainWindow. Isso é Herança. Nossa janela é uma "versão especializada"
// de uma janela padrão.
class MainWindow : public QMainWindow
{
    // --- A MACRO MÁGICA DO QT ---
    // Q_OBJECT é uma macro especial e obrigatória para qualquer classe que queira usar
    // o sistema de "Sinais e Slots" do Qt, que é a base da interatividade (ex: um clique
    // de botão chamando uma função). Ele dá "superpoderes" à nossa classe.
    Q_OBJECT

    // --- SEÇÃO PÚBLICA ---
    // "public" significa que estas partes da nossa classe podem ser acessadas de fora dela
    // (por exemplo, pelo arquivo main.cpp).
public:
    // O Construtor: é a função especial chamada automaticamente quando um objeto MainWindow é criado.
    // É o "manual de montagem" da nossa janela.
    MainWindow(QWidget *parent = nullptr);

    // O Destrutor: é a função chamada quando o objeto MainWindow é destruído (ex: ao fechar o app).
    // É o "manual de desmontagem e limpeza".
    ~MainWindow();

    // --- SEÇÃO DE SLOTS PRIVADOS ---
    // "slots" são funções especiais do Qt projetadas para "receber" sinais.
    // "private" significa que só a própria classe MainWindow pode chamar essas funções diretamente.
private slots:
    // Este slot será conectado ao sinal de clique de TODOS os 9 botões do tabuleiro.
    void handleBoardButtonClick();

    // Este slot será conectado ao sinal de clique do botão "Reiniciar Jogo".
    void resetGame();

    // --- SEÇÃO PRIVADA ---
    // "private" aqui agrupa funções e variáveis que são para uso interno da classe.
    // Nenhuma outra parte do programa precisa saber sobre elas. Isso é Encapsulamento.
private:
    // --- Funções Auxiliares (As "tarefas internas") ---
    void setupUi();              // Monta e estiliza todos os widgets da interface.
    void initializeGame();       // Prepara as variáveis para o início de uma nova partida.
    void checkForWinner();       // Contém a lógica que verifica se alguém ganhou ou se deu empate.
    void updateStatusLabel();    // Atualiza o texto do rótulo de mensagens (ex: "Vez do Jogador X").
    void setBoardButtonsEnabled(bool enabled); // Habilita ou desabilita todos os botões do tabuleiro de uma vez.

    // --- Variáveis de Membro (Os "atributos" ou "memória" da janela) ---

    // Lógica do Jogo:
    // Um vetor de inteiros que representa o estado do tabuleiro.
    // Ex: m_board[0] = 0 (vazio), m_board[1] = 1 (jogador 1), etc.
    QVector<int> m_board;

    // Elementos da Interface (UI):
    // Ponteiros para os widgets que criaremos. Precisamos guardá-los para poder manipulá-los depois.
    QWidget *m_centralWidget;                   // O widget principal que serve de "container" para os outros.
    QVector<QPushButton*> m_boardButtons;       // Um vetor para guardar os ponteiros dos 9 botões do tabuleiro.
    QLabel *m_statusLabel;                      // O ponteiro para o nosso rótulo de status.
    QPushButton *m_resetButton;                 // O ponteiro para o nosso botão de reiniciar.

    // Estado do Jogo:
    int m_currentPlayer;      // Guarda quem é o jogador da vez (1 ou 2).
    bool m_gameIsOver;        // Uma "bandeira" (flag) que nos diz se a partida atual já terminou.
};

#endif // Fim da guarda de inclusão MAINWINDOW_H
