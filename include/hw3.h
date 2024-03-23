#include <stdlib.h>

typedef struct TileStack
{
    char stack[5];
    int top;
} TileStack;

typedef struct GameState
{
    int boardHeight;
    int boardWidth;
    TileStack** currentBoard;
    int** boardDepth;
    struct GameState* prevGameState;
} GameState;

GameState* initialize_game_state(const char *filename);
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
GameState* undo_place_tiles(GameState *game);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);
