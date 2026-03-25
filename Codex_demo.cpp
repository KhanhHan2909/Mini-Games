#include <algorithm>
#include <chrono>
#include <conio.h>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <windows.h>

namespace {

constexpr int kBoardWidth = 10;
constexpr int kBoardHeight = 20;
constexpr int kHiddenRows = 4;
constexpr int kTotalRows = kBoardHeight + kHiddenRows;
constexpr int kFrameDelayMs = 16;
constexpr int kSoftDropDelayMs = 40;

enum class TetrominoType {
    I = 0,
    O = 1,
    T = 2,
    S = 3,
    Z = 4,
    J = 5,
    L = 6
};

struct Point {
    int x;
    int y;
};

struct Piece {
    TetrominoType type;
    int rotation;
    int x;
    int y;
};

const Point kShapes[7][4][4] = {
    {{{0, 1}, {1, 1}, {2, 1}, {3, 1}},
     {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
     {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
     {{1, 0}, {1, 1}, {1, 2}, {1, 3}}},

    {{{1, 0}, {2, 0}, {1, 1}, {2, 1}},
     {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
     {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
     {{1, 0}, {2, 0}, {1, 1}, {2, 1}}},

    {{{1, 0}, {0, 1}, {1, 1}, {2, 1}},
     {{1, 0}, {1, 1}, {2, 1}, {1, 2}},
     {{0, 1}, {1, 1}, {2, 1}, {1, 2}},
     {{1, 0}, {0, 1}, {1, 1}, {1, 2}}},

    {{{1, 0}, {2, 0}, {0, 1}, {1, 1}},
     {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
     {{1, 1}, {2, 1}, {0, 2}, {1, 2}},
     {{0, 0}, {0, 1}, {1, 1}, {1, 2}}},

    {{{0, 0}, {1, 0}, {1, 1}, {2, 1}},
     {{2, 0}, {1, 1}, {2, 1}, {1, 2}},
     {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
     {{1, 0}, {0, 1}, {1, 1}, {0, 2}}},

    {{{0, 0}, {0, 1}, {1, 1}, {2, 1}},
     {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
     {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
     {{1, 0}, {1, 1}, {0, 2}, {1, 2}}},

    {{{2, 0}, {0, 1}, {1, 1}, {2, 1}},
     {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
     {{0, 1}, {1, 1}, {2, 1}, {0, 2}},
     {{0, 0}, {1, 0}, {1, 1}, {1, 2}}},
};

const char kPieceGlyphs[7] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};

HANDLE GetOutputHandle() {
    static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    return handle;
}

void MoveCursorToTopLeft() {
    COORD position{0, 0};
    SetConsoleCursorPosition(GetOutputHandle(), position);
}

void HideCursor() {
    CONSOLE_CURSOR_INFO info{};
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetOutputHandle(), &info);
}

void SetTextColor(WORD attributes) {
    SetConsoleTextAttribute(GetOutputHandle(), attributes);
}

void ConfigureConsole() {
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(input, &mode)) {
        mode &= ~(ENABLE_QUICK_EDIT_MODE | ENABLE_MOUSE_INPUT);
        mode |= ENABLE_EXTENDED_FLAGS;
        SetConsoleMode(input, mode);
    }

    system("cls");
    HideCursor();
}

struct Game {
    std::vector<std::string> board;
    Piece current{};
    TetrominoType next = TetrominoType::I;
    std::deque<TetrominoType> bag;
    std::mt19937 rng;
    int score = 0;
    int linesCleared = 0;
    int level = 1;
    bool gameOver = false;
    bool paused = false;
    bool requestRedraw = true;

    Game()
        : board(kTotalRows, std::string(kBoardWidth, '.')),
          rng(static_cast<std::mt19937::result_type>(
              std::chrono::high_resolution_clock::now().time_since_epoch().count())) {}

    void Start() {
        RefillBagIfNeeded();
        next = TakeFromBag();
        SpawnPiece();
    }

    void RefillBagIfNeeded() {
        if (!bag.empty()) {
            return;
        }

        std::vector<TetrominoType> items = {
            TetrominoType::I, TetrominoType::O, TetrominoType::T, TetrominoType::S,
            TetrominoType::Z, TetrominoType::J, TetrominoType::L};
        std::shuffle(items.begin(), items.end(), rng);
        for (TetrominoType item : items) {
            bag.push_back(item);
        }
    }

    TetrominoType TakeFromBag() {
        RefillBagIfNeeded();
        TetrominoType value = bag.front();
        bag.pop_front();
        return value;
    }

    std::vector<Point> GetCells(const Piece& piece) const {
        std::vector<Point> cells;
        cells.reserve(4);

        const int type = static_cast<int>(piece.type);
        for (const Point& cell : kShapes[type][piece.rotation]) {
            cells.push_back({piece.x + cell.x, piece.y + cell.y});
        }

        return cells;
    }

    bool IsValidPosition(const Piece& piece) const {
        for (const Point& cell : GetCells(piece)) {
            if (cell.x < 0 || cell.x >= kBoardWidth || cell.y >= kTotalRows) {
                return false;
            }

            if (cell.y >= 0 && board[cell.y][cell.x] != '.') {
                return false;
            }
        }
        return true;
    }

    void SpawnPiece() {
        current = {next, 0, 3, 0};
        next = TakeFromBag();
        requestRedraw = true;

        if (!IsValidPosition(current)) {
            gameOver = true;
        }
    }

    bool TryMove(int dx, int dy) {
        Piece candidate = current;
        candidate.x += dx;
        candidate.y += dy;
        if (!IsValidPosition(candidate)) {
            return false;
        }
        current = candidate;
        requestRedraw = true;
        return true;
    }

    bool TryRotate() {
        Piece candidate = current;
        candidate.rotation = (candidate.rotation + 1) % 4;

        const int kicks[] = {0, -1, 1, -2, 2};
        for (int kick : kicks) {
            candidate.x = current.x + kick;
            candidate.y = current.y;
            if (IsValidPosition(candidate)) {
                current = candidate;
                requestRedraw = true;
                return true;
            }
        }

        return false;
    }

    void LockPiece() {
        for (const Point& cell : GetCells(current)) {
            if (cell.y >= 0 && cell.y < kTotalRows) {
                board[cell.y][cell.x] = kPieceGlyphs[static_cast<int>(current.type)];
            }
        }

        ClearLines();
        SpawnPiece();
    }

    void HardDrop() {
        while (TryMove(0, 1)) {
            score += 2;
        }
        LockPiece();
    }

    void SoftDropStep() {
        if (!TryMove(0, 1)) {
            LockPiece();
            return;
        }
        score += 1;
    }

    void GravityStep() {
        if (!TryMove(0, 1)) {
            LockPiece();
        }
    }

    void ClearLines() {
        int clearedNow = 0;

        for (int row = kTotalRows - 1; row >= 0; --row) {
            bool full = true;
            for (char cell : board[row]) {
                if (cell == '.') {
                    full = false;
                    break;
                }
            }

            if (!full) {
                continue;
            }

            board.erase(board.begin() + row);
            board.insert(board.begin(), std::string(kBoardWidth, '.'));
            ++clearedNow;
            ++row;
        }

        if (clearedNow == 0) {
            return;
        }

        linesCleared += clearedNow;
        level = 1 + linesCleared / 10;

        static const int kLineScores[5] = {0, 40, 100, 300, 1200};
        score += kLineScores[clearedNow] * level;
    }

    int FallDelayMs() const {
        const int speedUp = std::min(level - 1, 12) * 45;
        return std::max(90, 700 - speedUp);
    }

    std::vector<std::string> BuildFrame() const {
        std::vector<std::string> display(board.begin() + kHiddenRows, board.end());
        for (Point cell : GetCells(current)) {
            if (cell.y >= kHiddenRows && cell.y < kTotalRows && cell.x >= 0 && cell.x < kBoardWidth) {
                display[cell.y - kHiddenRows][cell.x] = kPieceGlyphs[static_cast<int>(current.type)];
            }
        }
        return display;
    }

    void Render() {
        std::vector<std::string> display = BuildFrame();
        MoveCursorToTopLeft();

        std::cout << "Classic Tetris\n";
        std::cout << "Controls: Left/Right or A/D, Down or S, Up/W rotate, Space hard drop, P pause, Q quit\n\n";

        for (int row = 0; row < kBoardHeight; ++row) {
            std::cout << "|";
            for (int col = 0; col < kBoardWidth; ++col) {
                const char cell = display[row][col];
                std::cout << (cell == '.' ? ' ' : '#');
            }
            std::cout << "|";

            if (row == 1) {
                std::cout << "   Score: " << score;
            } else if (row == 3) {
                std::cout << "   Lines: " << linesCleared;
            } else if (row == 5) {
                std::cout << "   Level: " << level;
            } else if (row == 8) {
                std::cout << "   Next:";
            } else if (row >= 10 && row < 14) {
                std::cout << "     ";
                RenderNextPreviewRow(row - 10);
            } else if (row == 16 && paused) {
                std::cout << "   [PAUSED]";
            } else if (row == 18 && gameOver) {
                std::cout << "   [GAME OVER]";
            }

            std::cout << "\n";
        }

        std::cout << "+" << std::string(kBoardWidth, '-') << "+\n";
        std::cout.flush();
        requestRedraw = false;
    }

    void RenderNextPreviewRow(int previewRow) const {
        std::string row(4, ' ');
        for (const Point& cell : kShapes[static_cast<int>(next)][0]) {
            if (cell.y == previewRow && cell.x >= 0 && cell.x < 4) {
                row[cell.x] = '#';
            }
        }
        std::cout << row;
    }
};

bool ConsumeKey(int& key) {
    if (!_kbhit()) {
        return false;
    }

    key = _getch();
    if (key == 0 || key == 224) {
        key = 256 + _getch();
    }
    return true;
}

bool HandleInput(Game& game) {
    int key = 0;
    bool requestedQuit = false;

    while (ConsumeKey(key)) {
        if (key == 'q' || key == 'Q') {
            requestedQuit = true;
            continue;
        }

        if (key == 'p' || key == 'P') {
            game.paused = !game.paused;
            game.requestRedraw = true;
            continue;
        }

        if (game.paused || game.gameOver) {
            continue;
        }

        switch (key) {
            case 'a':
            case 'A':
            case 256 + 75:
                game.TryMove(-1, 0);
                break;
            case 'd':
            case 'D':
            case 256 + 77:
                game.TryMove(1, 0);
                break;
            case 's':
            case 'S':
            case 256 + 80:
                game.SoftDropStep();
                break;
            case 'w':
            case 'W':
            case 256 + 72:
                game.TryRotate();
                break;
            case ' ':
                game.HardDrop();
                break;
            default:
                break;
        }
    }

    return requestedQuit;
}

}  // namespace

int main() {
    ConfigureConsole();
    SetTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    Game game;
    game.Start();
    game.Render();

    auto lastFall = std::chrono::steady_clock::now();
    auto lastSoftDrop = std::chrono::steady_clock::now();

    while (true) {
        if (HandleInput(game)) {
            break;
        }

        const auto now = std::chrono::steady_clock::now();

        if (!game.paused && !game.gameOver) {
            const auto fallElapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFall).count();
            if (fallElapsed >= game.FallDelayMs()) {
                game.GravityStep();
                lastFall = now;
            }

            if ((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0) {
                const auto softElapsed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSoftDrop).count();
                if (softElapsed >= kSoftDropDelayMs) {
                    game.SoftDropStep();
                    lastSoftDrop = now;
                    lastFall = now;
                }
            } else {
                lastSoftDrop = now;
            }
        }

        if (game.requestRedraw) {
            game.Render();
        }

        Sleep(kFrameDelayMs);
    }

    MoveCursorToTopLeft();
    game.Render();
    std::cout << "\nFinal score: " << game.score << "\n";
    std::cout << "Thanks for playing.\n";
    return 0;
}
