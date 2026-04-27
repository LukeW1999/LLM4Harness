// === STEP 1: SUCCESS PATH ===
// When aws_string_eq returns true:
//   - a.len: UNCHANGED
//   - a.bytes: UNCHANGED
//   - b.len: UNCHANGED
//   - b.bytes: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_string_eq returns false:
//   - a.len: UNCHANGED
//   - a.bytes: UNCHANGED
//   - b.len: UNCHANGED
//   - b.bytes: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// a (struct aws_string):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// b (struct aws_string):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&a): YES (must hold after call)
//   - aws_string_is_valid(&b): YES (must hold after call)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_harness() {
    struct aws_string *a = malloc(sizeof(struct aws_string) + 10); // Example length
    struct aws_string *b = malloc(sizeof(struct aws_string) + 10); // Example length

    if (a) {
        a->allocator = (struct aws_allocator *)nondet_ptr();
        a->len = nondet_size_t();
        a->bytes[0] = nondet_uint8_t();
    }

    if (b) {
        b->allocator = (struct aws_allocator *)nondet_ptr();
        b->len = nondet_size_t();
        b->bytes[0] = nondet_uint8_t();
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

    if (a) {
        assert(aws_string_is_valid(a));
    }

    if (b) {
        assert(aws_string_is_valid(b));
    }

    free(a);
    free(b);
}
