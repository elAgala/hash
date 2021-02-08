#ifndef __TESTER_H__
#define __TESTER_H__

#include <stdbool.h>

/*ADVERTENCIA ----> Modulo en fase beta, no mandar structuras nulas*/

#define MAX_NAME 50
#define MAX_TITLE 50

/*Function to compare the result with the expected one*/
typedef bool (*comparator)(void*, void*);

typedef struct tester tester_t;

/*Creates and reserves memory for tester_t structure*/
tester_t* createNewTester();

/*Creates a new category with title*/
void createNewCategory(tester_t* tester, char title[MAX_TITLE]);

/*Compares result with expected one and saves the status on actual category with its name.*/
void test(tester_t* tester, char name[MAX_NAME], void* result, void* expected, comparator compare);

/*Shows all the stats of each category*/
void showResults(tester_t* tester);

/*Frees the structure*/
void freeTester(tester_t* tester);

/*Some functions for comparision*/

bool compareInt(void* a, void* b);

bool compareChar(void* a, void* b);

bool compareBool(void* a, void* b);

#endif /*__TESTER_H__*/
