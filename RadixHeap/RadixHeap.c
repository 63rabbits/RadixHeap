#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "RadixHeap.h"

//////////////////////////////////////////////////
RH_t *createRadixHeap(int maxValue) {
    // Block illegal parameters.
    if (maxValue < 0) return NULL;
    if (maxValue > INT_MAX-1) return NULL;
    
    RH_t *R = malloc(sizeof(RH_t));
    if (R == NULL) return NULL;
    R->num = 0;
    R->lengthOfArray = ceil(log2(maxValue + 1)) + 2;
    R->array = malloc(R->lengthOfArray * sizeof(RHC_t));
    if (R->array == NULL) {
        free(R);
        return NULL;
    }
    for (int i=0; i<R->lengthOfArray; i++) {
        R->array[i].list = createDLList();
    }
    
    // initialize parameter of arrays.
    initializeRadixHeap(R, 0, R->lengthOfArray);
    R->array[R->lengthOfArray-1].beginOfRange = INT_MAX;    // Mark as infinity set.
    
    return R;
}

void initializeRadixHeap(RH_t *R, int initialValue, int length) {
    int before = 0;
    int range = 1;
    int delta = 0;
    for (int i=0; i<length; i++) {
        // initialize begin value of range.
        switch (i) {
            case 0:
            case 1:
                delta = i;
                break;
            default:
                range = range << 1;
                delta = range;
                break;
        }
        int value = delta + initialValue;
        if (value < before) {   // happen overflow !!
            value = INT_MAX;
        }
        before = value;
        R->array[i].beginOfRange = value;
    }
}

bool destroyRadixHeap(RH_t *R, RH_OPTION_e option) {
    // Block illegal parameters.
    if (R == NULL) return false;
    
    for (int i=0; i<R->lengthOfArray; i++) {
        DLL_t *list = R->array[i].list;
        while (!isEmptyDLList(list)) {
            RHE_t *rhElement = pullDLList(list);
            destroyElementOnRadixHeap(rhElement, RH_OPTION_WITH_ELEMENT);
        }
        destroyDLList(list, DLL_OPTION_NONE);
    }
    free(R->array);
    free(R);
    
    return true;
}

bool insertIntoRadixHeap(RH_t *R, int value, void *element) {
    RHE_t *rhElement = createElementOnRadixHeap(value, element);
    bool check = insertIntoRadixHeapByRange(R, rhElement, R->lengthOfArray);
    if (!check) {
        destroyElementOnRadixHeap(rhElement, RH_OPTION_NONE);
    }
    return check;
}

bool insertIntoRadixHeapByRange(RH_t *R, RHE_t *rhElement, int length) {
    bool result = false;
    for (int i=length-1; i>=0; i--) {
        if (R->array[i].beginOfRange <= rhElement->value) {
            result = insertAtTailOnDLList(R->array[i].list, rhElement);
            if (result) {
                R->num++;
            }
            return result;
        }
    }
    return result;
}

void *pullMinOnRadixHeap(RH_t *R) {
    // Blorck illegal parameters.
    if (R->num <= 0) return NULL;
    
    DLL_t *minList = R->array[0].list;
    if (isEmptyDLList(minList)) {
        // Since the leftmost bucket is empty, redistribution processing is performed.
        redistributeElementOnRadixHeap(R);
    }
    
    void *element = NULL;
    RHE_t *rhElement = pullHeadOnDLList(minList);
    if (rhElement != NULL) {
        R->num--;
        if (R->num <= 0) {
            initializeRadixHeap(R, 0, R->lengthOfArray);
        }
        element = rhElement->element;
        free(rhElement);
    }
    
    return element;
}

void *peekMinOnRadixHeap(RH_t *R) {
    // Blorck illegal parameters.
    if (R->num <= 0) return NULL;
    
    DLL_t *minList = R->array[0].list;
    if (isEmptyDLList(minList)) {
        // Since the leftmost bucket is empty, redistribution processing is performed.
        redistributeElementOnRadixHeap(R);
    }
    
    void *element = NULL;
    RHE_t *rhElement = peekElementHeadOnList(minList);
    if (rhElement != NULL) {
        element = rhElement->element;
    }
    
    return element;
}

bool redistributeElementOnRadixHeap(RH_t *R) {
    // search none empty cell.
    int noneEmptyIndex = 0;
    for (int i=0; i<R->lengthOfArray; i++) {
        if (!isEmptyDLList(R->array[i].list)) {
            noneEmptyIndex = i;
            break;
        }
    }
    // Distribution is impossible if there are no empty cells ahead.
    if (noneEmptyIndex <= 0) return false;
    
    DLL_t *leftMostlist = R->array[noneEmptyIndex].list;
    
    // change range of cells.
    int minValue = findMinValueOnListOfRadixHeap(leftMostlist);
    if (minValue >= INT_MAX) return false;

    initializeRadixHeap(R, minValue, noneEmptyIndex);
    int value = INT_MAX;
    if (noneEmptyIndex < R->lengthOfArray - 1) {
        value = R->array[noneEmptyIndex + 1].beginOfRange;
    }
    R->array[noneEmptyIndex].beginOfRange = value;    // Mark as empty set.

    // distribute elements.
    while (!isEmptyDLList(leftMostlist)) {
        RHE_t *rhElement = pullHeadOnDLList(leftMostlist);
        R->num--;
        insertIntoRadixHeapByRange(R, rhElement, noneEmptyIndex + 1);
    }
    
    return true;
}

int findMinValueOnListOfRadixHeap(DLL_t *list) {
    int minValue = INT_MAX;
    DLLC_t *cell = list->head;
    while (cell != NULL) {
        int value = ((RHE_t *)getElementOnDLList(cell))->value;
        if (value < minValue) {
            minValue = value;
        }
        cell = cell->next;
    }
    return minValue;
}

bool isEmptyRadixHeap(RH_t *R) {
    if (R->num <= 0) return true;
    return false;
}

RHE_t *createElementOnRadixHeap(int value, void *element) {
    // Block illegal parameters.
    if (value < 0) return false;
    if (element == NULL) return false;

    RHE_t *rhElement = malloc(sizeof(rhElement));
    if (rhElement == NULL) return NULL;
    rhElement->value = value;
    rhElement->element = element;
    return rhElement;
}

bool destroyElementOnRadixHeap(RHE_t *element, int option) {
    // Block illegal parameters.
    if (element == NULL) return false;
    if ((option == RH_OPTION_WITH_ELEMENT) &&
        (element->element != NULL)) {
        free(element->element);
    }
    free(element);
    return true;
}

void printStructureOnRadixHeap(RH_t *R) {
    printf("\n");
    printf("*** info : Radix Heap ***\n");
    
    printf("total number of elements = %d\n", R->num);
    for (int i=0; i<R->lengthOfArray; i++) {
        printf("\n");
        printf("array[%d]\n", i);
        printf("begin Of Range = %d\n", R->array[i].beginOfRange);
        DLL_t *list = R->array[i].list;
        DLLC_t *cell = list->head;
        int count = 0;
        while (cell != NULL) {
            if (count <= 0) printf("value of elements\n");
            count++;
            printf("%d, ", ((RHE_t*)cell->element)->value);
            cell = cell->next;
        }
        if (count > 0) {
            printf("\n");
            printf("number of elements on list = %d\n", count);
        }
    }
}
