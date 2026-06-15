#include <assert.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        ensure_string_is_allocated(&str, MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }
