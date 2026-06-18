#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_bytes_harness(void) {
    /* 1. Declare and bound the aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate enough memory for the struct plus the flexible array */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Initialize const members via casts (allowed in proof harness) */
    *((struct aws_allocator **)&str->allocator) = aws_default_allocator();
    *((size_t *)&str->len) = len;

    /* Nondeterministically initialize the bytes */
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    /* 2. Save old state */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len, &old_bytes);

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    /* The returned pointer must be the address of the bytes array */
    assert(result == str->bytes);

    /* Unchanged fields */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);

    /* 5. Validity invariant */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free(str);
}
