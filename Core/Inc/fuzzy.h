/**
 *******************************************************************************
 * @file   fuzzy.h
 * @author Lasha Valishvili (cardinal_tm)
 * @email  lvalishvili@icloud.com
 * @date   Apr 1, 2022
 *******************************************************************************
 **/

#ifndef INC_FUZZY_H_
#define INC_FUZZY_H_

#include <stdint.h>
#include <math.h>

/*
 * значения лингвистических переменных "отклонение", "воздействие"
 */
#define fVLN -125 // очень большое отрицательное
#define fLN -90 // большое отрицательное
#define fMN -55 // среднее отрицательное
#define fSN -20 // малое отрицательное
#define fNO 0 // нулевое
#define fSP 20 // малое положительное
#define fMP 55 // среднее положительное
#define fLP 90 // большое положительное
#define fVLP 125 // очень силное положительное

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef enum
{
	AND, OR
} op_type;

extern uint8_t numofrules;

struct rule
{
	int8_t fe;
	op_type op;
	int8_t fde;
	int8_t z;
};

extern struct rule rules[12];

void Fuzzy_Init(void);
double getFuzzyConclusion(int8_t e, int8_t de);

#endif /* INC_FUZZY_H_ */
