#pragma once

#include "SADXVariables.h"
#include "SADXEnums.h"

template<typename T>
constexpr int findValueInArray(const T* array, T value, int size)
{
	for (int i = 0; i < size; i++) {
		if (array[i] == value)
			return i;
	}

	return -1;
}

template<typename T>
constexpr bool isValueInArray(const T* array, T value, int size)
{
	return findValueInArray(array, value, size) != -1;
}
