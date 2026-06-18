#include <aws/common/string.h>
#include <stddef.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_secure_harness(void) {
    /* nondet length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024);

    /* allocate storage for struct + flexible array on the stack */
    uint8_t storage[sizeof(struct aws_string) + 1024 - 1];
    struct aws_string *str = (struct aws_string *)storage;

    /* set fields */
    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    /* initialize bytes with nondet data */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* assume the string is valid according to the library's predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* call the function under test */
    aws_string_destroy_secure(str);
}
