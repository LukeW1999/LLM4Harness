#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include "proof_helpers/make_common_data_structures.h"

/* Helper to allocate a flexible‑array aws_string with a given length */
static struct aws_string *allocate_aws_string(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate space for the struct plus the flexible array (len bytes + 1 for the terminator) */
    struct aws_string *s = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(s != NULL);

    /* nondet allocator (may be NULL) */
    s->allocator = (struct aws_allocator *)malloc(1);
    /* len field */
    *((size_t *)&s->len) = len;               /* const cast for initialization */
    /* bytes are nondet; they are already allocated as part of the malloc above */

    /* assume the string satisfies the library’s validity predicate */
    __CPROVER_assume(aws_string_is_valid(s));
    return s;
}

/* Helper to allocate a aws_byte_cursor with a bounded buffer */
static struct aws_byte_cursor *allocate_aws_byte_cursor(void) {
    struct aws_byte_cursor *c = malloc(sizeof(struct aws_byte_cursor));
    __CPROVER_assume(c != NULL);
    ensure_byte_cursor_has_allocated_buffer_member(c);
    __CPROVER_assume(aws_byte_cursor_is_bounded(c, MAX_BUFFER_SIZE));
    return c;
}

void aws_string_eq_byte_cursor_harness(void) {
    /* 1. nondeterministically decide whether the inputs are NULL or point to valid objects */
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    if (nondet_bool()) {
        str = allocate_aws_string();
    }

    if (nondet_bool()) {
        cur = allocate_aws_byte_cursor();
    }

    /* 2. Save immutable state for later comparison */
    struct store_byte_from_buffer str_bytes_storage;
    if (str) {
        save_byte_from_array(str->bytes, str->len, &str_bytes_storage);
    }

    struct aws_byte_cursor old_cur = {0};
    if (cur) {
        old_cur = *cur;
    }

    /* 3. Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* 4. Post‑condition on the return value */
    if (result) {
        /* true is returned only when both arguments are NULL or when both are non‑NULL
         * and their byte sequences compare equal. */
        assert((str == NULL && cur == NULL) ||
               (str != NULL && cur != NULL &&
                aws_array_eq(str->bytes, str->len, cur->ptr, cur->len)));
    } else {
        /* false is returned in all other cases */
        assert(!((str == NULL && cur == NULL) ||
                 (str != NULL && cur != NULL &&
                  aws_array_eq(str->bytes, str->len, cur->ptr, cur->len))));
    }

    /* 5. Unchanged fields – the function must not modify any input data */
    if (str) {
        /* the string structure is const, but we still assert the observable fields */
        assert(str->len == ((struct aws_string *)str)->len);
        assert(str->allocator == ((struct aws_string *)str)->allocator);
        assert_byte_from_buffer_matches(str->bytes, &str_bytes_storage);
    }

    if (cur) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
    }

    /* 6. Validity invariants must still hold after the call */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (cur) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}
