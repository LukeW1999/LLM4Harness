#include <aws/common/math.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_secure_zero_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;
    uint8_t *old_bytes = NULL;

    if (ptr_is_null) {
        pBuf = NULL;
        __CPROVER_assume(bufsize == 0);
    } else {
        if (bufsize > 0) {
            pBuf = aws_mem_acquire(allocator, bufsize);
            __CPROVER_assume(pBuf != NULL);
            for (size_t i = 0; i < bufsize; ++i) {
                ((uint8_t *)pBuf)[i] = nondet_uint8_t();
            }
            old_bytes = aws_mem_acquire(allocator, bufsize);
            __CPROVER_assume(old_bytes != NULL);
            memcpy(old_bytes, pBuf, bufsize);
        } else {
            pBuf = NULL;
        }
    }

    aws_secure_zero(pBuf, bufsize);

    if (pBuf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    } else {
        if (pBuf != NULL && bufsize == 0) {
            /* vacuously true */
        } else if (pBuf != NULL && old_bytes != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(((uint8_t *)pBuf)[i] == old_bytes[i]);
            }
        }
    }

    aws_mem_release(allocator, old_bytes);
    aws_mem_release(allocator, pBuf);
}
