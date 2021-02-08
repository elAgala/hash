#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define COLOR_MAGENTA "\x1b[35m\x1b[1m"
#define COLOR_CYAN "\x1b[36m\x1b[1m"
#define COLOR_YELLOW "\x1b[33m\x1b[1m"
#define COLOR_RED "\x1b[31m\x1b[1m"
#define COLOR_GREEN "\x1b[32m\x1b[1m"
#define COLOR_RESET "\x1b[0m"

#define MAX_NAME 50
#define MAX_TITLE 50

#define PASSED COLOR_GREEN"PASSED"COLOR_RESET
#define FAILED COLOR_RED"FAILED"COLOR_RESET

typedef bool (*comparator)(void*, void*);

typedef struct test{
	char name[MAX_NAME];
	bool status;
}test_t;

typedef struct category{
	char title[MAX_TITLE];
	test_t** tests;
	int testsPassed;
	int total;
}category_t;

typedef struct tester{
	int passed;
	int total;
	int totalCategories;
	category_t** categories;
}tester_t;

tester_t* createNewTester(){
	return calloc(1, sizeof(tester_t));
}

void createNewCategory(tester_t* tester, char title[MAX_TITLE]){
	tester->categories = realloc(tester->categories, ((size_t)tester->totalCategories + 1) * sizeof(category_t));
	tester->categories[tester->totalCategories] = calloc(1, sizeof(category_t));
	strcpy(tester->categories[tester->totalCategories]->title, title);
	tester->totalCategories++;
}

void test(tester_t* tester, char name[MAX_NAME], void* result, void* expected, comparator compare){
   category_t* category = tester->categories[tester->totalCategories - 1];
	category->tests = realloc(category->tests, ((size_t)category->total + 1) * sizeof(test_t));
	category->tests[category->total] = calloc(1, sizeof(category_t));
	strcpy(category->tests[category->total]->name, name);
	category->tests[category->total]->status = compare(result, expected);
	if(category->tests[category->total]->status){
		category->testsPassed++;
		tester->passed++;
	}
	category->total++;
	tester->total++;
}

void printTest(test_t* test, int id){
	printf("\nTEST:"COLOR_CYAN" %i"COLOR_RESET"| %s %s\n", id, test->name, test->status?PASSED:FAILED);
}

void printCategoryStats(category_t* category){
	int result = (category->testsPassed * 100)/category->total;
	printf("\nCategory results: %i out of %i (%s%i"COLOR_RESET"%%)\n", category->testsPassed, category->total, result==100?COLOR_GREEN:COLOR_RED, result);
}

void printGlobalStats(tester_t* tester){
	int result = (tester->passed * 100)/tester->total;
	printf("\nGlobal results: %i out of %i (%s%i"COLOR_RESET"%%)\n", tester->passed, tester->total, result==100?COLOR_GREEN:COLOR_RED, result);
}

void showResults(tester_t* tester){
	for(int i = 0; i < tester->totalCategories; i++){
		printf(COLOR_MAGENTA"\n\n%s\n\n"COLOR_RESET, tester->categories[i]->title);
		for(int j = 0; j < tester->categories[i]->total; j++)
			printTest(tester->categories[i]->tests[j], j + 1);
		printCategoryStats(tester->categories[i]);
	}
	printGlobalStats(tester);
}

void freeTester(tester_t* tester){
	for(int i = 0; i < tester->totalCategories; i++){
		for(int j = 0; j < tester->categories[i]->total; j++)
			free(tester->categories[i]->tests[j]);
		free(tester->categories[i]->tests);
		free(tester->categories[i]);
	}
	free(tester->categories);
	free(tester);
}

bool compareInt(void* a, void* b){
	int* x = (int*)a;
	int* y = (int*)b;

	if(*x != *y)
		return false;
	return true;
}

bool compareChar(void* a, void* b){
	char* x = (char*)a;
	char* y = (char*)b;

	if(*x != *y)
		return false;
	return true;
}

bool compareBool(void* a, void* b){
	bool* x = (bool*)a;
	bool* y = (bool*)b;
	return *x == *y;
}

