#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_c_str_harness() {
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = (uint8_t *)malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; i++) {
            array[i] = nondet_uint8_t();
        }
    } else {
        array = NULL;
    }

    size_t c_str_len = array_len + 1;
    char *c_str = (char *)malloc(c_str_len);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < array_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[array_len] = '\0';

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    if (result) {
        assert(array_len == 0 || array != NULL);
        assert(c_str != NULL);
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
        }
        assert(c_str[array_len] == '\0');
    } else {
        bool mismatch = false;
        for (size_t i = 0; i < array_len; i++) {
            if (array[i] != (uint8_t)c_str[i]) {
                mismatch = true;
                break;
            }
        }
        if (!mismatch) {
            assert(c_str[array_len] != '\0');
        }
    }

    if (array_len > 0) {
        free(array);
    }
    free(c_str);
}
