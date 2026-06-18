#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* 1. Allocate a string with a flexible array member */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    /* 2. Nondeterministically choose a length within bounds */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* 3. Initialise fields */
    str->allocator = NULL;               /* static string case is allowed */
    str->len = len;

    /* 4. Initialise the byte contents (including null terminator) */
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = 0;                 /* null terminator */

    /* 5. Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 6. Save old state for immutability checks */
    struct aws_string old = *str;      /* copies allocator and len */
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len + 1, &old_bytes);

    /* 7. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 8. Post‑condition: returned pointer equals the internal bytes field */
    assert(result == str->bytes);

    /* 9. Unchanged fields */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* 10. Byte contents must be unchanged */
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);

    /* 11. Validity invariant must still hold */
    assert(aws_string_is_valid(str));

    /* 12. Clean up */
    free(str);
}
