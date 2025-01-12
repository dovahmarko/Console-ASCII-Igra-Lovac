#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread> 
#include <chrono> /

const int BOARD_SIZE = 10;

class Board {
private:
    char grid[BOARD_SIZE][BOARD_SIZE];

public:
    Board() {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                grid[i][j] = '.';
            }
        }
    }

    void setObject(int x, int y, char symbol) {
        grid[x][y] = symbol;
    }

    void clearPosition(int x, int y) {
        grid[x][y] = '.';
    }

    void print() const {
        std::cout << "  a b c d e f g h i j\n";
        for (int i = 0; i < BOARD_SIZE; i++) {
            std::cout << BOARD_SIZE - i << " ";
            for (int j = 0; j < BOARD_SIZE; j++) {
                std::cout << grid[i][j] << " ";
            }
            std::cout << BOARD_SIZE - i << std::endl;
        }
        std::cout << "  a b c d e f g h i j\n";
    }
};

class Entity {
protected:
    int x, y;
    char symbol;
    Board& board;

public:
    Entity(int startX, int startY, char sym, Board& b) : x(startX), y(startY), symbol(sym), board(b) {
        board.setObject(x, y, symbol);
    }

    void move(int dx, int dy) {
        board.clearPosition(x, y);
        x += dx;
        y += dy;
        board.setObject(x, y, symbol);
    }

    int getX() const { return x; }
    int getY() const { return y; }
    char getSymbol() const { return symbol; }
};

class Player : public Entity {
public:
    Player(int startX, int startY, Board& b) : Entity(startX, startY, 'X', b) {}

    void move(char direction) {
        int dx = 0, dy = 0;
        switch (tolower(direction)) {
        case 'w': dx = -1; break;
        case 's': dx = 1; break;
        case 'a': dy = -1; break;
        case 'd': dy = 1; break;
        default:
            std::cout << "Nevažeća komanda za kretanje!\n";
            return;
        }
        Entity::move(dx, dy);
    }

    void shoot(char direction, int distance, Entity& monster1, Entity& monster2, Entity& monster3, Entity& target, bool& monster1Alive, bool& monster2Alive, bool& monster3Alive) {
        if (distance > 5) {
            std::cout << "Domet prevelik! Maksimalan domet je 5.\n";
            return;
        }

        int dx = 0, dy = 0;
        switch (tolower(direction)) {
        case 'w': dx = -1; break;
        case 's': dx = 1; break;
        case 'a': dy = -1; break;
        case 'd': dy = 1; break;
        default:
            std::cout << "Nevažeća komanda za pucanje!\n";
            return;
        }

        for (int i = 1; i <= distance; i++) {
            int targetXShot = x + dx * i;
            int targetYShot = y + dy * i;

            if (targetXShot < 0 || targetXShot >= BOARD_SIZE || targetYShot < 0 || targetYShot >= BOARD_SIZE) {
                std::cout << "Pucanje je izvan granica!\n";
                return;
            }

            if (targetXShot == monster1.getX() && targetYShot == monster1.getY() && monster1Alive) {
                std::cout << "Pogodio si prvog monstera!\n";
                monster1Alive = false;
                board.clearPosition(monster1.getX(), monster1.getY());
                return;
            }

            if (targetXShot == monster2.getX() && targetYShot == monster2.getY() && monster2Alive) {
                std::cout << "Pogodio si drugog monstera!\n";
                monster2Alive = false;
                board.clearPosition(monster2.getX(), monster2.getY());
                return;
            }

            if (targetXShot == monster3.getX() && targetYShot == monster3.getY() && monster3Alive) {
                std::cout << "Pogodio si trećeg monstera!\n";
                monster3Alive = false;
                board.clearPosition(monster3.getX(), monster3.getY());
                return;
            }
        }

        std::cout << "Domet nedovoljan za pogodak.\n";
    }
};

class Monster : public Entity {
public:
    Monster(int startX, int startY, Board& b) : Entity(startX, startY, 'O', b) {}

    void moveRandom() {
        int direction = rand() % 4;
        int dx = 0, dy = 0;

        switch (direction) {
        case 0: if (x > 0) dx = -1; break; 
        case 1: if (x < BOARD_SIZE - 1) dx = 1; break; 
        case 2: if (y > 0) dy = -1; break; 
        case 3: if (y < BOARD_SIZE - 1) dy = 1; break; 
        }
        Entity::move(dx, dy);
    }
};

class Game {
private:
    Board board;
    Player player;
    Monster monster1;
    Monster monster2;
    Monster monster3;
    Entity target;
    bool monster1Alive;
    bool monster2Alive;
    bool monster3Alive;

public:
    Game() : board(),
        player(0, 0, board),
        monster1(3, 3, board),
        monster2(6, 6, board),
        monster3(7, 7, board),
        target(9, 9, 'E', board),
        monster1Alive(true),
        monster2Alive(true),
        monster3Alive(true) {
    }

    void run() {
        srand(time(0));
        while (true) {
            board.print();
            std::cout << "Unesite potez (npr. W, A, S, D za kretanje, W5 za pucanje): ";
            std::string input;
            std::getline(std::cin, input);

            if (input.length() == 1) {
                player.move(input[0]);

                if (player.getX() == target.getX() && player.getY() == target.getY()) {
                    std::cout << "Kukavički ste pobjegli, sram vas bilo!!!!.\n";
                    break;
                }
            }
            else if (input.length() > 1) {
                char direction = input[0];
                int distance = std::stoi(input.substr(1));
                player.shoot(direction, distance, monster1, monster2, monster3, target, monster1Alive, monster2Alive, monster3Alive);
            }
            else {
                std::cout << "Nevažeći unos!\n";
            }

            if (monster1Alive) monster1.moveRandom();
            if (monster2Alive) monster2.moveRandom();
            if (monster3Alive) monster3.moveRandom();

            if (!monster1Alive && !monster2Alive && !monster3Alive) {
                std::cout << "Pobjeda! Sva čudovišta su ubijena.\n";
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
        }
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
