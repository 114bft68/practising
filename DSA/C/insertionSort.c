#include <stdio.h>
#include <stdlib.h>

#define bye(s) ({ \
    puts(s);      \
    return 1;     \
})

void insertionSort(int* array, size_t size);

int main(int argc, char** argv) {

    if (argc <= 2)
    {
        printf("No sorting is needed\nFormat: %s <numbers seperated by spaces>\n", argv[0]);
        return 1;
    }

    int* array = malloc((argc - 1) * sizeof(int));
    if (!array) bye("Memory allocation failed");

    for (int i = 1; i < argc; ++i) {

        sscanf(argv[i], "%d", &array[i - 1]);

    }

    insertionSort(array, argc - 1);

    for (int i = 0; i < argc - 1; ++i)
    {
        printf("%d ", array[i]);
    }

    free(array);
    array = NULL;

    puts("");

    return 0;
}

void insertionSort(int* array, size_t size) {

    for (int i = 1; i < size; ++i) {

        int key = array[i],
            beforeKey = i - 1;

        while (beforeKey > -1 && key < array[beforeKey])
        {
            array[beforeKey + 1] = array[beforeKey];
            --beforeKey;
        }

        array[beforeKey + 1] = key;

    }

}