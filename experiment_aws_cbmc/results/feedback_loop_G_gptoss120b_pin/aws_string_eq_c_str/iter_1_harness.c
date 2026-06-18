#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Bounding constant for string lengths – reuse the generic buffer bound */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN MAX_BUFFER_SIZE
#endif

void aws_string_eq_c_str_harness(void) {
    /* 1. Non‑deterministic inputs */
    struct aws_string *str;
    const char *c_str;

    /* Decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate space for the struct plus flexible array */
        str = malloc(sizeof(struct aws_string) + (len ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* Initialise fields */
        str->allocator = aws_default_allocator();
        str->len = len;

        /* The bytes are nondeterministic – no need to initialise them */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Decide whether c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        char *buf = malloc(c_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Nondeterministic contents, ensure null‑termination */
        buf[c_len] = '\0';
        c_str = buf;
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str_snapshot;
    struct store_byte_from_buffer old_bytes_storage;
    const char *old_c_str = c_str;

    if (str != NULL) {
        old_str_snapshot = *str;                     /* copy allocator and len */
        save_byte_from_array(str->bytes, str->len, &old_bytes_storage);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. Post‑condition assertions */

    /* Behaviour when either argument is NULL */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        /* Both non‑NULL – result must equal the array‑comparison helper */
        assert(result == aws_array_eq_c_str(str->bytes, str->len, c_str));
    }

    /* 5. Unchanged fields (function is pure) */
    if (str != NULL) {
        assert(str->allocator == old_str_snapshot.allocator);
        assert(str->len == old_str_snapshot.len);
        /* Bytes must be unchanged */
        assert_bytes_match(str->bytes, old_str_snapshot.bytes, str->len);
        assert_byte_from_buffer_matches(str->bytes, &old_bytes_storage);
    }
    assert(c_str == old_c_str);

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    return 0;
}
