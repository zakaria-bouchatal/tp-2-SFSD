#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void writeGameMetadata(TOF *tof) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return;
    fseek(file, 0, SEEK_SET);
    fwrite(&tof->metadata, sizeof(Metadata), 1, file);
    fclose(file);
}

static void readGameMetadata(TOF *tof) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return;
    fseek(file, 0, SEEK_SET);
    fread(&tof->metadata, sizeof(Metadata), 1, file);
    fclose(file);
}

int openGameFile(TOF *tof, const char *filename, char mode) {
    strcpy(tof->filename, filename);
    FILE *file;
    if (mode == 'N') {
        file = fopen(filename, "w+b");
        if (!file) {
            perror("Error creating game file");
            return -1;
        }
        tof->metadata.numBlocks = 0;
        tof->metadata.lastID = 0;
        fwrite(&tof->metadata, sizeof(Metadata), 1, file);
        fclose(file);
    } else if (mode == 'A') {
        file = fopen(filename, "r+b");
        if (!file) {
            perror("Error opening game file");
            return -1;
        }
        fclose(file);
        readGameMetadata(tof);
    } else {
        fprintf(stderr, "Invalid mode for game file: %c\n", mode);
        return -1;
    }
    return 0;
}

int insertGame(TOF *tof, Game game) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) {
        perror("Error inserting game");
        return -1;
    }

    GameBlock block;
    int blockIndex = tof->metadata.numBlocks - 1;
    if (blockIndex < 0) {
        blockIndex = 0;
        block.numGames = 0;
        tof->metadata.numBlocks = 1;
    } else {
        fseek(file, sizeof(Metadata) + blockIndex * sizeof(GameBlock), SEEK_SET);
        if (fread(&block, sizeof(GameBlock), 1, file) != 1) {
            printf("Error reading last game block. Initializing a new one.\n");
            block.numGames = 0;
        }
    }

    game.gameID = tof->metadata.lastID + 1;
    game.borrowerID = -1;

    if (block.numGames < 10) {
        block.games[block.numGames++] = game;
    } else {
        blockIndex++;
        block.numGames = 1;
        block.games[0] = game;
        tof->metadata.numBlocks++;
    }

    fseek(file, sizeof(Metadata) + blockIndex * sizeof(GameBlock), SEEK_SET);
    fwrite(&block, sizeof(GameBlock), 1, file);

    tof->metadata.lastID = game.gameID;
    writeGameMetadata(tof);

    fclose(file);
    return game.gameID;
}

int searchGameByID(TOF *tof, int gameID, Game *foundGame) {
    if (gameID <= 0 || gameID > tof->metadata.lastID) return -1;

    int index = gameID - 1;
    int blockIndex = index / 10;
    int pos = index % 10;

    if (blockIndex >= tof->metadata.numBlocks) return -1;

    FILE *file = fopen(tof->filename, "rb");
    if (!file) return -1;

    long offset = sizeof(Metadata) + blockIndex * sizeof(GameBlock) 
                  + pos * sizeof(Game);
    fseek(file, offset, SEEK_SET);
    if (fread(foundGame, sizeof(Game), 1, file) != 1) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int updateGame(TOF *tof, Game game) {
    if (game.gameID <= 0 || game.gameID > tof->metadata.lastID) return -1;
    int index = game.gameID - 1;
    int blockIndex = index / 10;
    int pos = index % 10;

    if (blockIndex >= tof->metadata.numBlocks) return -1;

    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return -1;

    long offset = sizeof(Metadata) + blockIndex * sizeof(GameBlock) 
                  + pos * sizeof(Game);
    fseek(file, offset, SEEK_SET);
    if (fwrite(&game, sizeof(Game), 1, file) != 1) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

void printGame(const Game *game) {
    printf("Game ID: %d\n", game->gameID);
    printf("Title: %s\n", game->title);
    printf("Borrower ID: %d\n", game->borrowerID);
}

void displayAllGames(TOF *tof) {
    FILE *file = fopen(tof->filename, "rb");
    if (!file) {
        perror("Error opening game file for display");
        return;
    }

    fseek(file, sizeof(Metadata), SEEK_SET);
    for (int b = 0; b < tof->metadata.numBlocks; b++) {
        GameBlock block;
        if (fread(&block, sizeof(GameBlock), 1, file) != 1) break;
        for (int i = 0; i < block.numGames; i++) {
            printf("Game ID: %d, Title: %s\n",
                   block.games[i].gameID,
                   block.games[i].title);
        }
    }
    fclose(file);
}
