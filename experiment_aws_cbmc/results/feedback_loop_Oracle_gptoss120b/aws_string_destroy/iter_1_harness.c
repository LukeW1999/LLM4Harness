#include <aws/common/string.h>
#include <aws/common/memory.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether to allocate a string or use NULL */
    if (__CPROVER_bool()) {
        /* allocate a valid string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 1024);               /* keep allocation bounded */

        struct aws_allocator *alloc = __CPROVER_bool()
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
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0;   /* NUL terminator */
    } else {
        str = NULL;
    }

    /* structural validity assumption required by the proof */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    /* --------------------------------------------------------------------- */
    /* Snapshot the observable state when the string is *not* owned by an   */
    /* allocator (i.e. allocator == NULL).  In that case aws_string_destroy */
    /* must not modify the memory.                                          */
    /* --------------------------------------------------------------------- */
    uint8_t *bytes_snapshot = NULL;
    size_t   len_snapshot   = 0;
    if (str != NULL && str->allocator == NULL) {
        len_snapshot   = str->len;
        bytes_snapshot = malloc(len_snapshot + 1);
        __CPROVER_assume(bytes_snapshot != NULL);
        memcpy(bytes_snapshot, str->bytes, len_snapshot + 1);
    }

    /* --------------------------------------------------------------------- */
    /* Call the function under verification                                    */
    /* --------------------------------------------------------------------- */
    aws_string_destroy(str);

    /* --------------------------------------------------------------------- */
    /* Post‑conditions                                                       */
    /* --------------------------------------------------------------------- */

    /* 1. No return value / error code to check – function is void.          */

    /* 2. No length or capacity invariants to enforce for a destroy routine.*/

    /* 3. Frame condition: when the string is not owned (allocator == NULL) */
    /*    the memory must remain unchanged.                                 */
    if (str != NULL && str->allocator == NULL) {
        assert(str->allocator == NULL);
        assert(str->len == len_snapshot);
        for (size_t i = 0; i < len_snapshot + 1; ++i) {
            assert(str->bytes[i] == bytes_snapshot[i]);
        }
    }

    return 0;
}
