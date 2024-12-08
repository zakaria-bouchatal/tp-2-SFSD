#ifndef RENTAL_H
#define RENTAL_H

#include "struct.h"

// Opens or creates a rental file
int openRentalFile(TOF *tof, const char *filename, char mode);

// Inserts a new rental linking a customer and a game
// Returns the new rental ID
int insertRental(TOF *tof, int customerID, int gameID);

// Searches for a rental by ID
// Returns 0 if found, -1 otherwise
int searchRentalByID(TOF *tof, int rentalID, Rental *foundRental);

// Cancels a rental by setting active = -1
int cancelRental(TOF *tof, int rentalID);

// Prints out a single rental's details
void printRental(const Rental *rental);

#endif
