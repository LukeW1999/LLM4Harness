// === STEP 1: SUCCESS PATH ===
// When aws_string_eq returns true:
//   - param1.len: UNCHANGED
//   - param1.bytes: UNCHANGED
//   - param2.len: UNCHANGED
//   - param2.bytes: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_string_eq returns false:
//   - param1.len: UNCHANGED
//   - param1.bytes: UNCHANGED
//   - param2.len: UNCHANGED
//   - param2.bytes: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// param1 (struct aws_string):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// param2 (struct aws_string):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&param1): YES (must hold after call)
//   - aws_string_is_valid(&param2): YES (must hold after call)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_eq_harness() {
    struct aws_string *a = malloc(sizeof(struct aws_string) + 10); // Assuming a max length of 10 for simplicity
    struct aws_string *b = malloc(sizeof(struct aws_string) + 10); // Assuming a max length of 10 for simplicity

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
    }

    free(a);
    free(b);
}
