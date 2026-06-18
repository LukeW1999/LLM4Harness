#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* nondeterministically decide whether to allocate a string or use NULL */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);               /* bound the allocation */

        /* allocate enough space for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);                         /* ensure allocation succeeded */

        /* nondeterministically choose an allocator (NULL or the default) */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        /* initialise the byte payload with nondeterministic values */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    /* validity pre‑condition */
    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    /* save old state */
    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    /* call the function under test */
    aws_string_destroy(str);

    /* -------------------------------------------------------------------- */
    /* Post‑conditions */

    /* the pointer itself must remain unchanged */
    assert(str == old_str);

    if (old_str != NULL && old_allocator != NULL) {
        /* memory was released – we must not dereference the object any more */
        /* no further field checks are performed in this case */
    } else if (old_str != NULL) {
        /* allocator was NULL, so no release happened – fields must be unchanged */
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        if (old_len > 0) {
            assert(((uint8_t *)str->bytes)[0] == ((uint8_t *)old_str->bytes)[0]);
        }
        /* the string must still be valid */
        assert(aws_string_is_valid(str));
    } else {
        /* str was NULL – nothing to check */
    }

    /* -------------------------------------------------------------------- */
    /* In all cases where the string object is still alive, it must satisfy
       the invariant that a valid aws_string is well‑formed. */
    if (str != NULL && old_allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
