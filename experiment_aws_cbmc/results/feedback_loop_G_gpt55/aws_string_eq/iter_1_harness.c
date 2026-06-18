#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct bounded_aws_string_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

static void init_bounded_aws_string_storage(struct bounded_aws_string_storage *str) {
    str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);

    for (size_t i = 0; i <= MAX_BUFFER_SIZE; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    str->bytes[str->len] = 0;
}

static bool bounded_byte_arrays_equal(const uint8_t *a, const uint8_t *b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void aws_string_eq_harness() {
    struct bounded_aws_string_storage storage_a;
    struct bounded_aws_string_storage storage_b;

    init_bounded_aws_string_storage(&storage_a);
    init_bounded_aws_string_storage(&storage_b);

    const struct aws_string *valid_a = (const struct aws_string *)&storage_a;
    const struct aws_string *valid_b = (const struct aws_string *)&storage_b;

    __CPROVER_assume(aws_string_is_valid(valid_a));
    __CPROVER_assume(aws_string_is_valid(valid_b));

    const struct aws_string *a;
    const struct aws_string *b;

    uint8_t choice = nondet_uint8_t();
    if (choice == 0) {
        a = NULL;
        b = NULL;
    } else if (choice == 1) {
        a = valid_a;
        b = NULL;
    } else if (choice == 2) {
        a = NULL;
        b = valid_b;
    } else if (choice == 3) {
        a = valid_a;
        b = valid_a;
    } else {
        a = valid_a;
        b = valid_b;
    }

    struct bounded_aws_string_storage old_storage_a = storage_a;
    struct bounded_aws_string_storage old_storage_b = storage_b;

    const struct aws_string *old_a = a;
    const struct aws_string *old_b = b;

    size_t old_a_len = old_a != NULL ? old_a->len : 0;
    size_t old_b_len = old_b != NULL ? old_b->len : 0;

    bool expected_result;
    if (old_a == old_b) {
        expected_result = true;
    } else if (old_a == NULL || old_b == NULL) {
        expected_result = false;
    } else if (old_a_len != old_b_len) {
        expected_result = false;
    } else {
        expected_result = bounded_byte_arrays_equal(old_a->bytes, old_b->bytes, old_a_len);
    }

    bool result = aws_string_eq(a, b);

    assert(result == expected_result);

    if (result) {
        assert(old_a == old_b || (old_a != NULL && old_b != NULL));
        if (old_a != NULL && old_b != NULL && old_a != old_b) {
            assert(old_a_len == old_b_len);
            assert_bytes_match(old_a->bytes, old_b->bytes, old_a_len);
        }
    } else {
        assert(old_a != old_b);
        assert(old_a == NULL || old_b == NULL || old_a_len != old_b_len ||
               !bounded_byte_arrays_equal(old_a->bytes, old_b->bytes, old_a_len));
    }

    assert(a == old_a);
    assert(b == old_b);

    assert(storage_a.allocator == old_storage_a.allocator);
    assert(storage_a.len == old_storage_a.len);
    assert_bytes_match(storage_a.bytes, old_storage_a.bytes, MAX_BUFFER_SIZE + 1);

    assert(storage_b.allocator == old_storage_b.allocator);
    assert(storage_b.len == old_storage_b.len);
    assert_bytes_match(storage_b.bytes, old_storage_b.bytes, MAX_BUFFER_SIZE + 1);

    if (a != NULL) {
        assert(aws_string_is_valid(a));
    }
    if (b != NULL) {
        assert(aws_string_is_valid(b));
    }

    assert(aws_string_is_valid(valid_a));
    assert(aws_string_is_valid(valid_b));
}
