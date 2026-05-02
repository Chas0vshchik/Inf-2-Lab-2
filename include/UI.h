#ifndef INF_2_LAB_2_UI_H
#define INF_2_LAB_2_UI_H

#include "Sequence.h"

using DataType = int;

void UI();

void printSequence(const Sequence<DataType> *seq);

Sequence<DataType> *createSequence();

#endif //INF_2_LAB_2_UI_H