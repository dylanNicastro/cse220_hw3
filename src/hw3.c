#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)

int push(TileStack *chars, char c) {
    if ((*chars).top == 4) {
        return 1;
    }
    (*chars).stack[++(*chars).top] = c;
    return 0;
}

// Note - does not return the popped char
int pop(TileStack *chars) {
    if ((*chars).top == -1) {
        return 1;
    }
    (*chars).top--;
    return 0;
    
}

char peek(TileStack chars) {
    if (chars.top == -1) {
        return '.';
    }
    return chars.stack[chars.top];
}

int depthSum(GameState *game) {
    int count = 0;
    for (int i = 0; i < (*game).boardHeight; i++) {
        for (int j = 0; j < (*game).boardWidth; j++) {
            count = count + (*game).boardDepth[i][j];
        }
    }
    return count;
}

int nonSpaceChars(const char *str) {
    int count = 0;
    for (int i = 0; i < (int)strlen(str); i++) {
        if (str[i] != ' ') {
            count++;
        }
    }
    return count;
}

int checkWord(char* word) {
    FILE *wordsList;
    wordsList = fopen("tests/words.txt", "r");
    int wordIsLegal = 0;
    char *dictWord = malloc(47*sizeof(char));
    while (fscanf(wordsList, "%s%*c", dictWord) == 1) {
        for (int i = 0; dictWord[i] != '\0'; i++) {
            dictWord[i] = toupper(dictWord[i]);
        }
        if (strcmp(dictWord, word) == 0) {
            wordIsLegal = 1;
            break;
        }
    }
    free(dictWord);
    fflush(wordsList);
    fclose(wordsList);
    return wordIsLegal;
}

int parseWords(GameState game, char direction, int startindex) {
    //printf("Starting parsing with boardheight = %d, boardwidth = %d\n",game.boardHeight, game.boardWidth);
    int illegalWord = 0;
    char *line = malloc((game.boardWidth + 1)*sizeof(char));
    int startrow = 0;
    int endrow = game.boardHeight;
    int startcol = 0;
    int endcol = game.boardWidth;
    if (direction == 'H') {
        startrow = startindex;
        endrow = startindex+1;
    }
    if (direction == 'V') {
        startcol = startindex;
        endcol = startindex+1;
    }
    for (int i = startrow; i < endrow; i++) {
        int j = 0;
        for (j = 0; j < game.boardWidth; j++) {
            line[j] = peek(game.currentBoard[i][j]);
        }
        line[j] = '\0';

        char* word = strtok(line,".");
        while (word != NULL) {
            //printf("Checking %s\n", word);
            if (checkWord(word) == 0) {
                //printf("Word was illegal!\n");
                illegalWord = 1;
                break;
            }
            word = strtok(NULL,".");
        }
    }
    if (illegalWord == 0) {
        line = realloc(line, (game.boardHeight + 1)*sizeof(char));
        for (int j = startcol; j < endcol; j++) {
            int i = 0;
            for (i = 0; i < game.boardHeight; i++) {
                line[i] = peek(game.currentBoard[i][j]);
            }
            line[i] = '\0';

            char* word = strtok(line,".");
            while (word != NULL) {
                //printf("Checking %s\n", word);
                if (checkWord(word) == 0) {
                    //printf("Word was illegal!\n");
                    illegalWord = 1;
                    break;
                }
                word = strtok(NULL,".");
            }
        }
    }
    free(line);
    return illegalWord;
}

