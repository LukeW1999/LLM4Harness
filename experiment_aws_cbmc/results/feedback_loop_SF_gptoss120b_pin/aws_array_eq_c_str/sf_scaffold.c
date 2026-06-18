#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_array_eq_c_str_harness(void) {
    /* Symbolic inputs */
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    const void *array = NULL;
    if (array_len > 0) {
        uint8_t *tmp = malloc(array_len);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            tmp[i] = nondet_uint8_t();
        }
        array = tmp;
    }

    /* c_str must have at least array_len + 1 bytes (including terminating NUL) */
    char *c_str = malloc(array_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < array_len; ++i) {
        c_str[i] = nondet_char();
    }
    c_str[array_len] = '\0';

    /* PRE-CALL SNAPSHOT */
    size_t snapshot_array_len = array_len;
    uint8_t *snapshot_array = NULL;
    if (array_len > 0) {
        snapshot_array = malloc(array_len);
        __CPROVER_assume(snapshot_array != NULL);
        for (size_t i = 0; i < array_len; ++i) {
            snapshot_array[i] = ((uint8_t *)array)[i];
        }
    }
    size_t snapshot_c_str_len = array_len + 1;
    char *snapshot_c_str = malloc(snapshot_c_str_len);
    __CPROVER_assume(snapshot_c_str != NULL);
    for (size_t i = 0; i < snapshot_c_str_len; ++i) {
        snapshot_c_str[i] = c_str[i];
    }

    /* Call the function under verification */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* ASSERT_POSTCONDITIONS_HERE */
}
