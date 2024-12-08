#ifndef GAME_H
#define GAME_H

#include "struct.h"

// Opens or creates a game file
int openGameFile(TOF *tof, const char *filename, char mode);

// Inserts a new game and returns its generated ID
int insertGame(TOF *tof, Game game);

// Searches for a game by ID
// Returns 0 if found, -1 otherwise
int searchGameByID(TOF *tof, int gameID, Game *foundGame);

// Updates an existing game record
int updateGame(TOF *tof, Game game);

// Prints out a single game's details
void printGame(const Game *game);

// Displays all games (ID and title)
void displayAllGames(TOF *tof);

#endif
