

#define FAIL_ON_WITH(value1, ret, value2)                                      \
    do {                                                                       \
        if((value1) == (value2)) {                                             \
            return ret;                                                        \
        }                                                                      \
    } while(0)

#define FINE_ON_WITH(value1, ret, value2)                                      \
    do {                                                                       \
        if((value1) != (value2)) {                                             \
            return ret;                                                        \
        }                                                                      \
    } while(0)
