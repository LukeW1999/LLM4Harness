#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Set up c_str — non-deterministic null-terminated string */
    /* We use a bounded array to keep the state space manageable */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';
    /* The rest of c_str is non-deterministic (already non-det from malloc in CBMC) */

    /* 3. Snapshot buf fields before the call */
    size_t old_len      = buf.len;
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;
    struct aws_allocator *old_allocator = buf.allocator;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert postconditions */

    /* FRAME: buf fields must not change */
    assert(buf.len      == old_len);
    assert(buf.capacity == old_capacity);
    assert(buf.buffer   == old_buffer);
    assert(buf.allocator == old_allocator);

    /* INVARIANTS: buf must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* RETURN VALUE: result must be consistent with aws_array_eq_c_str semantics.
     * The function returns true iff the buf contents equal the c_str contents.
     * We verify the result is a valid bool (always true for bool in C, but
     * we also check the logical consistency where we can). */
    assert(result == true || result == false);

    /* If result is true, then buf.len must equal strlen(c_str) */
    if (result) {
        assert(buf.len == strlen(c_str));
        /* Also verify byte-level match */
        if (buf.len > 0 && buf.buffer != NULL) {
            assert(memcmp(buf.buffer, c_str, buf.len) == 0);
        }
    }

    /* If result is false, either lengths differ or contents differ —
     * we cannot assert a specific negative condition without re-implementing
     * the logic, but we can assert the buf is unchanged. */
    /* (Already asserted above via frame conditions) */
}
