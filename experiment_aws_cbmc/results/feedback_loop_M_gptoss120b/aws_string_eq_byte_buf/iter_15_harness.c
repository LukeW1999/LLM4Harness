#include <stdbool.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str = NULL;
    struct aws_byte_buf buf;

    /* nondet allocation for the string */
    if (nondet_bool()) {
        ensure_string_is_allocated(&str, MAX_BUFFER_SIZE);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* nondet allocation for the byte buffer */
    if (nondet_bool()) {
        ensure_byte_buf_is_allocated(&buf, MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    } else {
        buf.buffer = NULL;
        buf.capacity = 0;
        buf.len = 0;
        buf.allocator = NULL;
    }

    /* invoke the function under verification */
    bool result = aws_string_eq_byte_buf(str, &buf);
    (void)result; /* suppress unused variable warning */
}
