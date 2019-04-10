#ifndef RadixHeap_h
#define RadixHeap_h

#include <stdbool.h>
#include "DualyLinkedList.h"

//////////////////////////////////////////////////
typedef enum RHOption {
    RH_OPTION_NONE,
    RH_OPTION_WITH_ELEMENT
} RH_OPTION_e;

typedef struct RadixHeapListElement {
    int value;
    void *element;
} RHE_t;

typedef struct RadixHeapCell {
    int beginOfRange;
    DLL_t *list;
} RHC_t;

typedef struct RadixHeap {
    int num;
    int lengthOfArray;
    RHC_t *array;
} RH_t;

//////////////////////////////////////////////////
// prototype
RH_t *createRadixHeap(int maxValue);
void initializeRadixHeap(RH_t *R, int initialValue, int length);
bool destroyRadixHeap(RH_t *R, RH_OPTION_e option);
bool insertIntoRadixHeap(RH_t *R, int value, void *element);
bool insertIntoRadixHeapByRange(RH_t *R, RHE_t *rhElement, int length);
void *pullMinOnRadixHeap(RH_t *R);
void *peekMinOnRadixHeap(RH_t *R);
bool redistributeElementOnRadixHeap(RH_t *R);
int findMinValueOnListOfRadixHeap(DLL_t *list);
bool isEmptyRadixHeap(RH_t *R);
RHE_t *createElementOnRadixHeap(int value, void *element);
bool destroyElementOnRadixHeap(RHE_t *element, int option);
void printStructureOnRadixHeap(RH_t *R);  // for debug

#endif
