#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_harness() {
    struct aws_string *a = bounded_malloc(sizeof(struct aws_string) + 10); // Assuming a max length of 10 for simplicity
    struct aws_string *b = bounded_malloc(sizeof(struct aws_string) + 10); // Assuming a max length of 10 for simplicity

    if (a && b) {
        a->allocator = NULL;
        b->allocator = NULL;
        a->len = nondet_size_t();
        b->len = nondet_size_t();
        __CPROVER_assume(a->len <= 10);
        __CPROVER_assume(b->len <= 10);
        for (size_t i = 0; i < a->len; i++) {
            a->bytes[i] = nondet_uint8_t();
        }
        for (size_t i = 0; i < b->len; i++) {
            b->bytes[i] = nondet_uint8_t();
        }

        struct aws_string old_a = *a;
        struct aws_string old_b = *b;

        bool result = aws_string_eq(a, b);

        assert(a->allocator == old_a.allocator);
        assert(a->len == old_a.len);
        assert(memcmp(a->bytes, old_a.bytes, a->len) == 0);
        assert(b->allocator == old_b.allocator);
        assert(b->len == old_b.len);
        assert(memcmp(b->bytes, old_b.bytes, b->len) == 0);

        assert(aws_string_is_valid(a));
        assert(aws_string_is_valid(b));

        if (result) {
            assert(a->len == b->len);
            for (size_t i = 0; i < a->len; i++) {
                assert(a->bytes[i] == b->bytes[i]);
            }
        } else {
            assert(a->len != b->len || memcmp(a->bytes, b->bytes, a->len) != 0);
        }
    } else {
        assert(a == NULL || b == NULL);
    }

    free(a);
    free(b);
}
