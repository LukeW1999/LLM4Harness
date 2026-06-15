#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Helper mutable version of aws_string to allow field initialization */
struct mutable_aws_string {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[1];
};

void aws_string_eq_byte_buf_harness(void) {
    /* -------------------------------------------------------------
     * 1. Declare and bound inputs
     * ------------------------------------------------------------- */
    const struct aws_string *str;

    /* nondet choice: str is NULL or points to a valid aws_string */
    if (nondet_bool()) {
        /* allocate a mutable string with a bounded length */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        struct mutable_aws_string *tmp =
            malloc(sizeof(struct mutable_aws_string) + (str_len == 0 ? 0 : str_len - 1));

        if (tmp) {
            tmp->allocator = aws_default_allocator();
            tmp->len = str_len;
            /* bytes are left nondet; they are readable because of malloc */
            str = (const struct aws_string *)tmp;
            __CPROVER_assume(aws_string_is_valid(str));
        } else {
            /* allocation failure – treat as NULL */
            str = NULL;
        }
    } else {
        str = NULL;
    }

    /* aws_byte_buf input */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* -------------------------------------------------------------
     * 2. Save old state
     * ------------------------------------------------------------- */
    struct aws_byte_buf old_buf = buf;
    const struct aws_string *old_str = str;

    /* -------------------------------------------------------------
     * 3. Call function under test
     * ------------------------------------------------------------- */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* -------------------------------------------------------------
     * 4. Post‑condition checks
     * ------------------------------------------------------------- */
    if (result) {
        /* result true only when both arguments are non‑NULL and the
         * underlying byte sequences are equal */
        assert(str != NULL);
        assert(aws_array_eq(str->bytes, str->len, buf.buffer, buf.len));
    } else {
        /* result false when either argument is NULL or the byte
         * sequences differ */
        if (str == NULL) {
            assert(str == NULL);
        } else {
            assert(!aws_array_eq(str->bytes, str->len, buf.buffer, buf.len));
        }
    }

    /* -------------------------------------------------------------
     * 5. Unchanged fields (function is read‑only)
     * ------------------------------------------------------------- */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.owns_buffer == old_buf.owns_buffer);
    assert(str == old_str);

    /* -------------------------------------------------------------
     * 6. Validity invariants
     * ------------------------------------------------------------- */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    assert(aws_byte_buf_is_valid(&buf));
}
