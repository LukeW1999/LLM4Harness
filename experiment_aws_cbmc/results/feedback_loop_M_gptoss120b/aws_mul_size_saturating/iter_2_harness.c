#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_secure_zero_harness(void) {
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= 1024);

    bool p_is_null = nondet_bool();
    void *pBuf = NULL;

    struct aws_allocator *allocator = aws_default_allocator();

    if (!p_is_null) {
        pBuf = aws_mem_acquire(allocator, bufsize);
        __CPROVER_assume(pBuf != NULL);
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    struct store_byte_from_buffer old_mem;
    if (pBuf != NULL && bufsize > 0) {
        save_byte_from_array((uint8_t *)pBuf, bufsize, &old_mem);
    }

    aws_secure_zero(pBuf, bufsize);

    if (pBuf != NULL && bufsize > 0) {
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    if (pBuf != NULL) {
        aws_mem_release(allocator, pBuf);
    }
}
