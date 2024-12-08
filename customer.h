#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "struct.h"

// Opens or creates a customer file
int openCustomerFile(TOF *tof, const char *filename, char mode);

// Inserts a new customer and returns its generated ID
int insertCustomer(TOF *tof, Customer customer);

// Searches for a customer by ID
// Returns 0 if found, -1 otherwise
int searchCustomerByID(TOF *tof, int customerID, Customer *foundCustomer);

// Prints out a single customer's details
void printCustomer(const Customer *customer);

// Displays all customers (ID and names)
void displayAllCustomers(TOF *tof);

#endif
