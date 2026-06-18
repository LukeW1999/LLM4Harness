#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool make_null = nondet_bool();

    unsigned char *buf = NULL;
    unsigned char *old_buf = NULL;
    void *pBuf = NULL;

    if (!make_null && bufsize > 0) {
        buf = (unsigned char *)malloc(bufsize);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < bufsize; ++i) {
            buf[i] = nondet_uint8_t();
        }
        pBuf = buf;

        old_buf = (unsigned char *)malloc(bufsize);
        __CPROVER_assume(old_buf != NULL);
        for (size_t i = 0; i < bufsize; ++i) {
            old_buf[i] = buf[i];
        }
    } else {
        pBuf = NULL;
    }

    size_t old_bufsize = bufsize;
    void *old_pBuf = pBuf;

    aws_secure_zero(pBuf, bufsize);

    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (old_pBuf != NULL && old_bufsize > 0) {
        for (size_t i = 0; i < bufsize; ++i) {
            assert(buf[i] == 0);
        }
    }

    if (buf != NULL) {
        free(buf);
    }
    if (old_buf != NULL) {
        free(old_buf);
    }
}
