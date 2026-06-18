#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    /* nondet length for the string */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* allocate memory for aws_string with flexible array member */
    struct aws_string *src = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
    __CPROVER_assume(src != NULL);

    /* initialise fields (allocator can be NULL, it's not used by the function) */
    src->allocator = NULL;
    src->len = len;

    /* fill the byte array with nondet data */
    for (size_t i = 0; i < len; ++i) {
        src->bytes[i] = (uint8_t) nondet_uint8();
    }

    /* structural validity assumption */
    __CPROVER_assume(aws_string_is_valid(src));

    /* snapshot the original state for frame condition checks */
    struct aws_string src_snapshot = *src;
    uint8_t *bytes_snapshot = malloc(len);
    __CPROVER_assume(bytes_snapshot != NULL);
    memcpy(bytes_snapshot, src->bytes, len);

    /* call the function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* post‑condition 1: return value correctness */
    assert(cur.ptr == aws_string_bytes(src));
    assert(cur.len == src->len);

    /* post‑condition 2: length invariants are already covered by the above asserts */

    /* post‑condition 3: frame condition – src must be unchanged */
    assert(src->allocator == src_snapshot.allocator);
    assert(src->len == src_snapshot.len);
    assert(memcmp(src->bytes, bytes_snapshot, len) == 0);

    /* clean up */
    free(bytes_snapshot);
    free((void *)src);
    return 0;
}
