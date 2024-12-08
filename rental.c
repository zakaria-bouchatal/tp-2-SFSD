#include "rental.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void writeRentalMetadata(TOF *tof) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return;
    fseek(file, 0, SEEK_SET);
    fwrite(&tof->metadata, sizeof(Metadata), 1, file);
    fclose(file);
}

static void readRentalMetadata(TOF *tof) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return;
    fseek(file, 0, SEEK_SET);
    fread(&tof->metadata, sizeof(Metadata), 1, file);
    fclose(file);
}

int openRentalFile(TOF *tof, const char *filename, char mode) {
    strcpy(tof->filename, filename);
    FILE *file;
    if (mode == 'N') {
        file = fopen(filename, "w+b");
        if (!file) {
            perror("Error creating rental file");
            return -1;
        }
        tof->metadata.numBlocks = 0;
        tof->metadata.lastID = 0;
        fwrite(&tof->metadata, sizeof(Metadata), 1, file);
        fclose(file);
    } else if (mode == 'A') {
        file = fopen(filename, "r+b");
        if (!file) {
            perror("Error opening rental file");
            return -1;
        }
        fclose(file);
        readRentalMetadata(tof);
    } else {
        fprintf(stderr, "Invalid mode for rental file: %c\n", mode);
        return -1;
    }
    return 0;
}

int insertRental(TOF *tof, int customerID, int gameID) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) {
        perror("Error inserting rental");
        return -1;
    }

    RentalBlock block;
    int blockIndex = tof->metadata.numBlocks - 1;
    if (blockIndex < 0) {
        blockIndex = 0;
        block.numRentals = 0;
        tof->metadata.numBlocks = 1;
    } else {
        fseek(file, sizeof(Metadata) + blockIndex * sizeof(RentalBlock), SEEK_SET);
        if (fread(&block, sizeof(RentalBlock), 1, file) != 1) {
            printf("Error reading last rental block. Initializing new one.\n");
            block.numRentals = 0;
        }
    }

    Rental rental;
    rental.rentalID = tof->metadata.lastID + 1;
    rental.customerID = customerID;
    rental.gameID = gameID;
    rental.active = 1; // Active rental

    if (block.numRentals < 10) {
        block.rentals[block.numRentals++] = rental;
    } else {
        blockIndex++;
        block.numRentals = 1;
        block.rentals[0] = rental;
        tof->metadata.numBlocks++;
    }

    fseek(file, sizeof(Metadata) + blockIndex * sizeof(RentalBlock), SEEK_SET);
    fwrite(&block, sizeof(RentalBlock), 1, file);

    tof->metadata.lastID = rental.rentalID;
    writeRentalMetadata(tof);

    fclose(file);
    return rental.rentalID;
}

int searchRentalByID(TOF *tof, int rentalID, Rental *foundRental) {
    if (rentalID <= 0 || rentalID > tof->metadata.lastID) return -1;

    int index = rentalID - 1;
    int blockIndex = index / 10;
    int pos = index % 10;

    if (blockIndex >= tof->metadata.numBlocks) return -1;

    FILE *file = fopen(tof->filename, "rb");
    if (!file) return -1;

    long offset = sizeof(Metadata) + blockIndex * sizeof(RentalBlock)
                  + pos * sizeof(Rental);
    fseek(file, offset, SEEK_SET);
    if (fread(foundRental, sizeof(Rental), 1, file) != 1) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int cancelRental(TOF *tof, int rentalID) {
    if (rentalID <= 0 || rentalID > tof->metadata.lastID) return -1;
    int index = rentalID - 1;
    int blockIndex = index / 10;
    int pos = index % 10;
    if (blockIndex >= tof->metadata.numBlocks) return -1;

    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return -1;

    long offset = sizeof(Metadata) + blockIndex * sizeof(RentalBlock)
                  + pos * sizeof(Rental);

    Rental r;
    fseek(file, offset, SEEK_SET);
    if (fread(&r, sizeof(Rental), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    r.active = -1;
    fseek(file, offset, SEEK_SET);
    fwrite(&r, sizeof(Rental), 1, file);
    fclose(file);
    return 0;
}

void printRental(const Rental *rental) {
    printf("Rental ID: %d\n", rental->rentalID);
    printf("Customer ID: %d\n", rental->customerID);
    printf("Game ID: %d\n", rental->gameID);
    printf("Status: %s\n", (rental->active == 1) ? "Active" : "Cancelled");
}
