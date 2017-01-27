#include <stdlib.h>
#include <stdio.h>

bool binarySearch(int array[], int Size, int key, int& location) //From https://en.wikibooks.org/wiki/Algorithm_Implementation/Search/Binary_search#C.2B.2B_.28common_Algorithm.29
{
	int low = 0, high = Size - 1, midpoint = 0;
	while (low <= high)
	{
		midpoint = low + (high - low) / 2;
		if (key == array[midpoint])
		{
			location = midpoint;
			return true;
		}
		else if (key< array[midpoint])
			high = midpoint - 1;
		else
			low = midpoint + 1; //when key is > array[midpoint]
	}
	return false;
}

int main()
{
	int loc;
	printf("hello world \n");
	int a[] = {1,3,6,7,8,9};
	if (binarySearch(a, 6, 6, loc)) {
		printf("Found at %d \n", loc);
	}
	system("PAUSE");
	return 0;
}