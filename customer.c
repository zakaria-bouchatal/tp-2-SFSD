#include "customer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Internal helper functions
static void writeCustomerMetadata(TOF *tof) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return;
    fseek(file, 0, SEEK_SET);
    fwrite(&tof->metadata, sizeof(Metadata), 1, file);
    fclose(file);
}

static void readCustomerMetadata(TOF *tof) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) return;
    fseek(file, 0, SEEK_SET);
    fread(&tof->metadata, sizeof(Metadata), 1, file);
    fclose(file);
}

int openCustomerFile(TOF *tof, const char *filename, char mode) {
    strcpy(tof->filename, filename);
    FILE *file;

    if (mode == 'N') {
        file = fopen(filename, "w+b");
        if (!file) {
            perror("Error creating customer file");
            return -1;
        }
        tof->metadata.numBlocks = 0;
        tof->metadata.lastID = 0;
        fwrite(&tof->metadata, sizeof(Metadata), 1, file);
        fclose(file);
    } else if (mode == 'A') {
        file = fopen(filename, "r+b");
        if (!file) {
            perror("Error opening customer file");
            return -1;
        }
        fclose(file);
        readCustomerMetadata(tof);
    } else {
        fprintf(stderr, "Invalid mode for customer file: %c\n", mode);
        return -1;
    }
    return 0;
}

int insertCustomer(TOF *tof, Customer customer) {
    FILE *file = fopen(tof->filename, "r+b");
    if (!file) {
        perror("Error inserting customer");
        return -1;
    }

    CustomerBlock block;
    int blockIndex = tof->metadata.numBlocks - 1;
    if (blockIndex < 0) {
        blockIndex = 0;
        block.numCustomers = 0;
        tof->metadata.numBlocks = 1;
    } else {
        fseek(file, sizeof(Metadata) + blockIndex * sizeof(CustomerBlock), SEEK_SET);
        if (fread(&block, sizeof(CustomerBlock), 1, file) != 1) {
            printf("Error reading last customer block. Initializing a new one.\n");
            block.numCustomers = 0;
        }
    }

    customer.customerID = tof->metadata.lastID + 1;

    if (block.numCustomers < 10) {
        block.customers[block.numCustomers++] = customer;
    } else {
        // Need a new block
        blockIndex++;
        block.numCustomers = 1;
        block.customers[0] = customer;
        tof->metadata.numBlocks++;
    }

    fseek(file, sizeof(Metadata) + blockIndex * sizeof(CustomerBlock), SEEK_SET);
    fwrite(&block, sizeof(CustomerBlock), 1, file);

    tof->metadata.lastID = customer.customerID;
    writeCustomerMetadata(tof);

    fclose(file);
    return customer.customerID;
}

int searchCustomerByID(TOF *tof, int customerID, Customer *foundCustomer) {
    if (customerID <= 0 || customerID > tof->metadata.lastID) return -1;

    int index = customerID - 1;
    int blockIndex = index / 10;
    int pos = index % 10;

    if (blockIndex >= tof->metadata.numBlocks) return -1;

    FILE *file = fopen(tof->filename, "rb");
    if (!file) return -1;

    long offset = sizeof(Metadata) + blockIndex * sizeof(CustomerBlock) 
                  + pos * sizeof(Customer);
    fseek(file, offset, SEEK_SET);
    if (fread(foundCustomer, sizeof(Customer), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

void printCustomer(const Customer *customer) {
    printf("Customer ID: %d\n", customer->customerID);
    printf("First Name: %s\n", customer->fname);
    printf("Last Name: %s\n", customer->lname);
    printf("Contact Info: %s\n", customer->coninfo);
}

void displayAllCustomers(TOF *tof) {
    FILE *file = fopen(tof->filename, "rb");
    if (!file) {
        perror("Error opening customer file for display");
        return;
    }

    fseek(file, sizeof(Metadata), SEEK_SET);

    for (int b = 0; b < tof->metadata.numBlocks; b++) {
        CustomerBlock block;
        if (fread(&block, sizeof(CustomerBlock), 1, file) != 1) break;
        for (int i = 0; i < block.numCustomers; i++) {
            printf("Customer ID: %d, Name: %s %s\n",
                   block.customers[i].customerID,
                   block.customers[i].fname,
                   block.customers[i].lname);
        }
    }

    fclose(file);
}

