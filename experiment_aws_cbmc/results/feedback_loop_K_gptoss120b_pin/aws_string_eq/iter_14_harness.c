#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

#define MAX_STRING_LEN 64

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
            uint8_t byte = nondet_uint8_t();
            a->bytes[i] = byte;
        }
    }
    take_snapshot(a, &snap_a);

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
            uint8_t byte = nondet_uint8_t();
            b->bytes[i] = byte;
        }
    }
    take_snapshot(b, &snap_b);

    bool result = aws_string_eq(a, b);
    bool expected = expected_eq(a, b);
    __CPROVER_assert(result == expected, "aws_string_eq returns correct result");

    assert_unchanged(a, &snap_a);
    assert_unchanged(b, &snap_b);
}
