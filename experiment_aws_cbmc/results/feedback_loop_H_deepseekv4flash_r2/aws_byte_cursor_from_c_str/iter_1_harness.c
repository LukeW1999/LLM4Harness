#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    /* Non-deterministic inputs */
    const char *c_str = nondet_bool() ? NULL : (const char *)malloc(nondet_size_t());
    size_t len = 0;
    if (c_str != NULL) {
        /* Ensure c_str is null-terminated */
        len = strlen(c_str);
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
    }

    /* Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* Postconditions */
    if (c_str != NULL) {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    } else {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    }

    /* Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
