#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256
#define IMPLIES(a, b) (!(a) || (b))

void aws_string_eq_byte_buf_harness(void) {
    /* nondeterministic inputs */
    struct aws_string *str;
    struct aws_byte_buf buf;

    /* allocate and initialize buf */
    uint8_t buf_storage[MAX_STRING_LEN];
    buf.buffer = buf_storage;
    buf.capacity = MAX_STRING_LEN;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    for (size_t i = 0; i < buf.capacity; ++i) {
        ((uint8_t *)buf.buffer)[i] = nondet_uint8_t();
    }

    /* allocate and initialize str (may be NULL) */
    bool str_is_null = nondet_bool();
    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_STRING_LEN);
        uint8_t str_storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        str = (struct aws_string *)str_storage;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = str_len;
        for (size_t i = 0; i < str_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    /* preconditions */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_STRING_LEN));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* snapshot old state for frame condition checks */
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    uint8_t old_buf_snapshot[MAX_STRING_LEN];
    __CPROVER_assume(old_buf_capacity <= MAX_STRING_LEN);
    for (size_t i = 0; i < old_buf_capacity; ++i) {
        old_buf_snapshot[i] = ((uint8_t *)buf.buffer)[i];
    }

    size_t old_str_len = 0;
    uint8_t old_str_snapshot[MAX_STRING_LEN];
    if (str != NULL) {
        old_str_len = str->len;
        __CPROVER_assume(old_str_len <= MAX_STRING_LEN);
        for (size_t i = 0; i < old_str_len; ++i) {
            old_str_snapshot[i] = ((uint8_t *)str->bytes)[i];
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* postconditions */

    /* 1. Return value correctness */
    if (str == NULL) {
        __CPROVER_assert(result == false, "result must be false when str is NULL");
    } else {
        bool expected = (str->len == buf.len) &&
                        (memcmp(str->bytes, buf.buffer, str->len) == 0);
        __CPROVER_assert(result == expected, "result must match expected equality");
    }

    /* 2. Buffer invariants (no modification) */
    __CPROVER_assert(buf.len == old_buf_len, "buf.len unchanged");
    __CPROVER_assert(buf.capacity == old_buf_capacity, "buf.capacity unchanged");
    for (size_t i = 0; i < buf.capacity; ++i) {
        __CPROVER_assert(((uint8_t *)buf.buffer)[i] == old_buf_snapshot[i],
                         "buf.buffer unchanged");
    }

    /* 3. String invariants (no modification) */
    if (str != NULL) {
        __CPROVER_assert(str->len == old_str_len, "str->len unchanged");
        for (size_t i = 0; i < old_str_len; ++i) {
            __CPROVER_assert(((uint8_t *)str->bytes)[i] == old_str_snapshot[i],
                             "str->bytes unchanged");
        }
    }
}
