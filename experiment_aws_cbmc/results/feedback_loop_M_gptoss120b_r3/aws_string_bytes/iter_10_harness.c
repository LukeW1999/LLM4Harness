#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_bytes_harness(void) {
    /* 1. Declare and bound the aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    size_t total_size = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
    __CPROVER_assume(total_size <= MAX_BUFFER_SIZE);

    union {
        struct aws_string s;
        uint8_t raw[MAX_BUFFER_SIZE];
    } u;
    struct aws_string *str = &u.s;
    __CPROVER_assume(str != NULL);

    /* allocator may be NULL (static string) or a real allocator */
    if (nondet_bool()) {
        str->allocator = aws_default_allocator();
    } else {
        str->allocator = NULL;
    }

    str->len = len;

    /* nondet fill the bytes */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* assume the string is valid before the call */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    struct store_byte_from_buffer old_byte;
    if (len > 0) {
        save_byte_from_array(str->bytes, len, &old_byte);
    }

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    assert(result == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    if (len > 0) {
        assert_byte_from_buffer_matches(str->bytes, &old_byte);
    }

    /* 5. Validity invariant must still hold */
    assert(aws_string_is_valid(str));
}
