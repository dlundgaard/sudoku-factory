#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

struct point {
    int x;
    int y;
};

class Sudoku {
    private:
        int grid[81];
        int guessQueue[9];
        int positionQueue[81];

    public:
        Sudoku();
        int findEmptyPos();
        int rowOf(int);
        int colOf(int);
        int getVal(int);
        bool rowContains(int, int);
        bool colContains(int, int);
        bool blockContains(int, int, int);
        bool insertIsLegal(int, int);
        bool solve();
        void complicate();
        void countAmountSolutions(int &);
        string toString();
};

Sudoku::Sudoku() {
    for (int i = 0; i < 81; i++) {
        this->grid[i] = 0;
    }
    for (int i = 0; i < 9; i++) {
        this->guessQueue[i] = i + 1;
    }
    for (int i = 0; i < 81; i++) {
        this->positionQueue[i] = i;
    }
    random_shuffle(
        this->guessQueue, 
        (this->guessQueue) + 9, 
        [](int max) { return (rand() % max); }
    );
    random_shuffle(
        this->positionQueue, 
        (this->positionQueue) + 81, 
        [](int max) { return (rand() % max); }
    );
}

int Sudoku::findEmptyPos() {
    for (int pos = 0; pos < 81; pos++) {
        if (this->grid[pos] == 0) {
            return pos;
        } 
    }
    return -1;
}

int Sudoku::rowOf(int pos) {
    return pos / 9;
}

int Sudoku::colOf(int pos) {
    return pos % 9;
}

int Sudoku::getVal(int pos) {
    return this->grid[pos];
}

bool Sudoku::rowContains(int row, int val) {
    for (int pos = row * 9; pos < (row * 9) + 9; pos++) {
        if (getVal(pos) == val) {
            return true;
        }
    }
    return false;
}

bool Sudoku::colContains(int col, int val) {
    for (int pos = col; pos < 81; pos += 9) {
        if (this->grid[pos] == val) {
            return true;
        }
    }
    return false;
}

bool Sudoku::blockContains(int row, int col, int val) {
    int blockRow = row / 3;
    int blockCol = col / 3;
    for (int i = 0; i < 27; i += 9) {
        for (int j = 0; j < 3; j++) {
            int pos = (blockRow * 27 + i) + (blockCol * 3 + j);
            if (this->grid[pos] == val) {
                return true;
            }
        }
    }
    return false;
}

bool Sudoku::insertIsLegal(int pos, int val) {
    int row = rowOf(pos);
    int col = colOf(pos);
    return  !rowContains(row, val) && 
            !colContains(col, val) && 
            !blockContains(row, col, val);
}

bool Sudoku::solve() {
    int emptyPos = findEmptyPos();
    if (emptyPos == -1) {
        return true;
    } else {
        for (int guessIndex = 0; guessIndex < 9; guessIndex++) {
            int guessVal = this->guessQueue[guessIndex];
            if (insertIsLegal(emptyPos, guessVal)) {
                this->grid[emptyPos] = guessVal;
                if (solve()) {
                    return true;
                }
                this->grid[emptyPos] = 0;
            }
        }
        return false;  
    }
}

void Sudoku::countAmountSolutions(int &amountSolutions) {
    int emptyPos = findEmptyPos();
    if (emptyPos == -1) {
        amountSolutions++;
    } else {
        for (int guessIndex = 0; guessIndex < 9 && amountSolutions <= 1; guessIndex++) {
            int guessVal = this->guessQueue[guessIndex];
            if (insertIsLegal(emptyPos, guessVal)) {
                this->grid[emptyPos] = guessVal;
                countAmountSolutions(amountSolutions);
            }
            this->grid[emptyPos] = 0;
        }
    }
}

void Sudoku::complicate() {
    for (int pos = 0; pos < 81; pos++) {
        int new_pos = this->positionQueue[pos];
        int oldValue = this->grid[new_pos];
        this->grid[new_pos] = 0;

        int amountSolutions = 0;
        countAmountSolutions(amountSolutions);
        if (amountSolutions != 1) {
            this->grid[new_pos] = oldValue;
        }
    }
}

