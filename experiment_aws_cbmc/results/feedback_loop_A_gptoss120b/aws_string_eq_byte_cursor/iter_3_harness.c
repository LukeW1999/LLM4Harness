#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include "proof_helpers/make_common_data_structures.h"

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

/* Allocate a valid aws_string with nondeterministic length */
static struct aws_string *allocate_aws_string(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate space for struct + bytes + terminating NUL */
    struct aws_string *s = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(s != NULL);

    /* Dummy allocator (may be NULL) */
    s->allocator = (struct aws_allocator *)malloc(1);
    __CPROVER_assume(s->allocator != NULL);

    s->len = len;
    /* bytes are nondet; they are already part of the malloced block */

    __CPROVER_assume(aws_string_is_valid(s));
    return s;
}

/* Allocate a valid aws_byte_cursor with a bounded buffer */
static struct aws_byte_cursor *allocate_aws_byte_cursor(void) {
    struct aws_byte_cursor *c = malloc(sizeof(struct aws_byte_cursor));
    __CPROVER_assume(c != NULL);
    ensure_byte_cursor_has_allocated_buffer_member(c);
    __CPROVER_assume(aws_byte_cursor_is_bounded(c, MAX_BUFFER_SIZE));
    return c;
}

void aws_string_eq_byte_cursor_harness(void) {
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* Make both allocation choices nondeterministic */
    if (nondet_bool()) {
        str = allocate_aws_string();
    }
    if (nondet_bool()) {
        cur = allocate_aws_byte_cursor();
    }

    /* Preserve original observable state */
    struct store_byte_from_buffer str_bytes_storage;
    if (str) {
        save_byte_from_array(str->bytes, str->len, &str_bytes_storage);
    }

    struct aws_byte_cursor old_cur = {0};
    if (cur) {
        old_cur = *cur;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Post‑condition: result is true iff the contents are equal */
    if (result) {
        assert((str == NULL && cur == NULL) ||
               (str != NULL && cur != NULL &&
                aws_array_eq(str->bytes, str->len, cur->ptr, cur->len)));
    } else {
        assert(!((str == NULL && cur == NULL) ||
                 (str != NULL && cur != NULL &&
                  aws_array_eq(str->bytes, str->len, cur->ptr, cur->len))));
    }

    /* Verify inputs are unchanged */
    if (str) {
        assert(str->len == ((struct aws_string *)str)->len);
        assert(str->allocator == ((struct aws_string *)str)->allocator);
        assert_byte_from_buffer_matches(str->bytes, &str_bytes_storage);
    }

    if (cur) {
        assert(cur->len == old_cur.len);
        assert(cur->ptr == old_cur.ptr);
    }

    /* Re‑establish invariants */
    if (str) {
        assert(aws_string_is_valid(str));
    }
    if (cur) {
        assert(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
    }
}

int main(void) {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