int checkForCoveredWords(GameState game, int** oldDepth, char direction, int startindex) {
    int coveredWord = 0;
    char *line = malloc((game.boardWidth + 1)*sizeof(char));
    char *oldline = malloc((game.boardWidth + 1)*sizeof(char));
    if (direction == 'H') {
        for (int i = startindex; i < startindex+1; i++) {
            int j = 0;
            for (j = 0; j < game.boardWidth; j++) {
                line[j] = game.boardDepth[i][j] + '0';
                oldline[j] = oldDepth[i][j] + '0';

            }
            line[j] = '\0';
            oldline[j] = '\0';
            //printf("Line: %s\n", line);
            //printf("Old line: %s\n", oldline);

            char* word = strtok(oldline,"0");
            while (word != NULL) {
                //printf("New: %s\n", word);
                int wordstartindex = word-oldline;
                int wordendindex = wordstartindex+(int)strlen(word);
                //printf("Word is from index %d to %d\n", wordstartindex, wordendindex-1);
                char* oldword = malloc((strlen(word)+1)*sizeof(char));
                char* newwordminus1 = malloc((strlen(word)+1)*sizeof(char));
                int k = 0;
                for (k = wordstartindex; k < wordendindex; k++) {
                    newwordminus1[k-wordstartindex] = (char)((int)line[k] - 1);
                }
                newwordminus1[k-wordstartindex] = '\0';
                for (k = wordstartindex; k < wordendindex; k++) {
                    oldword[k-wordstartindex] = oldline[k];
                }
                oldword[k-wordstartindex] = '\0';
                //printf("Word minus 1 is %s, Old word is %s\n", newwordminus1, oldword);
                for (k = 0; k < (int)strlen(word); k++) {
                    //printf("%c vs %c\n", newwordminus1[k], oldword[k]);
                    if (newwordminus1[k] == oldword[k]) {
                        coveredWord = 1;
                    }
                    else {
                        coveredWord = 0;
                        break;
                    }
                }
                //printf("coveredWord: %d\n", coveredWord);
                if (coveredWord == 1) {
                    free(newwordminus1);
                    free(oldword);
                    break;
                }
                free(newwordminus1);
                free(oldword);
                word = strtok(NULL,"0");
            }
        }
    }

    if (direction == 'V') {
        line = realloc(line, (game.boardHeight + 1)*sizeof(char));
        oldline = realloc(oldline, (game.boardHeight + 1)*sizeof(char));
        for (int j = startindex; j < startindex+1; j++) {
            int i = 0;
            for (i = 0; i < game.boardHeight; i++) {
                line[i] = game.boardDepth[i][j] + '0';
                oldline[i] = oldDepth[i][j] + '0';

            }
            line[i] = '\0';
            oldline[i] = '\0';

            char* word = strtok(oldline,"0");
            while (word != NULL) {
                //printf("New: %s\n", word);
                int wordstartindex = word-oldline;
                int wordendindex = wordstartindex+(int)strlen(word);
                //printf("Word is from index %d to %d\n", wordstartindex, wordendindex-1);
                char* oldword = malloc((strlen(word)+1)*sizeof(char));
                char* newwordminus1 = malloc((strlen(word)+1)*sizeof(char));
                int k = 0;
                for (k = wordstartindex; k < wordendindex; k++) {
                    newwordminus1[k-wordstartindex] = (char)((int)line[k] - 1);
                }
                newwordminus1[k-wordstartindex] = '\0';
                for (k = wordstartindex; k < wordendindex; k++) {
                    oldword[k-wordstartindex] = oldline[k];
                }
                oldword[k-wordstartindex] = '\0';
                //printf("Word minus 1 is %s, Old word is %s\n", newwordminus1, oldword);
                for (k = 0; k < (int)strlen(word); k++) {
                    //printf("%c vs %c\n", newwordminus1[k], oldword[k]);
                    if (newwordminus1[k] == oldword[k]) {
                        coveredWord = 1;
                    }
                    else {
                        coveredWord = 0;
                        break;
                    }
                }
                //printf("coveredWord: %d\n", coveredWord);
                if (coveredWord == 1) {
                    free(newwordminus1);
                    free(oldword);
                    break;
                }
                free(newwordminus1);
                free(oldword);
                word = strtok(NULL,"0");
            }
        }
    }
    free(line);
    free(oldline);
    return coveredWord;
}

//ChangeStack changes;
GameState currentState;

