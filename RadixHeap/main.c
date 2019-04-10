#include <unistd.h>
//#define checkMemoryLeak

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include "RadixHeap.h"

//////////////////////////////////////////////////
typedef struct Element {
    int value;
} Element_t;

//////////////////////////////////////////////////
void test(void);
Element_t *createElement(int value);
bool destroyElement(Element_t *element);

//////////////////////////////////////////////////
int main(int argc, const char * argv[]) {
    
#ifdef checkMemoryLeak
    while (true) {
        test();
        sleep(1);
    }
#else
    test();
#endif
    
    return EXIT_SUCCESS;
}

void test() {
    printf("=== start [%s] ===\n", __func__);
    
    int numOfValue = 99;
    srand((unsigned) time(NULL));
    
    RH_t *R = createRadixHeap(RAND_MAX-1);
    if (R == NULL) {
        printf("error [%s] : could not create a Radix Heap.\n", __func__);
        return;
    }
    
    //    printRadixHeapStructure(R); // for debug
    
    printf("*** insert [%s] ***\n", __func__);
    for (int j=0; j<1; j++) {
        for (int k=0; k<numOfValue; k++) {
            int value = rand();
            if (value >= INT_MAX) {
                value = INT_MAX - 1;
            }
            Element_t *element = createElement(value);
            printf("insert #%4d : %10d\n", k+1, element->value);
            bool result = insertIntoRadixHeap(R, value, element);
            if (!result) {
                printf("error [%s] : could not insert value %d.\n", __func__, element->value);
                destroyElement(element);
            }
        }
    }
    
    printf("*** peek min [%s] ***\n", __func__);
    Element_t *element = peekMinOnRadixHeap(R);
    printf("peek min : %10d\n", element->value);
    
    //        printRadixHeapStructure(R); // for debug
    
    printf("*** pull [%s] ***\n", __func__);
    int count = 0;
    while (!isEmptyRadixHeap(R)) {
        count++;
        Element_t *element = pullMinOnRadixHeap(R);
        printf("pull #%4d : %10d\n", count, element->value);
        destroyElement(element);
    }
    
    //        printRadixHeapStructure(R); // for debug
    
    destroyRadixHeap(R, RH_OPTION_WITH_ELEMENT);
    
    printf("=== end [%s] ===\n", __func__);
}

Element_t *createElement(int value) {
    Element_t *element = malloc(sizeof(Element_t));
    if (element == NULL) return NULL;
    element->value = value;
    return element;
}

bool destroyElement(Element_t *element) {
    // Block illegal parameters.
    if (element == NULL) return false;
    free(element);
    return true;
}
