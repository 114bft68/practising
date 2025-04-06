#include <stdio.h>
#include <stdlib.h>

#define bye(s) ({ \
    puts(s);      \
    return 1;     \
})

int binarySearch(int search, int* arr, size_t size);

int main(int argc, char** argv) {

    if (argc < 4) {

        printf("No searching is needed\nFormat: %s <search> <numbers...>\n", argv[0]);
        return 1;
    
    }
    
    int searchFor = 0;
    sscanf(argv[1], "%d", &searchFor);
    
    int* array = malloc((argc - 2) * sizeof(int));
    if (!array) bye("Memory allocation failed");

    for (int i = 2; i < argc; ++i) {

        sscanf(argv[i], "%d", &array[i - 2]);

        if (i == 2) continue;

        if (array[i - 3] > array[i - 2]) {
        
            free(array);
            bye("Sort your array in ascending order first");
        
        }

    }

    int looped = binarySearch(searchFor, array, (size_t) (argc - 2));
    printf("%s. Looped for %d times.\n", looped ? "Found" : "Not found", looped);

    free(array);
    return 0;

}

int binarySearch(int search, int* arr, size_t size) {
    int first   = 0,
        last    = size - 1,
        counter = 0; // not necessary for binary search

    while (first <= last) {
        
        int mid = (int) ((first + last) / 2);
        ++counter;

        if (arr[mid] == search) {
            
            return counter; // not necessary for binary search, it can return 1 (then no one knows how many loops were run)
            
        } else if (search < arr[mid]) {
            
            last = mid - 1;
            
        } else {
            
            first = mid + 1;

        }

    }
    
    return 0;
}