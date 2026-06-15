#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

/* CBMC harness for aws_byte_buf_from_c_str */
void aws_byte_buf_from_c_str_harness(void) {
    const char *c_str;
    /* nondeterministically decide whether c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* allocate a nondeterministic sized, readable buffer */
        size_t alloc_len = nondet_size_t();
        __CPROVER_assume(alloc_len <= MAX_BUFFER_SIZE);
        c_str = (const char *)malloc(alloc_len + 1);
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, alloc_len + 1));
        /* ensure null‑termination somewhere within the allocated region */
        ((char *)c_str)[alloc_len] = '\0';
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* expected length according to the implementation */
    size_t expected_len = (c_str == NULL) ? 0 : strlen(c_str);

    /* post‑condition checks */
    assert(buf.len == expected_len);
    assert(buf.capacity == expected_len);
    if (expected_len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == (uint8_t *)c_str);
    }
    assert(buf.allocator == NULL);

    /* validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