GameState* initialize_game_state(const char *filename) {

    FILE *inputfile;
    inputfile = fopen(filename, "r");
    if (inputfile == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return NULL;
    }
    int currentrow = 0;
    int currentcol = 0;
    char currentchar;
    while (1) {
        currentchar = fgetc(inputfile); 
        if (feof(inputfile)) {
            break;
        }
        if (currentchar == '\n') {
            currentrow++;
            //printf("\n");
        }
        else {
            //printf("%c ",currentchar);
            currentcol++;
        }
    }
    currentState.boardHeight = currentrow;
    currentState.boardWidth = currentcol/currentrow;
    rewind(inputfile);
    // create board array
    TileStack **board = malloc(currentState.boardHeight*sizeof(TileStack*));
    for (int i = 0; i < currentState.boardHeight; i++) {
        board[i] = malloc(currentState.boardWidth*sizeof(TileStack));
        for (int j = 0; j < currentState.boardWidth; j++) {
            TileStack tile;
            tile.top = -1;
            board[i][j] = tile;
        }
    }
    // create depth array
    int **depth = malloc(currentState.boardHeight*sizeof(int*));
    for (int i = 0; i < currentState.boardHeight; i++) {
        depth[i] = malloc(currentState.boardWidth*sizeof(int));
    }

    currentrow = 0;
    currentcol = 0;

    // store data
    while (1) {
        currentchar = fgetc(inputfile); 
        if (feof(inputfile)) {
            break;
        }
        if (currentchar == '\n') {
            currentrow++;
            currentcol = 0;
            // printf("\n");
        }
        else if (currentchar != '.') {
            TileStack *currentTile = &board[currentrow][currentcol];
            push(currentTile, currentchar);
            depth[currentrow][currentcol] = 1;
            currentcol++;
            // printf("%c", currentchar);
        }
        else {
            depth[currentrow][currentcol] = 0;
            currentcol++;
            // printf("%c", currentchar);
        }
    }

    currentState.currentBoard = board;
    currentState.boardDepth = depth;
    fflush(inputfile);
    fclose(inputfile);
    return &currentState;
}

GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    if (row < 0 || col < 0 || (direction != 'V' && direction != 'H')) { // basic checks for row, col, direction
        return game;
    }
    // if it is the first word on the board and it is 1 or less letters:
    if (depthSum(game) == 0 && nonSpaceChars(tiles) < 2) {
        return game;
    }

    int newheight = (*game).boardHeight;
    int newwidth = (*game).boardWidth;
    if (direction == 'H') {
        if (row > (*game).boardHeight-1) {
            return game;
        }
        if ((int)(col+strlen(tiles)) > newwidth) {
            newwidth = (int)(col+strlen(tiles));
        }
    }
    else if (direction == 'V') {
        if (col > (*game).boardWidth-1) {
            return game;
        }
        if ((int)(row+strlen(tiles)) > newheight) {
            newheight = (int)(row+strlen(tiles));
        }
    }
    //printf("Height: %d\nWidth: %d\n", newheight, newwidth);
    // New height and width necessary are now obtained


    // Resize the board and depth arrays with realloc and malloc:
    for (int i = 0; i < (*game).boardHeight; i++) {
        (*game).currentBoard[i] = realloc((*game).currentBoard[i], newwidth*sizeof(TileStack));
        (*game).boardDepth[i] = realloc((*game).boardDepth[i], newwidth*sizeof(int));
        for (int j = (*game).boardWidth; j < newwidth; j++) {
            TileStack tile;
            tile.top = -1;
            (*game).currentBoard[i][j] = tile;
            (*game).boardDepth[i][j] = 0;
        }
        for (int j = 0; j < newwidth; j++) {
            //printf("%c ", peek((*game).currentBoard[i][j]));
        }
        //printf("\n");
    }
    (*game).currentBoard = realloc((*game).currentBoard, newheight*sizeof(TileStack*));
    (*game).boardDepth = realloc((*game).boardDepth, newheight*sizeof(int*));
    for (int i = (*game).boardHeight; i < newheight; i++) {
        (*game).currentBoard[i] = malloc(newwidth*sizeof(TileStack));
        (*game).boardDepth[i] = malloc(newwidth*sizeof(int));
        for (int j = 0; j < newwidth; j++) {
            TileStack tile;
            tile.top = -1;
            (*game).currentBoard[i][j] = tile;
            (*game).boardDepth[i][j] = 0;
            //printf("%c ", peek((*game).currentBoard[i][j]));
        }
        //printf("\n");
    }
    (*game).boardHeight = newheight;
    (*game).boardWidth = newwidth;


    // parseWords(*game);
    // Board is now properly resized
    // Make a copy of the depth board for legality checking later on:
    int** depthcopy = malloc(newheight*sizeof(int*));
    for (int i = 0; i < newheight; i++) {
        depthcopy[i] = malloc(newwidth*sizeof(int));
        for (int j = 0; j < newwidth; j++) {
            depthcopy[i][j] = (*game).boardDepth[i][j];
        }
    }


    //Change newchange;
    //newchange.tiles = tiles;

    // for each char in the tiles string:
    int tiles_placed = 0;
    for (int i = 0; i < (int)(strlen(tiles)); i++) {
        // if it is not a space:
        if (tiles[i] != ' ') {
            // 2 cases:
            // vertical:
            if (direction == 'V') {
                // col is constant, current row is row+i
                // if the tile is not full:
                if ((*game).boardDepth[row+i][col] < 5) {
                    (*game).boardDepth[row+i][col]++;
                    push(&(*game).currentBoard[row+i][col], tiles[i]);
                    tiles_placed++;
                    //printf("Added %c to the board position [%d][%d]\n", tiles[i], row+i, col);
                }
                else {
                    for (int j = 0; j < newheight; j++) {
                        free(depthcopy[j]);
                    }
                    free(depthcopy);
                    return game;
                }
            }
            // horizontal:
            if (direction == 'H') {
                // row is constant, current col is col+i
                // if the tile is not full:
                if ((*game).boardDepth[row][col+i] < 5) {
                    (*game).boardDepth[row][col+i]++;
                    push(&(*game).currentBoard[row][col+i], tiles[i]);
                    tiles_placed++;
                    //printf("Added %c to the board position [%d][%d]\n", tiles[i], row, col+i);
                }
                else {
                    for (int j = 0; j < newheight; j++) {
                        free(depthcopy[j]);
                    }
                    free(depthcopy);
                    return game;
                }
            }
        }
    }
    *num_tiles_placed = tiles_placed;



    int startindex;
    if (direction == 'V') {
        startindex = col;
    }
    else {
        startindex = row;
    }

    if (parseWords(*game, direction, startindex) == 1) {
        //printf("Illegal word detected!\n");
        undo_place_tiles(game);
        for (int i = 0; i < newheight; i++) {
            free(depthcopy[i]);
        }
        free(depthcopy);
        return game;
    }
    else {
        //printf("All words found were legal.\n");
    }


    if (checkForCoveredWords(*game, depthcopy, direction, startindex) == 1) {
        //printf("A word was covered!\n");
        undo_place_tiles(game);
        for (int i = 0; i < newheight; i++) {
            free(depthcopy[i]);
        }
        free(depthcopy);
        return game;
    }
    else {
        //printf("No words were covered.\n");
    }




    for (int i = 0; i < newheight; i++) {
        free(depthcopy[i]);
    }
    free(depthcopy);
    return game;
}

GameState* undo_place_tiles(GameState *game) {
    //printf("No good!");
    return game;
}

void free_game_state(GameState *game) {
    for (int i = 0; i < (*game).boardHeight; i++) {
        free((*game).boardDepth[i]);
        free((*game).currentBoard[i]);
    }
    free((*game).boardDepth);
    free((*game).currentBoard);
}

void save_game_state(GameState *game, const char *filename) {
    FILE *outputfile;
    outputfile = fopen(filename, "w");
    // Write board array
    // printf("Height: %d, Width: %d\n", (*game).boardHeight, (*game).boardWidth);
    for (int i = 0; i < (*game).boardHeight; i++) {
        for (int j = 0; j < (*game).boardWidth; j++) {
            // printf("%c",peek((*game).currentBoard[i][j]));
            fprintf(outputfile, "%c", peek((*game).currentBoard[i][j]));
        }
        fprintf(outputfile, "\n");
    }
    for (int i = 0; i < (*game).boardHeight; i++) {
        for (int j = 0; j < (*game).boardWidth; j++) {
            // printf("%c",peek((*game).currentBoard[i][j]));
            fprintf(outputfile, "%d", (*game).boardDepth[i][j]);
        }
        fprintf(outputfile, "\n");
    }
    fclose(outputfile);
}
