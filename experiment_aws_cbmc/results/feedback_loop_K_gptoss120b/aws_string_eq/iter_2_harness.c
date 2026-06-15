#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

/* Helper to create a nondet aws_string */
static struct aws_string *make_nondet_string(void) {
    struct aws_string *s = make_aws_string_nondet();
    __CPROVER_assume(s != NULL);
    return s;
}

/* Snapshot the observable state of a string */
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

/* Verify that a string has not changed */
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

    if (__CPROVER_nondet_bool()) {
        a = make_nondet_string();
    }
    if (__CPROVER_nondet_bool()) {
        b = make_nondet_string();
    }

    size_t a_len_snapshot = 0, b_len_snapshot = 0;
    uint8_t *a_bytes_snapshot = NULL, *b_bytes_snapshot = NULL;
    snapshot_string(a, &a_len_snapshot, &a_bytes_snapshot);
    snapshot_string(b, &b_len_snapshot, &b_bytes_snapshot);

    bool result = aws_string_eq(a, b);

    bool expected = false;
    if (a == b) {
        expected = true;
    } else if (a != NULL && b != NULL && a->len == b->len) {
        expected = (memcmp(a->bytes, b->bytes, a->len) == 0);
    }
    assert(result == expected);

    check_unchanged(a, a_len_snapshot, a_bytes_snapshot);
    check_unchanged(b, b_len_snapshot, b_bytes_snapshot);

    free(a_bytes_snapshot);
    free(b_bytes_snapshot);
    aws_string_destroy(a);
    aws_string_destroy(b);
}