string Sudoku::toString() {
    string output = "";
    for (int pos = 0; pos < 81; pos++) {
        if (this->grid[pos] != 0) {
            output += to_string(this->grid[pos]);
        } else {
            output += "_";
        }
        int row = rowOf(pos);
        int col = colOf(pos);
        if (col % 3 == 2) {
            output += " ";
        }
        output += " ";
        if (col == 8) {
            output += "\n";
            if (row > 0 && row % 3 == 2) {
                output += "\n";
            }
        }
    }
    return output;
}

void makeSVG(const char *exportPath) {
    FILE *outputFile = fopen(exportPath, "w");
    int A4Width = 1485;
    int A4Height = 1050;
    fprintf(outputFile, "<?xml version='1.0' encoding='UTF-8' standalone='no'?>");
    fprintf(outputFile, "<svg version='1.1' baseProfile='full' width='%d' height='%d' xmlns='http://www.w3.org/2000/svg' >\n",
        A4Width,
        A4Height
    );
    int sudokuSize = 450;
    int margin = 20;
    char style[60];
    Sudoku *puzzle;
    for (int i = 0; i < 6; i++) {
        puzzle = new Sudoku();
        puzzle->solve();
        puzzle->complicate();
        int originX = margin + (sudokuSize + margin) * (i % 3);
        int originY = margin + (sudokuSize + margin) * (i / 3);
        point topLeft = {
           originX,
           originY, 
        };
        point topRight = {
           originX + sudokuSize, 
           originY, 
        };
        point bottomRight = {
           originX + sudokuSize, 
           originY + sudokuSize, 
        };
        point bottomLeft = {
           originX, 
           originY + sudokuSize, 
        };
        sprintf(style, "'fill:none; stroke:black; stroke-width:4;'");
        fprintf(outputFile, "<polyline points='%d,%d %d,%d %d,%d %d,%d %d,%d %d,%d' style=%s />\n", 
            topLeft.x, topLeft.y,
            topRight.x, topRight.y,
            bottomRight.x, bottomRight.y,
            bottomLeft.x, bottomLeft.y,
            topLeft.x, topLeft.y,
            topRight.x, topRight.y, // draw top line again to avoid gap in top-left corner
            style
        );
        int cellSize = sudokuSize / 9;
        for (int i = 1; i < 9; i++) {
            if (i % 3 == 0) {
                sprintf(style, "'fill:none; stroke:black; stroke-width:4;'");;
            } else {
                sprintf(style, "'fill:none; stroke:black; stroke-width:1;'");
            }
            fprintf(outputFile, "<polyline points='%d,%d %d,%d' style=%s />\n",
                originX, originY + cellSize * i,
                originX + sudokuSize, originY + cellSize * i,
                style
            );        
            fprintf(outputFile, "<polyline points='%d,%d %d,%d' style=%s />\n",
                originX + cellSize * i, originY,
                originX + cellSize * i, originY + sudokuSize,
                style
            );
        }
        sprintf(style, "'font-weight:bold' font-size='30px' font-family='Arial'");
        for (int pos = 0; pos < 81; pos++) {
            if (puzzle->getVal(pos) != 0) {
                int row = puzzle->rowOf(pos);
                int col = puzzle->colOf(pos);
                int x = originX + col * cellSize + 16;
                int y = originY + row * cellSize + 35;
                fprintf(outputFile, "<text x='%d' y='%d' style=%s >%d</text>\n",
                    x,
                    y,
                    style,
                    puzzle->getVal(pos)
                );
            }
        }
        delete puzzle;
    }
    fprintf(outputFile, "</svg>");
    fclose(outputFile);
}

int main(int argc, const char *argv[]) {
    srand(chrono::high_resolution_clock::now().time_since_epoch().count());

    const char *exportPath = "output.svg";
    if (argc > 1) {
        exportPath = *++argv;
    }
    makeSVG(exportPath);

    return 0;
}
