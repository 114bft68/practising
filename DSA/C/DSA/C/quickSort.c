#include <stdio.h>
#include <stdlib.h>

#define bye(s) ({ \
    puts(s);      \
    return 1;     \
})

void quickSort(int* array, size_t size);

int main(int argc, char** argv) {
    
    if (argc <= 2)
    {
        printf("No sorting is needed\nFormat: %s <numbers seperated by spaces>\n", argv[0]);
        return 1;
    }

    int* array = malloc((argc - 1) * sizeof(int));
    if (!array) bye("Memory allocation failed");

    for (int i = 1; i < argc; ++i)
    {
        sscanf(argv[i], "%d", &array[i - 1]);
    }

    quickSort(array, argc - 1);

    for (int i = 0; i < argc - 1; ++i)
    {
        printf("%d ", array[i]);
    }

    puts("");

    return 0;

}


void quickSort(int* array, size_t size) {

    if (size <= 1) return;

    int pivot = array[(int) (size / 2)],
        start = 0,
        end   = size - 1;

    while (start < end) {

        while (array[start] < pivot) ++start;
        while (array[end] > pivot)   --end;

        if (start < end) {

            int temp = array[start];
            array[start++] = array[end];
            array[end--] = temp;

        } else if (start == end) {

            ++start;
            --end;

        }

    }

    quickSort(array, end + 1);
    quickSort(array + start, size - start);

}