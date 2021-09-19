#include <stdio.h>

typedef struct {
    char data[4096];
    int key;
} item;

item array[] = {
    {"bill", 3},
    {"neil", 4},
    {"john", 2},
    {"rick", 5},
    {"alex", 1},
};

void sort(item *a, int n) {
    int i = 0, j = 0;
    item* p;

    for(; i < n; i++) {
        p = a+i;
        j = 1;
        while (j < n-i) {
            if( p->key > (p+j)->key) {
                item t = *p;
                *p  = *(p+j);
                *(p+j) = t;
            }
	    ++j;
        }
    }
}

int main() {
    int i;
    sort(array,5);
    for(i = 0; i < 5; i++)
        printf("array[%d] = {%s, %d}\n",
                i, array[i].data, array[i].key);
    return 0;
}
