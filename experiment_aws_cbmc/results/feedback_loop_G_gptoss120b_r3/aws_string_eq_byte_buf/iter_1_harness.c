#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Non‑deterministic inputs (may be NULL) */
    struct aws_string *str = NULL;
    struct aws_byte_buf buf;
    bool str_present = nondet_bool();
    bool buf_present = nondet_bool();

    /* -------------------------------------------------
     * Allocate and initialise a non‑null aws_string
     * ------------------------------------------------- */
    if (str_present) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        /* allocate space for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + str_len);
        __CPROVER_assume(str != NULL);

        /* initialise fields */
        str->allocator = aws_default_allocator();
        str->len = str_len;

        /* nondet initialise the bytes */
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* -------------------------------------------------
     * Allocate and initialise a non‑null aws_byte_buf
     * ------------------------------------------------- */
    if (buf_present) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(buf.allocator != NULL);
        __CPROVER_assume(buf.capacity >= buf.len);
        __CPROVER_assume(AWS_MEM_IS_READABLE(buf.buffer, buf.capacity));
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    } else {
        /* when not present, make the pointer we will pass NULL */
        /* (the variable `buf` is unused in that case) */
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str_present) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf_present) {
        old_buf = buf;
        save_byte_from_array(buf.buffer, buf.len, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf_present ? &buf : NULL);

    /* 4. Post‑condition: return value matches specification */
    if (!str_present && !buf_present) {
        assert(result == true);
    } else if (!str_present || !buf_present) {
        assert(result == false);
    } else {
        /* both non‑NULL – result must equal array comparison */
        assert(result == aws_array_eq(str->bytes, str->len, buf.buffer, buf.len));
    }

    /* 5. Unchanged fields (immutability) */
    if (str_present) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (buf_present) {
        assert(buf.allocator == old_buf.allocator);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.len == old_buf.len);
        assert(buf.buffer == old_buf.buffer);
        assert_byte_from_buffer_matches(buf.buffer, &old_buf_bytes);
    }

    /* 6. Validity invariants must still hold */
    if (str_present) {
        assert(aws_string_is_valid(str));
    }
    if (buf_present) {
        assert(aws_byte_buf_is_valid(&buf));
    }
}
