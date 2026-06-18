#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void aws_array_eq_c_str_harness() {
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    void *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    } else {
        bool is_null = nondet_bool();
        if (is_null) {
            array = NULL;
        } else {
            array = malloc(1);
            __CPROVER_assume(array != NULL);
        }
    }

    char *c_str = malloc(array_len + 1);
    __CPROVER_assume(c_str != NULL);

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    if (result) {
        assert(c_str[array_len] == '\0');
        if (array_len > 0) {
            assert(memcmp(array, c_str, array_len) == 0);
        }
        assert(strlen(c_str) == array_len);
    }

    free(array);
    free(c_str);
}
