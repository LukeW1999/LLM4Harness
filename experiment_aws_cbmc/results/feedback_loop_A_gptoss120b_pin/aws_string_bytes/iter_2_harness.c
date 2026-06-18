#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* 1. Allocate and bound an aws_string on the stack */
    size_t max_len = 256;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    uint8_t storage[sizeof(struct aws_string) + max_len + 1];
    struct aws_string *s = (struct aws_string *)storage;

    s->allocator = aws_default_allocator();
    s->len = len;

    /* make the bytes readable and set a null terminator */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)s->bytes)[i] = nondet_uint8_t();
    }
    ((uint8_t *)s->bytes)[len] = 0; /* null terminator */

    __CPROVER_assume(aws_string_is_valid(s));

    /* 2. Save old immutable state */
    struct aws_string old = *s;                     /* shallow copy of fixed fields */
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(s->bytes, s->len + 1, &old_bytes);

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(s);

    /* 4. Post‑condition: return value points to the string's bytes */
    assert(result == s->bytes);

    /* 5. Unchanged fields (immutable on both success and “failure” – function never fails) */
    assert(s->allocator == old.allocator);
    assert(s->len == old.len);
    assert_byte_from_buffer_matches(s->bytes, &old_bytes);

    /* 6. Validity invariant must still hold */
    assert(aws_string_is_valid(s));
}
