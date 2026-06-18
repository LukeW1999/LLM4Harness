#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Bound for the length of the string we will create */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN MAX_BUFFER_SIZE
#endif

void aws_string_bytes_harness(void) {
    /* 1. Allocate and nondeterministically initialize an aws_string */
    struct aws_string *str;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate enough memory for the struct plus (len) bytes.
     * The struct already contains one byte in the flexible array member,
     * so we allocate (len) bytes total, which is sizeof(struct aws_string) + len - 1. */
    str = (struct aws_string *)malloc(sizeof(struct aws_string) + len - 1);
    __CPROVER_assume(str != NULL);

    /* Initialize fields */
    str->allocator = aws_default_allocator();
    /* The struct member `len` is const, but we can assign it here because the
     * memory is freshly allocated and the const qualifier only prevents later
     * modification. */
    *((size_t *)&str->len) = len; /* cast away const for initialization */

    /* Initialize the bytes with nondeterministic values */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* 2. Assume the string is valid according to the library invariant */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 3. Save old immutable state for later comparison */
    struct aws_string old = *str; /* shallow copy of the header */
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len, &old_bytes);

    /* 4. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 5. Post‑condition: the returned pointer must be the address of the bytes field */
    assert(result == str->bytes);

    /* 6. Unchanged fields: allocator and length must stay the same */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* 7. The byte contents must be unchanged */
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);

    /* 8. The string must still satisfy its validity predicate */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free((void *)str);
    return 0;
}
