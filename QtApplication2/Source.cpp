#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>

#include <iostream>

using namespace std;

class TicTacToe : public QWidget {
private:
    int size;
    int winSize;
    QGridLayout* layout;
    QMessageBox msgBox;
    QVector<QVector<char>> board;
    char currentPlayer;
    bool gameEnded;
public:
    TicTacToe(int n, int m) : size(n), currentPlayer('X'), gameEnded(false), winSize(m) {
        layout = new QGridLayout();
        board.resize(size, QVector<char>(size, ' '));

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                QPushButton* button = new QPushButton("");
                button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                layout->addWidget(button, i, j);
                connect(button, &QPushButton::clicked, this, [=]() { makeMove(button); });
            }
        }
        setLayout(layout);
    }


    void makeMove(QPushButton* button) {
        int row, col;
        if (!gameEnded && buttonToIndex(button, row, col)&& board[row][col] == ' ') {
            button->setText(QString(currentPlayer));
            board[row][col] = currentPlayer;

            if (checkWin(currentPlayer)) {
                endGame(QString(currentPlayer) + " wins!");
            }
            else if (isBoardFull()) {
                endGame("It's a draw!");
            }
            else {
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                if (currentPlayer == 'O') {
                    makeComputerMove();
                }
            }
        }
    }


    int evel(char player) {

        if (checkWin(player)) {
            return 100;
        }

        char opponent = (player == 'X') ? 'O' : 'X';
        if (checkWin(opponent)) {
            return -100;
        }

        if (isBoardFull()) {
            return 0;
        }
        int playerRowCount = 0;
        int opponentRowCount = 0;

        // Row
        for (int row = 0; row < size; row++) {
            for (int col = 0; col <= size - winSize; col++) {
                int playerCount = 0;
                int opponentCount = 0;
                for (int i = 0; i < winSize; i++) {
                    if (board[row][col + i] == player) {
                        playerCount++;
                    }
                    else if (board[row][col + i] == opponent) {
                        opponentCount++;
                    }
                }
                if (playerCount > 0 && opponentCount == 0) {
                    playerRowCount += playerCount;
                }
                else if (opponentCount > 0 && playerCount == 0) {
                    opponentRowCount += opponentCount;
                }
            }
        }

        // Col
        for (int col = 0; col < size; col++) {
            for (int row = 0; row <= size - winSize; row++) {
                int playerCount = 0;
                int opponentCount = 0;
                for (int i = 0; i < winSize; i++) {
                    if (board[row + i][col] == player) {
                        playerCount++;
                    }
                    else if (board[row + i][col] == opponent) {
                        opponentCount++;
                    }
                }
                if (playerCount > 0 && opponentCount == 0) {
                    playerRowCount += playerCount;
                }
                else if (opponentCount > 0 && playerCount == 0) {
                    opponentRowCount += opponentCount;
                }
            }
        }

        return playerRowCount - opponentRowCount;

    }

    void makeComputerMove() {
        int bestScore = INT_MIN;
        int bestRow = -1;
        int bestCol = -1;

        int centerRow = size / 2;
        int centerCol = size / 2;


       if (board[centerRow][centerCol] == ' ') {
            bestRow = centerRow;
            bestCol = centerCol;
        }
        else {

            for (int row = 0; row < size; row++) {
                for (int col = 0; col < size; col++) {
                    if (board[row][col] == ' ') {
                        board[row][col] = currentPlayer;
                        int score = minimaxAlphaBeta(board, 4, INT_MIN, INT_MAX, false);
                        qDebug() << "Move: (" << row << ", " << col << "), Score: " << score;
                        board[row][col] = ' ';

                        if (score > bestScore) {
                            bestScore = score;
                            bestRow = row;
                            bestCol = col;
                        }
                    }
                }
            }
        }
        

        

        if (bestRow != -1 && bestCol != -1) {
            QPushButton* button = static_cast<QPushButton*>(layout->itemAtPosition(bestRow, bestCol)->widget());
            makeMove(button);
        }
    }

    int minimaxAlphaBeta(QVector<QVector<char>>& board, int depth, int alpha, int beta, bool maximizingPlayer) {
        

        if (depth == 0 || checkWin(currentPlayer) || checkWin((currentPlayer == 'X') ? 'O' : 'X') || isBoardFull()) {
            return evel('O');
        }

        if (maximizingPlayer) {
            int maxScore = INT_MIN;
            for (int row = 0; row < size; row++) {
                for (int col = 0; col < size; col++) {
                    if (board[row][col] == ' ') {
                        board[row][col] = 'O';
                        int score = minimaxAlphaBeta(board, depth - 1, alpha, beta, false);
                        board[row][col] = ' ';
                        maxScore = max(maxScore, score);
                        alpha = max(alpha, score);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return maxScore;
        }
        else {
            int minScore = INT_MAX;
            for (int row = 0; row < size; row++) {
                for (int col = 0; col < size; col++) {
                    if (board[row][col] == ' ') {
                        board[row][col] = 'X';
                        int score = minimaxAlphaBeta(board, depth - 1, alpha, beta, true);
                        board[row][col] = ' ';
                        minScore = min(minScore, score);
                        beta = min(beta, score);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return minScore;
        }
    }

    bool checkWin(char player) {
        // rows
        for (int row = 0; row < size; row++) {
            for (int col = 0; col <= size - winSize; col++) {
                bool win = true;
                for (int i = 0; i < winSize; i++) {
                    if (board[row][col + i] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }

        // columns
        for (int col = 0; col < size; col++) {
            for (int row = 0; row <= size - winSize; row++) {
                bool win = true;
                for (int i = 0; i < winSize; i++) {
                    if (board[row + i][col] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }

        // diagonals
        for (int row = 0; row <= size - winSize; row++) {
            for (int col = 0; col <= size - winSize; col++) {
                bool win = true;
                for (int i = 0; i < winSize; i++) {
                    if (board[row + i][col + i] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }

        for (int row = 0; row <= size - winSize; row++) {
            for (int col = size - 1; col >= winSize - 1; col--) {
                bool win = true;
                for (int i = 0; i < winSize; i++) {
                    if (board[row + i][col - i] != player) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isBoardFull()   {
        for (int row = 0; row < size; row++) {
            for (int col = 0; col < size; col++) {
                if (board[row][col] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }

    void endGame(const QString& message) {
        msgBox.setText(message);
        msgBox.exec();
        gameEnded = true;
    }

    bool buttonToIndex(QPushButton* button, int& row, int& col) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (layout->itemAtPosition(i, j)->widget() == button) {
                    row = i;
                    col = j;
                    return true;
                }
            }
        }
        return false;
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    int size = QInputDialog::getInt(nullptr, "Tic Tac Toe", "Enter the size of the board:",3,3,9,1);
    int winSize = QInputDialog::getInt(nullptr, "Tic Tac Toe", "Enter the number of marks to win:",3, 3, size, 1);

    TicTacToe game(size, winSize);
    game.setFixedSize(size * 100, size * 100);
    game.setWindowTitle("Tic Tac Toe");
    game.show();

    

    return app.exec();
}
