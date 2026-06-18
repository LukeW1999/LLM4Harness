#include <aws/common/string.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
_Bool nondet_bool(void);

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    /* allocate a candidate aws_string with flexible array */
    size_t alloc_len = nondet_size_t();
    __CPROVER_assume(alloc_len < 1024);               /* keep allocation bounded */
    str = malloc(sizeof(struct aws_string) + alloc_len);
    if (str) {
        /* nondet allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* nondet length that fits into the allocated buffer */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= alloc_len);

        /* initialise the bytes with nondet values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t j = 0; j < alloc_len; ++j) {
            bytes[j] = nondet_uint8_t();
        }
    }

    /* keep a copy of the original pointer for post‑condition checks */
    struct aws_string *old_str = str;
    size_t i = nondet_size_t();

    /* ground‑truth preconditions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(i < old_str->len);

    /* function under test */
    aws_string_destroy_secure(str);

    /* post‑conditions */
    if (old_str != NULL) {
        /* the i‑th byte must have been zeroed before the memory was released */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(old_str);
        assert(bytes[i] == 0);
    }

    return 0;
}
