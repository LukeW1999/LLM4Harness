#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    /* 1. Non‑deterministically decide whether we have a string object */
    bool have_str = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t len = 0;
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;

    if (have_str) {
        /* 2. Choose a length for the flexible array member and bound it */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* 3. Allocate memory for the struct plus the flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* 4. Non‑deterministically decide whether the string was allocated with an allocator */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            str->allocator = alloc;
        } else {
            str->allocator = NULL;
        }

        str->len = len;
        /* bytes are left uninitialized – they are not examined by the harness */

        /* 5. Save old immutable fields that are safe to read after the call */
        old_allocator = str->allocator;
        old_len = str->len;
    }

    /* 6. Call the function under test */
    aws_string_destroy(str);

    /* 7. Post‑conditions */

    /* The pointer variable itself must not be modified */
    if (have_str) {
        assert(str == str);
    } else {
        assert(str == NULL);
    }

    /* If the string had no allocator, aws_string_destroy does nothing.
       In that case the struct remains valid and its fields unchanged. */
    if (str != NULL && old_allocator == NULL) {
        assert(str->allocator == NULL);
        assert(str->len == old_len);
    }

    /* If the string had an allocator, the memory may have been released.
       We must not dereference `str` after the call in that case. */

    /* 8. Clean‑up for the case where the memory was not freed by the function */
    if (str != NULL && old_allocator == NULL) {
        free(str);
    }
}
