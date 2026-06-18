#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Declare src pointer, possibly NULL */
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        /* nondeterministically choose a length bounded by MAX_BUFFER_SIZE */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate enough memory for the struct plus flexible array */
        src = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(src != NULL);

        /* initialize const fields via memcpy to bypass constness */
        struct aws_string tmp;
        tmp.allocator = aws_default_allocator();
        tmp.len = len;
        memcpy(src, &tmp, sizeof(struct aws_string));

        /* bytes content is nondeterministic; no need to initialise */

        /* assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Save old state of src (if non‑NULL) for immutability checks */
    struct aws_string old;
    if (src) {
        old = *src;
    }

    /* 3. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 4. Post‑condition checks */
    if (src == NULL) {
        /* When src is NULL the cursor must be zeroed */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* When src is non‑NULL the cursor must reference the string's bytes */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* 5. Unchanged fields of src (if non‑NULL) */
    if (src) {
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        /* The byte contents are not required to be unchanged for this function */
    }

    /* 6. Validity invariants must still hold */
    if (src) {
        assert(aws_string_is_valid(src));
    }
}
