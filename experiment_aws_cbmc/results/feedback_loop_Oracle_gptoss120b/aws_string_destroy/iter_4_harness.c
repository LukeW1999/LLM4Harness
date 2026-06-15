#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether to allocate a string or use NULL */
    _Bool allocate = __CPROVER_nondet_bool();
    if (allocate) {
        /* allocate a valid string */
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len < 1024);               /* keep allocation bounded */

        struct aws_allocator *alloc = __CPROVER_nondet_bool()
                                      ? aws_default_allocator()
                                      : NULL;

        /* allocate memory for the struct plus the flexible array member */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        str->allocator = alloc;
        str->len       = len;

        /* initialise the bytes (including the required NUL terminator) */
        uint8_t *bytes = (uint8_t *)str->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = __CPROVER_nondet_uint8_t();
        }
        bytes[len] = 0;   /* NUL terminator */
    } else {
        str = NULL;
    }

    /* structural validity assumption required by the proof */
    __CPROVER_assume(!(str != NULL) || aws_string_is_valid(str));

    /* Snapshot the observable state when the string is *not* owned by an allocator */
    uint8_t *bytes_snapshot = NULL;
    size_t   len_snapshot   = 0;
    if (str != NULL && str->allocator == NULL) {
        len_snapshot   = str->len;
        bytes_snapshot = malloc(len_snapshot + 1);
        __CPROVER_assume(bytes_snapshot != NULL);
        memcpy(bytes_snapshot, str->bytes, len_snapshot + 1);
    }

    /* Call the function under verification */
    aws_string_destroy(str);

    /* Frame condition: when the string is not owned (allocator == NULL) the memory must remain unchanged */
    if (str != NULL && str->allocator == NULL) {
        assert(str->allocator == NULL);
        assert(str->len == len_snapshot);
        for (size_t i = 0; i < len_snapshot + 1; ++i) {
            assert(str->bytes[i] == bytes_snapshot[i]);
        }
    }
}
