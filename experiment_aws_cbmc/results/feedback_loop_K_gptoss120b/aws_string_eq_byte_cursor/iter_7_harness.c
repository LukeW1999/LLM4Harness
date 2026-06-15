#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string str_obj;
    struct aws_byte_cursor cur_obj;

    struct aws_string *str = nondet_pointer();
    if (str != NULL) {
        str = &str_obj;
        str->allocator = allocator;
        str->len = nondet_uint();
        __CPROVER_assume(str->len <= MAX_LEN);
        if (str->len == 0) {
            str->bytes = NULL;
        } else {
            str->bytes = aws_mem_acquire(allocator, str->len);
            __CPROVER_assume(str->bytes != NULL);
        }
    }

    struct aws_byte_cursor *cur = nondet_pointer();
    if (cur != NULL) {
        cur = &cur_obj;
        cur->len = nondet_uint();
        __CPROVER_assume(cur->len <= MAX_LEN);
        if (cur->len == 0) {
            cur->ptr = NULL;
        } else {
            cur->ptr = aws_mem_acquire(allocator, cur->len);
            __CPROVER_assume(cur->ptr != NULL);
        }
    }

    __CPROVER_assume(str == NULL ||
                     (str->len <= MAX_LEN &&
                      (str->len == 0 ? str->bytes == NULL : str->bytes != NULL)));
    __CPROVER_assume(cur == NULL ||
                     (cur->len <= MAX_LEN &&
                      (cur->len == 0 ? cur->ptr == NULL : cur->ptr != NULL)));

    bool result = aws_string_eq_byte_cursor(str, cur);

    if (str == NULL && cur == NULL) {
        __CPROVER_assert(result == true,
                         "Result should be true when both arguments are NULL");
    } else if (str == NULL || cur == NULL) {
        __CPROVER_assert(result == false,
                         "Result should be false when exactly one argument is NULL");
    } else {
        bool eq = (str->len == cur->len) &&
                  (str->len == 0 || memcmp(str->bytes, cur->ptr, str->len) == 0);
        __CPROVER_assert(result == eq,
                         "Result should reflect byte-wise equality");
    }
}
