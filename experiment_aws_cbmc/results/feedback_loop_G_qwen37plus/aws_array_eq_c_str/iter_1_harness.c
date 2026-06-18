#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness(void) {
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    char *c_str = malloc(array_len + 1);
    __CPROVER_assume(c_str != NULL);

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    bool expected = true;
    for (size_t i = 0; i < array_len; ++i) {
        if (c_str[i] == '\0' || array[i] != (uint8_t)c_str[i]) {
            expected = false;
            break;
        }
    }
    if (expected) {
        expected = (c_str[array_len] == '\0');
    }

    assert(result == expected);
}
