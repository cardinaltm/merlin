/*
 * fuzzy.c
 *
 *  Created on: Mar 31, 2022
 *      Author: cardinal
 */

#include "fuzzy.h"

/*
 * степень принадлежности uA(x)
 * в какой степени (мере) элемент x принадлежит нечеткому множеству A.
 * \param[out] мера принадлежности
 * \param[in] x элемент из X
 * \param[in] A нечеткое множество A
 */
double mu(int8_t x, int8_t A)
{
	return exp(-(pow(x - A, 2) / (2 * pow(30, 2))));
}

/*
 * добавить правила к регулятору
 * \param[out]
 * \param[in] ife значение лингвистической переменной "отклонение" для ошибки
 * \param[in] op оператор и/или
 * \param[in] deltae значение лингвистической переменной "отклонение" первой разности ошибки
 * \param[in] then значение лингвистической переменной "воздействие"
 */
double addrule(int8_t fe, op_type op, int8_t fde, int8_t z)
{
	rules[numofrules].fe = fe;
	rules[numofrules].op = op;
	rules[numofrules].fde = fde;
	rules[numofrules].z = z;
	numofrules++;
}

/*
 * решение нечеткого регулятора. нечеткий вывод
 * \param[out] управляюшее воздействие
 * \param[in] e текушая ошибка
 * \param[in] de первая разница ошибок
 */
double getFuzzyConclusion(int8_t e, int8_t de)
{
	double summ_alpha_c = 0, sum_alpha = 0;
	// цикл по правилам
	for (int i = 0; i < (int) (numofrules); i++)
	{
		double alpha = 0, mue = 0, mude = 0;
		// степень соответствия ошибки нечеткому множеству fe для i-го правила
		mue = mu(e, rules[i].fe);
		// степень соответствия первой разности ошибки нечеткому множеству fde для i-го правила
		mude = mu(de, rules[i].fde);
		// применяем логический оператор и/или
		alpha = rules[i].op == 0 ? MIN(mue, mude) : MAX(mue, mude);
		// числитель и знаменатель для дискретного варианта
		// центроидного метода приведения к четкости
		summ_alpha_c += (alpha * rules[i].z);
		summ_alpha += alpha;
	}

	// вичисляем воздействие на обьект управления
	return summ_alpha_c / summ_alpha;
}

void Fuzzy_Init(void)
{
	numofrules = 0;

	addrule(fNO, AND, fNO, fNO);
	addrule(fVLN, OR, fVLN, fVLP);
	addrule(fVLP, OR, fVLP, fVLN);
	addrule(fLN, AND, fSN, fVLP);
	addrule(fLP, AND, fSP, fVLN);
	addrule(fSN, AND, fSN, fSP);
	addrule(fSP, AND, fSP, fSN);

	addrule(fMN, AND, fMP, fSP);
	addrule(fMN, AND, fMN, fLP);
	addrule(fMP, AND, fMN, fLP);
	addrule(fMP, AND, fMP, fLN);

//	for (int i = 0; i < (int) (numofrules); i++)
//	{
//		printf("IF e=%i %s deltae %i THEN output=%i\r\n", rules[i].fe, (rules[i].op == 0 ? "AND" : "OR"), rules[i].fde, rules[i].z);
//	}
//
//	// от обьекта управления
//	int8_t e = 50;
//	int8_t de = -30;
//	printf("e=%i; deltae=%i Z=%f\r\n", e, de, getFuzzyConclusion(e, de));
}

