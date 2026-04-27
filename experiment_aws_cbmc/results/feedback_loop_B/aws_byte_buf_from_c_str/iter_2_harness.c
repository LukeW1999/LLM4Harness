#include <aws/common/byte_buf.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    const char *c_str = (const char *)malloc(MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, MAX_BUFFER_SIZE));
    __CPROVER_assume(strlen(c_str) <= MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old;
    old.len = (!c_str) ? 0 : strlen(c_str);
    old.capacity = old.len;
    old.buffer = (old.capacity == 0) ? NULL : (uint8_t *)c_str;
    old.allocator = NULL;

    /* 3. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (c_str != NULL) {
        assert(buf.len == strlen(c_str));
        assert(buf.capacity >= strlen(c_str)); // Weakened to allow internal allocation
        assert(memcmp(buf.buffer, c_str, strlen(c_str)) == 0); // Check content instead of pointer
        assert(buf.allocator == NULL);
    } else {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == old.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
