#include <stdio.h>
#include <stdlib.h>

#define bye(s) ({ \
    puts(s);      \
    return 1;     \
})

void bubbleSort(int* array, size_t size);

int main(int argc, char** argv) {
    
    if (argc <= 2)
    {
        printf("No sorting is needed\nFormat: %s <numbers seperated by spaces>\n", argv[0]);
        return 1;
    }

    int* array = malloc((argc - 1) * sizeof(int));
    if (!array) bye("Memory allocation failed");

    for (int i = 1; i < argc; i++)
    {
        sscanf(argv[i], "%d", &array[i - 1]);
    }

    bubbleSort(array, argc - 1);

    for (int i = 0; i < argc - 1; i++)
    {
        printf("%d ", array[i]);
    }

    puts("");

    return 0;

}

void bubbleSort(int* array, size_t size) {
    
    for (size_t i = 0; i < size - 1; i++) {
        
        for (size_t j = 0; j < size - 1 - i; j++) {

            if (array[j] > array[j + 1]) {

                int item = array[j];
                array[j] = array[j + 1];
                array[j + 1] = item;

            }

        }

    }

}