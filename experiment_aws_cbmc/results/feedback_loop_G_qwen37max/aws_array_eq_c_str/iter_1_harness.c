#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_array_eq_c_str_harness() {
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    } else {
        if (nondet_bool()) {
            array = malloc(1);
        }
    }

    /* Allocate a sufficiently large buffer for c_str to prevent CBMC from
     * flagging out-of-bounds reads if array_len > c_str_len, since the
     * implementation reads str_bytes[i] up to array_len before checking for '\0'. */
    char *c_str = malloc(MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(c_str != NULL);

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    c_str[c_str_len] = '\0';

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    if (result) {
        assert(array_len == c_str_len);
        assert(array_len == strlen(c_str));
    }
}
