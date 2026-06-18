#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool nondet_bool(void);
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

#define MAX_STRING_LEN 256U

void aws_byte_cursor_from_string_harness(void) {
    /* Decide whether src is NULL */
    bool src_is_null = nondet_bool();

    /* Allocate storage for src when non‑NULL */
    uint8_t src_mem[sizeof(struct aws_string) + MAX_STRING_LEN];
    struct aws_string *src = src_is_null ? NULL : (struct aws_string *)src_mem;

    if (src) {
        /* Non‑deterministic allocator (NULL for static strings or default) */
        src->allocator = nondet_bool() ? NULL : aws_default_allocator();

        /* Non‑deterministic length bounded by MAX_STRING_LEN */
        src->len = nondet_size_t();
        __CPROVER_assume(src->len <= MAX_STRING_LEN);

        /* Fill the bytes with non‑deterministic values */
        for (size_t i = 0; i < src->len; ++i) {
            src->bytes[i] = nondet_uint8_t();
        }

        /* Assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Save old state of src (if non‑NULL) */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes;
    if (src) {
        old_allocator = src->allocator;
        old_len = src->len;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Post‑condition assertions */
    if (src == NULL) {
        /* When src is NULL the cursor is zero‑initialized */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor points to the string bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* Unchanged fields of src (if non‑NULL) */
    if (src) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* Validity invariants */
    if (src) {
        assert(aws_string_is_valid(src));
    }

    /* The cursor should be bounded (no more than MAX_STRING_LEN bytes) */
    assert(cursor.len <= MAX_STRING_LEN);
}
