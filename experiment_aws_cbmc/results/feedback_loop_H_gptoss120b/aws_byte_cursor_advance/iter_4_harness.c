#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>

/* Define a reasonable maximum buffer size for the harness */
#define MAX_BUFFER_SIZE 256

/* Nondeterministic size generator for CBMC */
size_t nondet_size_t(void);

void aws_byte_cursor_advance_harness(void) {
    /* 1. Prepare a deterministic buffer */
    static uint8_t buffer[MAX_BUFFER_SIZE];

    /* 2. Initialise the original cursor with nondeterministic length */
    struct aws_byte_cursor orig;
    orig.ptr = buffer;
    orig.len = nondet_size_t();
    __CPROVER_assume(orig.len <= MAX_BUFFER_SIZE);

    /* 3. Save a copy of the original cursor for later comparison */
    struct aws_byte_cursor old = orig;

    /* 4. Choose a nondeterministic advance length */
    size_t advance_len = nondet_size_t();
    __CPROVER_assume(advance_len <= MAX_BUFFER_SIZE);

    /* 5. Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_advance(&orig, advance_len);

    /* 6. Verify post‑conditions */
    if (advance_len <= old.len) {
        /* The original cursor should be advanced */
        assert(orig.ptr == old.ptr + advance_len);
        assert(orig.len == old.len - advance_len);
        /* The returned cursor should refer to the slice that was advanced over */
        assert(result.ptr == old.ptr);
        assert(result.len == advance_len);
    } else {
        /* The original cursor should remain unchanged */
        assert(orig.ptr == old.ptr);
        assert(orig.len == old.len);
        /* The function returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    }
}
