#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

extern size_t nondet_size_t(void);

void aws_byte_buf_eq_c_str_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf buf;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= SIZE_MAX - 1);
    buf.capacity = capacity;

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    buf.len = len;

    if (len > 0) {
        buf.buffer = __CPROVER_allocate(len, 0);
        __CPROVER_assume(__CPROVER_is_fresh(buf.buffer, len));
    } else {
        buf.buffer = NULL;
    }
    buf.allocator = allocator;

    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= SIZE_MAX - 1);
    char *c_str = __CPROVER_allocate(c_len + 1, 0);
    __CPROVER_assume(__CPROVER_is_fresh(c_str, c_len + 1));
    c_str[c_len] = '\0';

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);
    (void)result;
    return 0;
}
