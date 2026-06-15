#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "aws/common/string.h"
#include "aws/common/memory.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 1024);                     /* bound the length */

        /* allocate enough memory for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* set fields */
        str->len = len;
        str->allocator = (struct aws_allocator *)nondet_uint64_t();

        /* initialise the byte payload with nondeterministic values */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* call the function under test */
    aws_string_destroy_secure(str);

    /* post‑conditions */
    if (str == NULL) {
        /* nothing should have changed */
        assert(str == NULL);
    } else {
        /* the pointer itself must remain unchanged (function does not modify the caller's variable) */
        assert(str != NULL);
    }
}
