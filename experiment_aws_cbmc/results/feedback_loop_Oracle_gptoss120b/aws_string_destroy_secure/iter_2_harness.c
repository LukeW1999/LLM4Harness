#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    /* nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);

    /* stack‑allocated buffer for aws_string with flexible array */
    uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_LEN];
    struct aws_string *str = nondet_bool() ? NULL : (struct aws_string *)buffer;

    if (str) {
        /* allocator is NULL to avoid freeing stack memory */
        str->allocator = NULL;
        str->len = len;

        /* nondeterministic bytes */
        uint8_t *bytes = (uint8_t *)str->bytes;
        for (size_t j = 0; j < len; ++j) {
            bytes[j] = nondet_uint8_t();
        }
    }

    /* keep a copy of the original length and a snapshot of one byte */
    size_t old_len = 0;
    uint8_t old_byte = 0;
    if (str) {
        old_len = str->len;
        if (old_len > 0) {
            size_t i = nondet_size_t();
            __CPROVER_assume(i < old_len);
            old_byte = ((uint8_t *)str->bytes)[i];
            (void)i; /* expose i to the post‑condition */
        }
    }

    /* structural validity assumption */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    /* call the function under verification */
    aws_string_destroy_secure(str);

    /* post‑conditions */
    if (str) {
        if (str->allocator == NULL) {
            /* when no allocator, the memory is not freed and must be zeroed */
            if (old_len > 0) {
                size_t i = nondet_size_t();
                __CPROVER_assume(i < old_len);
                assert(((uint8_t *)str->bytes)[i] == 0);
            }
            /* length must remain unchanged */
            assert(str->len == old_len);
        } else {
            /* allocator present case (not exercised in this harness) */
            assert(str->len == old_len);
        }
    } else {
        /* str == NULL: nothing to check */
        assert(str == NULL);
    }
}
