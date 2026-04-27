#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void assert_bytes_match(const uint8_t *a, const uint8_t *b, size_t len) {
    for (size_t i = 0; i < len; i++) {
        assert(a[i] == b[i]);
    }
}

void aws_string_eq_harness() {
    struct aws_string *a = bounded_malloc(sizeof(struct aws_string) + 10); // Example length
    struct aws_string *b = bounded_malloc(sizeof(struct aws_string) + 10); // Example length

    if (a) {
        a->allocator = (struct aws_allocator *)nondet_ptr();
        a->len = nondet_size_t();
        __CPROVER_assume(a->len <= 10); // Ensure len is within allocated bounds
        for (size_t i = 0; i < a->len; i++) {
            a->bytes[i] = nondet_uint8_t();
        }
    }

    if (b) {
        b->allocator = (struct aws_allocator *)nondet_ptr();
        b->len = nondet_size_t();
        __CPROVER_assume(b->len <= 10); // Ensure len is within allocated bounds
        for (size_t i = 0; i < b->len; i++) {
            b->bytes[i] = nondet_uint8_t();
        }
    }

    struct aws_string old_a;
    struct aws_string old_b;

    if (a) {
        old_a = *a;
    }
    if (b) {
        old_b = *b;
    }

    bool result = aws_string_eq(a, b);

    if (a) {
        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert_bytes_match(a->bytes, old_a.bytes, a->len);
    }

    if (b) {
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert_bytes_match(b->bytes, old_b.bytes, b->len);
    }

    if (a && b) {
        if (result) {
            assert(a->len == b->len);
            assert_bytes_match(a->bytes, b->bytes, a->len);
        } else {
            bool any_diff = false;
            if (a->len != b->len) {
                any_diff = true;
            } else {
                for (size_t i = 0; i < a->len; i++) {
                    if (a->bytes[i] != b->bytes[i]) {
                        any_diff = true;
                        break;
                    }
                }
            }
            assert(any_diff);
        }
    }

    if (a) {
        assert(aws_string_is_valid(a));
    }

    if (b) {
        assert(aws_string_is_valid(b));
    }

    free(a);
    free(b);
}
