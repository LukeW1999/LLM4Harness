#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer (may be NULL) */
    struct aws_string *src;

    /* nondeterministically decide if src is NULL */
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* allocate a string with a bounded length on the stack */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        src = (struct aws_string *)storage;

        /* initialize fields */
        src->allocator = aws_default_allocator();
        src->len = len;

        /* bytes are part of the allocated block; they are left nondeterministic */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) for immutability checks */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes = {0};

    if (src != NULL) {
        old_allocator = src->allocator;
        old_len = src->len;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition assertions */
    if (src == NULL) {
        /* When src is NULL the cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor must reference the string bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 5. Unchanged fields of src (if non‑NULL) */
    if (src != NULL) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    /* 6. Validity invariants */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
