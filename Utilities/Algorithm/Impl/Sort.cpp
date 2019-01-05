#include <algorithm>

#include "..\..\Include\Sort.h"

using namespace std;

int partition(int arr[], int left, int right)  //找基准数 划分
{
	int i = left + 1;
	int j = right;
	int temp = arr[left];

	while (i <= j)
	{
		while (arr[i] < temp)
		{
			i++;
		}
		while (arr[j] > temp)
		{
			j--;
		}
		if (i < j)
			swap(arr[i++], arr[j--]);
		else i++;
	}
	swap(arr[j], arr[left]);
	return j;

}

void quick_sort_method1(int arr[], int left, int right)
{
	if (left > right)
		return;
	int j = partition(arr, left, right);
	quick_sort_method1(arr, left, j - 1);
	quick_sort_method1(arr, j + 1, right);
}


template<typename T>
void quicksort(T data[], int first, int last)
{
	int lower = first + 1;
	int upper = last;
	swap(data[first], data[(first + last) / 2]);
	T bound = data[first];
	while (lower <= upper)
	{
		while (data[lower] < bound)
			lower++;
		while (data[upper] > bound)
			upper--;
		if (lower < upper)
			swap(data[lower++], data[upper--]);
		else lower++;
	}
	swap(data[upper], data[first]);
	if (first < upper - 1)
		quicksort(data, first, upper - 1);
	if (upper + 1 < last)
		quicksort(data, upper + 1, last);
}

template<class T>
void quicksort(T data[], int n)
{
	int i, max;
	if (n < 2)
		return;
	for (i = 1, max = 0; i < n; i++)
		if (data[max] < data[i])
			max = i;
	swap(data[n - 1], data[max]);
	quicksort(data, 0, n - 2);  //  
}