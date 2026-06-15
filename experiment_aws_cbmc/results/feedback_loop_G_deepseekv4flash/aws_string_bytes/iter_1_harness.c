#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

/* Bounds for string length */
#define MAX_STRING_SIZE 10

void aws_string_bytes_harness() {
    /* Nondeterministic length */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_SIZE);

    /* Allocate memory for aws_string and its bytes together */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Initialize const fields (via cast to bypass const for initialization) */
    struct aws_allocator *allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    *((struct aws_allocator **)&str->allocator) = allocator;
    *((size_t *)&str->len) = len;

    /* Fill bytes with nondeterministic values */
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = nondet_uint8_t();
    }

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save the original byte content for immutability check */
    struct store_byte_from_buffer storage;
    if (len > 0) {
        save_byte_from_array(str->bytes, len, &storage);
    }

    /* Call the function */
    const uint8_t *result = aws_string_bytes(str);

    /* Assert the result points to the bytes array */
    assert(result == str->bytes);

    /* Assert that the string remains valid */
    assert(aws_string_is_valid(str));

    /* Assert that the bytes content has not changed (the function should not modify them) */
    if (len > 0) {
        assert_byte_from_buffer_matches(str->bytes, &storage);
    }

    /* Assert that the const fields are unchanged */
    assert(str->allocator == allocator);
    assert(str->len == len);
}
