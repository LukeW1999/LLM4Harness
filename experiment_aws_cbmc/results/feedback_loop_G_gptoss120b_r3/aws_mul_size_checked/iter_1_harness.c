#include <aws/common/math.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size bounded by the test harness limit */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministically decide whether the buffer pointer is NULL */
    void *pBuf;
    struct store_byte_from_buffer old_bytes;
    bool have_buffer = nondet_bool();

    if (have_buffer) {
        /* allocate a writable buffer of the chosen size */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* remember the original contents for later comparison */
        save_byte_from_array((uint8_t *)pBuf, bufsize, &old_bytes);
    } else {
        pBuf = NULL;
    }

    /* call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* post‑conditions */
    if (pBuf != NULL && bufsize != 0) {
        /* the buffer must be completely zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        /* when the pointer is NULL the function asserts bufsize == 0 */
        if (pBuf == NULL) {
            assert(bufsize == 0);
        }
        /* when the size is zero the buffer (if any) must remain unchanged */
        if (bufsize == 0 && pBuf != NULL) {
            assert_byte_from_buffer_matches((uint8_t *)pBuf, &old_bytes);
        }
    }

    /* clean up */
    free(pBuf);
}
