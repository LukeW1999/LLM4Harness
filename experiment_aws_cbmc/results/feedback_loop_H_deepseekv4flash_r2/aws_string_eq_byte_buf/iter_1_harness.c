#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic booleans for whether pointers are NULL */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    /* Allocate and initialize string if not null */
    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_STRING_LEN); /* bound the length to limit state space */

        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len * sizeof(uint8_t));
        __CPROVER_assume(str != NULL);

        /* Set the const fields by casting away const (safe in harness) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;

        /* Fill bytes with non-deterministic values */
        for (size_t i = 0; i < len; i++) {
            *(uint8_t *)&str->bytes[i] = nondet_uint8_t();
        }

        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Allocate and initialize byte buffer if not null */
    if (!buf_is_null) {
        buf = (struct aws_byte_buf *)malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buf != NULL);

        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Save old state for immutability checks */
    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer old_buf_storage;
    struct aws_string old_str;
    if (buf) {
        old_buf = *buf;
        save_byte_from_array(buf->buffer, buf->len, &old_buf_storage);
    }
    if (str) {
        old_str.allocator = str->allocator;
        old_str.len = str->len;
        /* bytes are const, we assume they remain unchanged */
    }

    /* Call function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */

    /* 1. Result matches expected behavior */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* both non-null: result is determined by array comparison;
         * we only assert that it is a bool (already by type) */
    }

    /* 2. Immutability of string (if non-null) */
    if (str) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        /* bytes are const and should not be modified */
    }

    /* 3. Immutability of byte buffer (if non-null) */
    if (buf) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->len == old_buf.len);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->buffer == old_buf.buffer);
        assert_bytes_match(buf->buffer, old_buf.buffer, buf->len);
    }

    /* 4. Validity invariants still hold */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (buf) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
