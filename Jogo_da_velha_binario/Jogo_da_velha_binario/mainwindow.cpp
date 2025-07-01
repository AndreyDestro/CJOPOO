#include "mainwindow.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>

// Construtor: chamado quando a janela é criada
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    initializeGame();
}

// Destrutor: chamado quando a janela é fechada
MainWindow::~MainWindow()
{
}

// Configura todos os elementos visuais da interface
void MainWindow::setupUi()
{
    // Define a imagem de fundo da janela, buscando nos recursos
    this->setStyleSheet("QMainWindow { border-image: url(:/imagens/background_pixel_color.png); }");
    this->setWindowTitle("Jogo da Velha Binário");
    this->setFixedSize(400, 500);

    // Cria e define o widget central que conterá todos os outros
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Layout principal para organizar os widgets verticalmente
    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);

    // Cria e estiliza o rótulo de status
    m_statusLabel = new QLabel(this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "color: #00FF00; font-size: 20px; font-family: 'Press Start 2P', Courier; "
        "font-weight: bold; background-color: rgba(0, 0, 0, 0.5); "
        "border-radius: 5px; padding: 5px;"
        );
    mainLayout->addWidget(m_statusLabel);

    // Cria o layout em grade para o tabuleiro
    QGridLayout *boardLayout = new QGridLayout();
    boardLayout->setSpacing(10);
    m_boardButtons.resize(9);

    // Loop para criar os 9 botões do tabuleiro
    for (int i = 0; i < 9; ++i) {
        m_boardButtons[i] = new QPushButton(this);
        m_boardButtons[i]->setFixedSize(100, 100);
        m_boardButtons[i]->setIconSize(QSize(80, 80));
        m_boardButtons[i]->setStyleSheet(
            "QPushButton { background-color: rgba(0, 50, 0, 0.7); border: 2px solid #00FF00; border-radius: 10px; }"
            "QPushButton:hover { background-color: rgba(0, 80, 0, 0.8); }"
            );
        connect(m_boardButtons[i], &QPushButton::clicked, this, &MainWindow::handleBoardButtonClick);
        boardLayout->addWidget(m_boardButtons[i], i / 3, i % 3);
    }
    mainLayout->addLayout(boardLayout);

    // Cria e estiliza o botão de reiniciar
    m_resetButton = new QPushButton("Reiniciar Jogo", this);
    m_resetButton->setStyleSheet(
        "QPushButton { color: #00FF00; background-color: #333; border: 1px solid #00FF00; "
        "padding: 10px; font-family: 'Press Start 2P', Courier; font-size: 16px; }"
        "QPushButton:hover { background-color: #555; }"
        );
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::resetGame);
    mainLayout->addWidget(m_resetButton, 0, Qt::AlignCenter);
}

// Prepara as variáveis para uma nova partida
void MainWindow::initializeGame()
{
    m_board.fill(0, 9);
    m_currentPlayer = 1;
    m_gameIsOver = false;

    for(QPushButton* button : m_boardButtons) {
        button->setIcon(QIcon());
    }
    setBoardButtonsEnabled(true);
    updateStatusLabel();
}

// Chamado quando um botão do tabuleiro é clicado
void MainWindow::handleBoardButtonClick()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton || m_gameIsOver) return;

    int index = m_boardButtons.indexOf(clickedButton);
    if (m_board[index] != 0) return;

    m_board[index] = m_currentPlayer;
    QIcon icon(m_currentPlayer == 1 ? ":/imagens/icon_1.png" : ":/imagens/icon_0.png");
    clickedButton->setIcon(icon);

    checkForWinner();

    if (!m_gameIsOver) {
        m_currentPlayer = (m_currentPlayer == 1) ? 2 : 1;
        updateStatusLabel();
    }
}

// Verifica se há um vencedor ou empate
void MainWindow::checkForWinner()
{
    const int winPatterns[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Linhas
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Colunas
        {0, 4, 8}, {2, 4, 6}             // Diagonais
    };

    for (int i = 0; i < 8; ++i) {
        int a = winPatterns[i][0], b = winPatterns[i][1], c = winPatterns[i][2];
        if (m_board[a] != 0 && m_board[a] == m_board[b] && m_board[a] == m_board[c]) {
            m_statusLabel->setText(QString("JOGADOR %1 VENCEU!").arg(m_currentPlayer));
            m_gameIsOver = true;
            setBoardButtonsEnabled(false);
            return;
        }
    }

    if (!m_board.contains(0)) {
        m_statusLabel->setText("EMPATE! (DEU VELHA)");
        m_gameIsOver = true;
    }
}

// Atualiza o texto do rótulo de status
void MainWindow::updateStatusLabel()
{
    m_statusLabel->setText(QString("Vez do Jogador %1").arg(m_currentPlayer));
}

// Habilita ou desabilita os botões do tabuleiro
void MainWindow::setBoardButtonsEnabled(bool enabled)
{
    for (QPushButton *button : m_boardButtons) {
        button->setEnabled(enabled);
    }
}

// Chamado quando o botão de reiniciar é clicado
void MainWindow::resetGame()
{
    initializeGame();
}
