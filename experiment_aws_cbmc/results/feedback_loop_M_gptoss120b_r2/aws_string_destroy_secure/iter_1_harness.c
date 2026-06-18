#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Helper to create a zero-filled buffer of bounded size */
static void fill_zero_buffer(uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        buf[i] = 0;
    }
}

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    /* 1. nondet decide whether we have a string or NULL */
    if (nondet_bool()) {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate enough space for the header + len bytes */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondet allocator: either NULL (static string) or the default allocator */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        str->len = len;

        /* initialize the flexible array member with nondet data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    /* 2. assume the string is valid before the call */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 3. save immutable fields for later comparison */
    struct aws_allocator *old_allocator = NULL;
    size_t                old_len       = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len       = str->len;
    }

    /* 4. call the function under test */
    aws_string_destroy_secure(str);

    /* 5. post‑condition checks */
    if (str) {
        /* unchanged fields */
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);

        if (old_allocator == NULL) {
            /* when there is no allocator the memory is not freed;
               the function must have zeroed the data bytes */
            uint8_t zero_buf[MAX_BUFFER_SIZE];
            fill_zero_buffer(zero_buf, old_len);
            assert_bytes_match((const uint8_t *)str->bytes, zero_buf, old_len);

            /* the string must still be valid because it was not deallocated */
            assert(aws_string_is_valid(str));
        } else {
            /* when an allocator is present the memory may have been released.
               We cannot safely inspect the contents after a free, but the
               allocator pointer itself must remain unchanged. */
            /* No further assertions on the bytes are made in this case. */
        }
    } else {
        /* str == NULL: the function is a no‑op, nothing to assert. */
    }
}
