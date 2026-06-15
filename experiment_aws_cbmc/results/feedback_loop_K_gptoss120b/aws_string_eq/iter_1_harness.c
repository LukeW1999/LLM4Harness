#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Helper to create a nondet aws_string with static allocator (allocator == NULL) */
static struct aws_string *make_nondet_string(void) {
    size_t len;
    __CPROVER_assume(len <= 256);               /* bound the length for tractability */

    /* allocate space for struct + (len-1) extra bytes (bytes[1] already accounts for 1) */
    struct aws_string *s = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
    __CPROVER_assume(s != NULL);

    s->allocator = NULL;                        /* static string */
    s->len = len;

    /* make the payload bytes nondet */
    for (size_t i = 0; i < len; ++i) {
        s->bytes[i] = (uint8_t) __CPROVER_nondet_uint();
    }

    /* ensure the string satisfies the library’s validity predicate */
    __CPROVER_assume(aws_string_is_valid(s));

    return s;
}

/* Make a copy of the string’s observable state for later comparison */
static void snapshot_string(const struct aws_string *src,
                            size_t *len_out,
                            uint8_t **bytes_out) {
    if (src == NULL) {
        *len_out = 0;
        *bytes_out = NULL;
        return;
    }
    *len_out = src->len;
    if (src->len == 0) {
        *bytes_out = NULL;
        return;
    }
    *bytes_out = malloc(src->len);
    __CPROVER_assume(*bytes_out != NULL);
    memcpy(*bytes_out, src->bytes, src->len);
}

/* Verify that the snapshot matches the current state */
static void check_unchanged(const struct aws_string *src,
                             size_t len_snapshot,
                             const uint8_t *bytes_snapshot) {
    if (src == NULL) {
        return;
    }
    assert(src->len == len_snapshot);
    if (len_snapshot > 0) {
        assert(memcmp(src->bytes, bytes_snapshot, len_snapshot) == 0);
    }
}

void aws_string_eq_harness(void) {
    struct aws_string *a = NULL;
    struct aws_string *b = NULL;

    /* nondet choice: either keep NULL or allocate a valid string */
    if (__CPROVER_nondet_bool()) {
        a = make_nondet_string();
    }
    if (__CPROVER_nondet_bool()) {
        b = make_nondet_string();
    }

    /* Snapshot pre‑state */
    size_t a_len_snapshot = 0, b_len_snapshot = 0;
    uint8_t *a_bytes_snapshot = NULL, *b_bytes_snapshot = NULL;
    snapshot_string(a, &a_len_snapshot, &a_bytes_snapshot);
    snapshot_string(b, &b_len_snapshot, &b_bytes_snapshot);

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Post‑condition: result matches the specification */
    bool expected = false;
    if (a == b) {
        expected = true;
    } else if (a != NULL && b != NULL && a->len == b->len) {
        expected = (memcmp(a->bytes, b->bytes, a->len) == 0);
    }
    assert(result == expected);

    /* Frame condition: a and b are unchanged */
    check_unchanged(a, a_len_snapshot, a_bytes_snapshot);
    check_unchanged(b, b_len_snapshot, b_bytes_snapshot);

    /* Clean up */
    free(a_bytes_snapshot);
    free(b_bytes_snapshot);
    free(a);
    free(b);
    return 0;
}
