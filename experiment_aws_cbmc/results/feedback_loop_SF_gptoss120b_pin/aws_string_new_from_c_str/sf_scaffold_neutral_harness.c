#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic C string */
    char *c_str_buf;
    size_t max_len = nondet_uint();
    __CPROVER_assume(max_len < 256);
    c_str_buf = malloc(max_len + 1);
    __CPROVER_assume(c_str_buf != NULL);
    for (size_t i = 0; i < max_len; ++i) {
        c_str_buf[i] = (char)nondet_uint8_t();
    }
    size_t null_pos = nondet_uint();
    __CPROVER_assume(null_pos <= max_len);
    c_str_buf[null_pos] = '\0';
    const char *c_str = c_str_buf;

    /* preconditions */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* snapshot of input state */
    size_t orig_len = 0;
    while (c_str[orig_len] != '\0') {
        ++orig_len;
    }
    char *c_str_snapshot = malloc(orig_len);
    __CPROVER_assume(c_str_snapshot != NULL);
    for (size_t i = 0; i < orig_len; ++i) {
        c_str_snapshot[i] = c_str[i];
    }

    /* call under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    
}
