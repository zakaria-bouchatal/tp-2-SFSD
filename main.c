#include <stdio.h>
#include <stdlib.h>
#include "customer.h"
#include "game.h"
#include "rental.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"

static void printBanner() {
    printf("========================================\n");
    printf(BOLD "       CUSTOMER & GAME RENTAL SYSTEM\n" RESET);
    printf("========================================\n\n");
}

static void printMenu() {
    printf(GREEN "1." RESET " Add Customer\n");
    printf(GREEN "2." RESET " Add Game\n");
    printf(GREEN "3." RESET " Create Rental\n");
    printf(GREEN "4." RESET " Search Rental by ID\n");
    printf(GREEN "5." RESET " Cancel Rental\n");
    printf(GREEN "6." RESET " Display All Customers\n");
    printf(GREEN "7." RESET " Display All Games\n");
    printf(GREEN "8." RESET " Exit\n\n");
    printf("Enter choice: ");
}

int main() {
    TOF customerTOF, gameTOF, rentalTOF;

    if (openCustomerFile(&customerTOF, "customers.dat", 'A') == -1) return EXIT_FAILURE;
    if (openGameFile(&gameTOF, "games.dat", 'A') == -1) return EXIT_FAILURE;
    if (openRentalFile(&rentalTOF, "rentals.dat", 'A') == -1) return EXIT_FAILURE;

    // Print the banner once at the start
    printBanner();

    int choice;
    while (1) {
        printMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            printf("----------------------------------------\n");
            // Since no input buffer cleanup is done, if invalid input is entered,
            // future scans may fail. This is a limitation of not clearing the buffer.
            continue;
        }

        if (choice == 1) {
            Customer cu;
            printf("Enter First Name: ");
            scanf("%s", cu.fname);
            printf("Enter Last Name: ");
            scanf("%s", cu.lname);
            printf("Enter Contact Info: ");
            scanf("%s", cu.coninfo);

            int cid = insertCustomer(&customerTOF, cu);
            if (cid > 0) {
                printf("Customer added successfully! ID: %d\n", cid);
            } else {
                printf("Error adding customer.\n");
            }

        } else if (choice == 2) {
            Game g;
            printf("Enter Game Title: ");
            scanf(" %[^\n]", g.title);
            g.borrowerID = -1;
            int gid = insertGame(&gameTOF, g);
            if (gid > 0) {
                printf("Game added successfully! ID: %d\n", gid);
            } else {
                printf("Error adding game.\n");
            }

        } else if (choice == 3) {
            int cid, gid;
            printf("Enter Customer ID: ");
            scanf("%d", &cid);
            printf("Enter Game ID: ");
            scanf("%d", &gid);

            Game foundGame;
            if (searchGameByID(&gameTOF, gid, &foundGame) == 0) {
                if (foundGame.borrowerID == -1) {
                    int rid = insertRental(&rentalTOF, cid, gid);
                    if (rid > 0) {
                        foundGame.borrowerID = cid;
                        if (updateGame(&gameTOF, foundGame) == 0) {
                            printf("Rental created. Rental ID: %d\n", rid);
                        } else {
                            printf("Error updating game borrower.\n");
                        }
                    } else {
                        printf("Error creating rental.\n");
                    }
                } else {
                    printf("Game is already borrowed by Customer ID: %d\n", foundGame.borrowerID);
                }
            } else {
                printf("Game not found.\n");
            }

        } else if (choice == 4) {
            int rid;
            printf("Enter Rental ID: ");
            scanf("%d", &rid);

            Rental r;
            if (searchRentalByID(&rentalTOF, rid, &r) == 0) {
                printRental(&r);

                Customer cst;
                Game gm;

                if (searchCustomerByID(&customerTOF, r.customerID, &cst) == 0) {
                    printf("Customer: %s %s\n", cst.fname, cst.lname);
                } else {
                    printf("Customer not found.\n");
                }

                if (searchGameByID(&gameTOF, r.gameID, &gm) == 0) {
                    printf("Game Title: %s\n", gm.title);
                } else {
                    printf("Game not found.\n");
                }
            } else {
                printf("Rental not found.\n");
            }

        } else if (choice == 5) {
            int rid;
            printf("Enter Rental ID to cancel: ");
            scanf("%d", &rid);

            Rental r;
            if (searchRentalByID(&rentalTOF, rid, &r) == 0) {
                if (r.active == 1) {
                    if (cancelRental(&rentalTOF, rid) == 0) {
                        Game foundGame;
                        if (searchGameByID(&gameTOF, r.gameID, &foundGame) == 0) {
                            foundGame.borrowerID = -1;
                            updateGame(&gameTOF, foundGame);
                        }
                        printf("Rental cancelled successfully.\n");
                    } else {
                        printf("Error cancelling rental.\n");
                    }
                } else {
                    printf("Rental is already cancelled.\n");
                }
            } else {
                printf("Rental not found.\n");
            }

        } else if (choice == 6) {
            printf("All Customers:\n");
            displayAllCustomers(&customerTOF);

        } else if (choice == 7) {
            printf("All Games:\n");
            displayAllGames(&gameTOF);

        } else if (choice == 8) {
            // Print goodbye and exit
            printf("Goodbye!\n");
            break;

        } else {
            printf("Invalid choice. Try again.\n");
        }

        printf("----------------------------------------\n");
    }

    return EXIT_SUCCESS;
}
