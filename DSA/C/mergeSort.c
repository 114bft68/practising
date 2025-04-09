#include <stdio.h>
#include <stdlib.h>

#define bye(s) ({ \
    puts(s);      \
    exit(1);     \
})

void mergeSort(int* array, size_t size);

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

    mergeSort(array, argc - 1);

    for (int i = 0; i < argc - 1; ++i)
    {
        printf("%d ", array[i]);
    }

    free(array);
    array = NULL;

    puts("");

    return 0;

}

// ascending order
void mergeSort(int* array, size_t size) {

    if (size <= 1) return;

    size_t mid = (size_t) (size / 2);

    mergeSort(array, mid);
    mergeSort(array + mid, size - mid);

    int *parallelArray = malloc(size * sizeof(int));
    if (!parallelArray) {
        free(array);
        bye("Memory allocation failed");
    }

    for (int i = 0; i < size; ++i)
    {
        parallelArray[i] = array[i];
    }

    int lhs = 0  , // left  hand side (first index) 
        rhs = mid, // right hand side (first index)
        idx = 0;   // index to the real array

    while (lhs < mid && rhs < size)
    {
        array[idx++] = parallelArray[parallelArray[lhs] > parallelArray[rhs] ? rhs++ : lhs++];
    }

    const int rlhs = mid - lhs , // remaining items on the LHS/ RHS
              rrhs = size - rhs;

    for (int i = 0; i < rlhs; ++i)
    {
        array[idx++] = parallelArray[lhs++];
    }

    for (int i = 0; i < rrhs; ++i)
    {
        array[idx++] = parallelArray[rhs++];
    }

    free(parallelArray);
    parallelArray = NULL;

}