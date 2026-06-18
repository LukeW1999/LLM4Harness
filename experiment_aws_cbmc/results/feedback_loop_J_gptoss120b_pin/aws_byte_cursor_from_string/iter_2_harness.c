#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate a buffer on the stack large enough for the struct and its bytes */
        uint8_t mem[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        src = (struct aws_string *)mem;

        /* Initialize the fields of the string */
        src->allocator = aws_default_allocator();
        src->len = len;

        /* Assume the string satisfies its validity predicate */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Save old state of src (if non‑NULL) for immutability checks */
    struct aws_string old;
    if (src) {
        old = *src;
    }

    /* Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Post‑condition checks */
    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    /* Unchanged fields of src (if non‑NULL) */
    if (src) {
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert(aws_string_is_valid(src));
    }
}
