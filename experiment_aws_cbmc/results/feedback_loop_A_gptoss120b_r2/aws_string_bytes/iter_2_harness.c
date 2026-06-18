#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* 1. Declare and bound the aws_string */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate storage for the struct plus flexible array on the stack */
    uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *str = (struct aws_string *)storage;

    /* initialise fields (constness is ignored for harness purposes) */
    str->allocator = aws_default_allocator();
    *((size_t *)&str->len) = len;               /* cast away const to set length */

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();       /* nondet contents */
    }

    /* assume the string satisfies the library's validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    struct aws_string old = *str;
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len, &old_bytes);

    /* 3. Call function under test */
    const uint8_t *ret = aws_string_bytes(str);

    /* 4. Post‑condition: return value points to the internal bytes */
    assert(ret == str->bytes);

    /* 5. Unchanged fields */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);

    /* 6. Validity invariant must still hold */
    assert(aws_string_is_valid(str));
}
