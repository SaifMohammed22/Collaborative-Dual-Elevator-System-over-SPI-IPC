#define TURN_OFF_SLAVE 1U  /* Set to 1 to turn off, 0 to enable */

int main(void) {
    #if TURN_OFF_SLAVE
        /* Safe, empty loop */
        while(1) {
        }
    #endif
}