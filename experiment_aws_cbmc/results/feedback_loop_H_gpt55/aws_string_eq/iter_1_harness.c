#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_bounded {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

static void initialize_bounded_aws_string(struct aws_string_bounded *str) {
    str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    str->bytes[str->len] = 0;

    __CPROVER_assume(aws_string_is_valid((const struct aws_string *)str));
}

static bool bytes_are_equal(const uint8_t *a, size_t a_len, const uint8_t *b, size_t b_len) {
    if (a_len != b_len) {
        return false;
    }

    for (size_t i = 0; i < a_len; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

static bool strings_are_equal(const struct aws_string *a, const struct aws_string *b) {
    if (a == b) {
        return true;
    }

    if (a == NULL || b == NULL) {
        return false;
    }

    return bytes_are_equal(a->bytes, a->len, b->bytes, b->len);
}

static void assert_bounded_aws_string_unchanged(
    const struct aws_string_bounded *current,
    const struct aws_string_bounded *old) {

    assert(current->allocator == old->allocator);
    assert(current->len == old->len);

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        assert(current->bytes[i] == old->bytes[i]);
    }
}

void aws_string_eq_harness() {
    struct aws_string_bounded a_storage;
    struct aws_string_bounded b_storage;

    initialize_bounded_aws_string(&a_storage);
    initialize_bounded_aws_string(&b_storage);

    struct aws_string_bounded old_a_storage = a_storage;
    struct aws_string_bounded old_b_storage = b_storage;

    const struct aws_string *a = NULL;
    const struct aws_string *b = NULL;

    uint8_t choice = nondet_uint8_t();
    __CPROVER_assume(choice < 5);

    if (choice == 0) {
        a = (const struct aws_string *)&a_storage;
        b = (const struct aws_string *)&b_storage;
    } else if (choice == 1) {
        a = (const struct aws_string *)&a_storage;
        b = (const struct aws_string *)&a_storage;
    } else if (choice == 2) {
        a = NULL;
        b = (const struct aws_string *)&b_storage;
    } else if (choice == 3) {
        a = (const struct aws_string *)&a_storage;
        b = NULL;
    } else {
        a = NULL;
        b = NULL;
    }

    __CPROVER_assume(a == NULL || aws_string_is_valid(a));
    __CPROVER_assume(b == NULL || aws_string_is_valid(b));

    bool expected = strings_are_equal(a, b);

    bool result = aws_string_eq(a, b);

    assert(result == expected);

    if (result) {
        assert(expected);
        assert(a == b || (a != NULL && b != NULL));
        if (a != b) {
            assert(a->len == b->len);
            assert(bytes_are_equal(a->bytes, a->len, b->bytes, b->len));
        }

        assert_bounded_aws_string_unchanged(&a_storage, &old_a_storage);
        assert_bounded_aws_string_unchanged(&b_storage, &old_b_storage);
    } else {
        assert(!expected);
        assert(a != b);
        if (a != NULL && b != NULL) {
            assert(!bytes_are_equal(a->bytes, a->len, b->bytes, b->len));
        } else {
            assert(a == NULL || b == NULL);
        }

        assert_bounded_aws_string_unchanged(&a_storage, &old_a_storage);
        assert_bounded_aws_string_unchanged(&b_storage, &old_b_storage);
    }

    assert_bounded_aws_string_unchanged(&a_storage, &old_a_storage);
    assert_bounded_aws_string_unchanged(&b_storage, &old_b_storage);

    assert(aws_string_is_valid((const struct aws_string *)&a_storage));
    assert(aws_string_is_valid((const struct aws_string *)&b_storage));

    if (a != NULL) {
        assert(aws_string_is_valid(a));
    } else {
        assert(a == NULL);
    }

    if (b != NULL) {
        assert(aws_string_is_valid(b));
    } else {
        assert(b == NULL);
    }
}
