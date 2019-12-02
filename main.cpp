#include <main.h>

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
 __attribute__(( constructor )) void premain()
{
    init();
}

int main(void) {

    setup();
    
    while (1) {
        loop();
    }
    return 0;
}
