#include "Utils.h"

#include "SADXVariables.h"
#include "SADXEnums.h"

bool isValueInArray(const enum LevelIDs *array, enum LevelIDs value, int size)
{
	for (int i = 0; i < size; i++) {
		if (array[i] == value)
			return true;
	}

	return false;
}