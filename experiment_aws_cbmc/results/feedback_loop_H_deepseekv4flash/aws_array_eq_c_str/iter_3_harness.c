#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_array_eq_c_str_harness() {
    size_t array_len;
    __CPROVER_assume(array_len < MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = (uint8_t *)malloc(array_len);
        __CPROVER_assume(array != NULL);
        for (size_t i = 0; i < array_len; i++) {
            array[i] = nondet_uint8_t();
        }
    }

    size_t c_str_len = array_len + 1;
    char *c_str = (char *)malloc(c_str_len);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < array_len; i++) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[array_len] = '\0';

    bool result = aws_array_eq_c_str((array_len == 0) ? NULL : (const void *)array, array_len, (const char *)c_str);

    bool expected = true;
    if (array_len == 0) {
        expected = (c_str[0] == '\0');
    } else {
        for (size_t i = 0; i < array_len; i++) {
            if (array[i] == '\0' || array[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
        }
        if (expected) {
            expected = (c_str[array_len] == '\0');
        }
    }
    assert(result == expected);
}
