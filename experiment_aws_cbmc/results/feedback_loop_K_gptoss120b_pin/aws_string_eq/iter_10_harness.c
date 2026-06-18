#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

/* Maximum length for nondeterministic strings to keep the model tractable */
#define MAX_STRING_LEN 64

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
    for (size_t i = 0; i < a->len; ++i) {
        if (a->bytes[i] != b->bytes[i]) {
            return false;
        }
    }
    return true;
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
    __CPROVER_assert(s->len == snap->len, "length unchanged");
    for (size_t i = 0; i < s->len; ++i) {
        __CPROVER_assert(s->bytes[i] == snap->bytes[i], "bytes unchanged");
    }
}

void aws_string_eq_harness(void) {
    /* Arrange: create nondeterministic inputs for a */
    bool a_is_null = nondet_bool();
    struct aws_string *a = NULL;
    struct string_snapshot snap_a;
    if (!a_is_null) {
        uint8_t a_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
        a = (struct aws_string *)a_buf;
        size_t a_len = nondet_uint64_t();
        __CPROVER_assume(a_len <= MAX_STRING_LEN);
        a->len = a_len;
        for (size_t i = 0; i < a_len; ++i) {
            a->bytes[i] = nondet_uint8_t();
        }
    }
    take_snapshot(a, &snap_a);

    /* Arrange: create nondeterministic inputs for b */
    bool b_is_null = nondet_bool();
    struct aws_string *b = NULL;
    struct string_snapshot snap_b;
    if (!b_is_null) {
        uint8_t b_buf[sizeof(struct aws_string) + MAX_STRING_LEN];
        b = (struct aws_string *)b_buf;
        size_t b_len = nondet_uint64_t();
        __CPROVER_assume(b_len <= MAX_STRING_LEN);
        b->len = b_len;
        for (size_t i = 0; i < b_len; ++i) {
            b->bytes[i] = nondet_uint8_t();
        }
    }
    take_snapshot(b, &snap_b);

    /* Act: invoke the function under test */
    bool result = aws_string_eq(a, b);

    /* Assert: post‑condition on return value */
    bool expected = expected_eq(a, b);
    __CPROVER_assert(result == expected, "aws_string_eq returns correct result");

    /* Assert: frame property (no modification) */
    assert_unchanged(a, &snap_a);
    assert_unchanged(b, &snap_b);
}
