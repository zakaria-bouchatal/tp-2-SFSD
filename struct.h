#ifndef STRUCT_H
#define STRUCT_H


typedef struct {
    int customerID;
    char fname[50];    
    char lname[50];      
    char coninfo[50];    
} Customer;


typedef struct {
    Customer customers[10];
    int numCustomers;
} CustomerBlock;


typedef struct {
    int gameID;
    char title[50];
    int borrowerID; 
} Game;


typedef struct {
    Game games[10];
    int numGames;
} GameBlock;


typedef struct {
    int rentalID;
    int customerID;
    int gameID;
    int active;    // 1 = active -1 = canceled
} Rental;


typedef struct {
    Rental rentals[10];
    int numRentals;
} RentalBlock;


typedef struct {
    int numBlocks;
    int lastID;
} Metadata;


typedef struct {
    char filename[200];
    Metadata metadata;
} TOF;

#endif
