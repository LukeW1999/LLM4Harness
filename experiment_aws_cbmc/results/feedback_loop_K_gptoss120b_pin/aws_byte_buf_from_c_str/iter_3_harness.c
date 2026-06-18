#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Function under test */
struct aws_byte_buf aws_byte_buf_from_c_str(const char *c_str);

/* nondet helpers (provided by CBMC) */
extern size_t nondet_size_t(void);
extern bool nondet_bool(void);
extern uint8_t nondet_uint8_t(void);

void aws_byte_buf_from_c_str_harness(void) {
    /* nondeterministic length for the string (including zero length) */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 256);               /* bound the size for tractability */

    /* nondeterministically decide whether c_str is NULL or points to a string */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[len] = '\0';
        c_str = tmp;
    }

    /* Snapshot of input string to verify it is unchanged */
    uint8_t *snapshot = NULL;
    size_t snap_len = 0;
    if (c_str != NULL) {
        snap_len = len + 1;
        snapshot = malloc(snap_len);
        __CPROVER_assume(snapshot != NULL);
        memcpy(snapshot, c_str, snap_len);
    }

    /* Call the function under verification */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postcondition checks */
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        if (buf.buffer != NULL) {
            assert(buf.len == expected_len);
            assert(buf.capacity >= expected_len);
            assert(memcmp(buf.buffer, c_str, expected_len) == 0);
        } else {
            /* Allocation may have failed */
            assert(buf.len == 0);
            assert(buf.capacity == 0);
        }
    }
    assert(buf.allocator == NULL);

    /* Verify that the input string memory was not modified */
    if (c_str != NULL) {
        assert(memcmp(snapshot, c_str, snap_len) == 0);
    }
}
