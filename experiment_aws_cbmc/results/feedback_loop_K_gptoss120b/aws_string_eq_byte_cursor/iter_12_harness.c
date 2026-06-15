#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_uint();
        __CPROVER_assume(len <= MAX_LEN);
        const uint8_t *buf = NULL;
        if (len > 0) {
            buf = (const uint8_t *)aws_mem_acquire(allocator, len);
            __CPROVER_assume(buf != NULL);
        }
        str = aws_string_new_from_array(allocator, buf, len);
        __CPROVER_assume(str != NULL);
    }

    struct aws_byte_cursor cur;
    struct aws_byte_cursor *cur_ptr;
    if (nondet_bool()) {
        cur_ptr = NULL;
    } else {
        cur_ptr = &cur;
        cur.len = nondet_uint();
        __CPROVER_assume(cur.len <= MAX_LEN);
        if (cur.len == 0) {
            cur.ptr = NULL;
        } else {
            cur.ptr = (const uint8_t *)aws_mem_acquire(allocator, cur.len);
            __CPROVER_assume(cur.ptr != NULL);
        }
    }

    bool result = aws_string_eq_byte_cursor(str, cur_ptr);

    if (str == NULL && cur_ptr == NULL) {
        __CPROVER_assert(result == true,
                         "Result should be true when both arguments are NULL");
    } else if (str == NULL || cur_ptr == NULL) {
        __CPROVER_assert(result == false,
                         "Result should be false when exactly one argument is NULL");
    } else {
        bool eq = (str->len == cur_ptr->len) &&
                  (str->len == 0 || memcmp(aws_string_bytes(str), cur_ptr->ptr, str->len) == 0);
        __CPROVER_assert(result == eq,
                         "Result should reflect byte-wise equality");
    }

    if (str) {
        aws_string_destroy(str);
    }
}
