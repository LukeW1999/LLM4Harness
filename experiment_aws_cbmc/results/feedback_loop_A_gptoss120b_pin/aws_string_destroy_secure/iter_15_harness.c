#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

bool nondet_bool(void);
uint64_t nondet_uint64_t(void);
uint8_t nondet_uint8_t(void);

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;

    /* storage that can hold the string struct plus its bytes */
    union {
        struct aws_string s;
        unsigned char bytes[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    } storage;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        str = &storage.s;
        str->len = len;

        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* initialise the bytes with nondeterministic values */
        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* Save old state that we can still observe after the call */
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    /* Call the function under verification */
    aws_string_destroy_secure(str);

    /* Post‑condition checks */
    if (str) {
        if (old_allocator == NULL) {
            const uint8_t *bytes = aws_string_bytes(str);
            for (size_t i = 0; i < old_len; ++i) {
                assert(bytes[i] == 0);
            }
        } else {
            /* Dynamically allocated string: memory may have been released.
               No further dereferencing of `str` is allowed. */
        }
    } else {
        assert(str == NULL);
    }
}
