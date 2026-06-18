#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_new_from_c_str_harness() {
    /* allocator (default, never NULL) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic length of the input C string, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate a buffer for the C string (including space for the null terminator) */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill the buffer with nondeterministic non‑zero bytes and terminate it */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[len] = '\0';

    /* save the original contents of c_str for later immutability check */
    struct store_byte_from_buffer old_c_str;
    save_byte_from_array((const uint8_t *)c_str, len, &old_c_str);

    /* remember the original allocator pointer (should not change) */
    struct aws_allocator *old_allocator = allocator;

    /* call the function under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* post‑condition checks */
    if (result != NULL) {
        /* the newly created string must be valid */
        assert(aws_string_is_valid(result));

        /* allocator stored in the string must be the one we passed in */
        assert(result->allocator == allocator);

        /* length must equal the length of the input C string */
        assert(result->len == len);

        /* the bytes of the string must match the input bytes */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);
    }

    /* the input C string must remain unchanged */
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str);

    /* the allocator pointer argument must remain unchanged */
    assert(allocator == old_allocator);
}
