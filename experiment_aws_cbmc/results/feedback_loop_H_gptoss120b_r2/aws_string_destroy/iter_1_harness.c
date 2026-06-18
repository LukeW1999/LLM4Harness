#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;
    uint8_t *old_bytes = NULL;
    struct aws_string old = {0};

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 256);                     /* bound the length */
        /* allocate space for the struct plus flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        /* nondeterministically decide allocator (NULL for static strings) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
        str->len = len;
        /* initialize the bytes */
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        /* assume the string is initially valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* save a shallow copy of the struct (fields) */
        old = *str;
        /* save a copy of the byte contents for later comparison */
        if (len > 0) {
            old_bytes = malloc(len);
            __CPROVER_assume(old_bytes != NULL);
            for (size_t i = 0; i < len; ++i) {
                old_bytes[i] = str->bytes[i];
            }
        }
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* Post‑condition checks */
    if (str && str->allocator) {
        /* In this path the memory is released; we must not dereference `str`
         * after the call, so no further assertions about its contents are made. */
    } else {
        /* The function should not have modified the string */
        if (str) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            if (old.len > 0) {
                assert_bytes_match(str->bytes, old_bytes, old.len);
            }
        }
        /* The string (if any) must still satisfy its validity predicate */
        assert(aws_string_is_valid(str));
    }
}
