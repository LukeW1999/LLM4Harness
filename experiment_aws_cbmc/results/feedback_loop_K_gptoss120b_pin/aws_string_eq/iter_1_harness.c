#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Maximum length for nondeterministic strings to keep the model tractable */
#define MAX_STRING_LEN 64

/* Helper to create a nondeterministic aws_string (or NULL) */
static struct aws_string *make_nondet_string(void) {
    bool is_null = nondet_bool();
    if (is_null) {
        return NULL;
    }

    size_t len = nondet_uint64();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate space for struct + flexible array */
    struct aws_string *s = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(s != NULL);

    /* Initialise fields */
    s->allocator = aws_default_allocator();
    s->len = len;

    /* Fill bytes with nondeterministic data */
    for (size_t i = 0; i < len; ++i) {
        s->bytes[i] = nondet_uint8();
    }

    /* Ensure the constructed string satisfies the library invariant */
    __CPROVER_assume(aws_string_is_valid(s));

    return s;
}

/* Compute the expected result of aws_string_eq using a pure specification */
static bool expected_eq(const struct aws_string *a, const struct aws_string *b) {
    if (a == b) {
        return true;
    }
    if (a == NULL || b == NULL) {
        return false;
    }
    if (a->len != b->len) {
        return false;
    }
    return (memcmp(a->bytes, b->bytes, a->len) == 0);
}

/* Preserve a snapshot of a string's observable state for frame checking */
struct string_snapshot {
    size_t len;
    uint8_t bytes[MAX_STRING_LEN];
};

static void take_snapshot(const struct aws_string *s, struct string_snapshot *snap) {
    if (s == NULL) {
        snap->len = 0;
        return;
    }
    snap->len = s->len;
    for (size_t i = 0; i < s->len; ++i) {
        snap->bytes[i] = s->bytes[i];
    }
}

static void assert_unchanged(const struct aws_string *s, const struct string_snapshot *snap) {
    if (s == NULL) {
        return;
    }
    assert(s->len == snap->len);
    for (size_t i = 0; i < s->len; ++i) {
        assert(s->bytes[i] == snap->bytes[i]);
    }
}

void aws_string_eq_harness(void) {
    /* Arrange: create nondeterministic inputs */
    struct aws_string *a = make_nondet_string();
    struct aws_string *b = make_nondet_string();

    /* Record pre‑state for frame property */
    struct string_snapshot snap_a, snap_b;
    take_snapshot(a, &snap_a);
    take_snapshot(b, &snap_b);

    /* Act: invoke the function under test */
    bool result = aws_string_eq(a, b);

    /* Assert: post‑condition on return value */
    bool expected = expected_eq(a, b);
    assert(result == expected);

    /* Assert: frame property (no modification) */
    assert_unchanged(a, &snap_a);
    assert_unchanged(b, &snap_b);

    /* Clean up */
    free((void *)a);
    free((void *)b);
    return 0;
}
