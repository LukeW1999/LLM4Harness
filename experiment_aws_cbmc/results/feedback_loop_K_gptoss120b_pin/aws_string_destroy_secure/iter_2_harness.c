#include <aws/common/string.h>
#include <assert.h>
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
    str->allocator = NULL;
    str->len = len;

    /* initialize bytes with nondet data */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* keep a copy of original bytes for later comparison */
    uint8_t orig_bytes[1024];
    for (size_t i = 0; i < len; ++i) {
        orig_bytes[i] = ((uint8_t *)str->bytes)[i];
    }

    /* assume the string is valid according to the library's predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* call the function under test */
    aws_string_destroy_secure(str);

    /* post‑conditions */
    if (str != NULL && str->allocator == NULL) {
        /* bytes must be zeroed */
        for (size_t i = 0; i < len; ++i) {
            assert(((uint8_t *)str->bytes)[i] == 0);
        }
        /* struct fields unchanged */
        assert(str->len == len);
        assert(str->allocator == NULL);
    }
}
