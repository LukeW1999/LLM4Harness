#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    bool is_null = nondet_bool();
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    void *pBuf = NULL;
    uint8_t *byte_buf = NULL;
    uint8_t *old_buf = NULL;

    if (is_null) {
        pBuf = NULL;
        __CPROVER_assume(bufsize == 0);
    } else {
        if (bufsize > 0) {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            byte_buf = (uint8_t *)pBuf;

            for (size_t i = 0; i < bufsize; ++i) {
                byte_buf[i] = nondet_uint8_t();
            }

            old_buf = malloc(bufsize);
            __CPROVER_assume(old_buf != NULL);
            for (size_t i = 0; i < bufsize; ++i) {
                old_buf[i] = byte_buf[i];
            }
        } else {
            pBuf = malloc(1);
            __CPROVER_assume(pBuf != NULL);
            byte_buf = (uint8_t *)pBuf;
        }
    }

    size_t old_bufsize = bufsize;
    void *old_pBuf = pBuf;

    aws_secure_zero(pBuf, bufsize);

    if (pBuf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; ++i) {
            assert(byte_buf[i] == 0);
        }
    } else {
        if (bufsize > 0 && pBuf != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(byte_buf[i] == old_buf[i]);
            }
        }
    }

    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_buf != NULL) {
        free(old_buf);
    }
}
