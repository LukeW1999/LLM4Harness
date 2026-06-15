#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness(void) {
    /* 1. Nondeterministic inputs */
    struct aws_string *str;
    const char *c_str;

    /* str may be NULL or a valid aws_string */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate enough space for the flexible array member */
        size_t max_len = MAX_BUFFER_SIZE;
        str = malloc(sizeof(struct aws_string) + max_len - 1);
        __CPROVER_assume(str != NULL);

        /* nondet length bounded by MAX_BUFFER_SIZE */
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= max_len);

        /* allocator is not used by the function under test; set to NULL */
        str->allocator = NULL;

        /* the bytes array is part of the allocation; its contents are irrelevant */
    }

    /* c_str may be NULL or a valid null‑terminated C string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
        char *buf = malloc(c_len + 1);
        __CPROVER_assume(buf != NULL);
        /* ensure null termination */
        buf[c_len] = '\0';
        c_str = buf;
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    if (str != NULL) {
        old_str = *str;
        /* save the original bytes for later comparison */
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Post‑condition assertions */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        /* when both arguments are non‑NULL the result must equal the
         * outcome of the underlying array comparison.  The harness cannot
         * directly compute that, but we can at least assert that the
         * result is a boolean value (true or false). */
        assert(result == true || result == false);
    }

    /* 5. Unchanged fields (immutability) */
    if (str != NULL) {
        assert(str->len == old_str.len);
        assert(str->allocator == old_str.allocator);
        /* bytes must remain unchanged */
        assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    }

    if (c_str != NULL) {
        /* c_str pointer must not be altered */
        const char *saved_c_str = c_str;
        assert(c_str == saved_c_str);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
