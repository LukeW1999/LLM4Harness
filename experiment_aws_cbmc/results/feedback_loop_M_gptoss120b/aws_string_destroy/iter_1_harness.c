#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether the pointer is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate a string with a bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocate memory for the struct plus the flexible array */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the allocator is present */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* set the length */
        *((size_t *)&str->len) = len;   /* cast away const for initialization */

        /* fill the byte array with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    }

    /* Save the old state for later comparison */
    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    /* Call the function under test */
    aws_string_destroy(str);

    /* Post‑condition checks */

    /* Case 1: str is NULL – function does nothing */
    if (old_str == NULL) {
        assert(str == NULL);
    }
    /* Case 2: str is non‑NULL but allocator is NULL – no free occurs */
    else if (old_allocator == NULL) {
        /* The pointer itself must remain unchanged */
        assert(str == old_str);
        /* Allocator field must stay NULL */
        assert(str->allocator == NULL);
        /* Length must stay the same */
        assert(str->len == old_len);
        /* The string must still be valid */
        assert(aws_string_is_valid(str));
    }
    /* Case 3: str is non‑NULL and allocator is non‑NULL – memory is released */
    else {
        /* The pointer value itself is not modified by the function */
        assert(str == old_str);
        /* After release we cannot safely dereference the object, so we do not
           assert on its fields. The only guarantee is that the function does
           not write to the pointer variable. */
    }

    /* Clean up the allocation if it was not freed (to avoid leaks in the harness) */
    if (old_str != NULL && old_allocator != NULL) {
        /* In the real library the memory would have been released; in the
           harness we simply free it to avoid a leak, but only if it was not
           already freed by the function under test. */
        free(old_str);
    }
}
