#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str;
    if (nondet_bool()) {
        str = NULL;
    } else {
        /* allocate space for the struct (flexible array member has size 1) */
        size_t alloc_size = sizeof(struct aws_string);
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (NULL or default) */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* nondeterministic length */
        str->len = nondet_size_t();

        /* bytes are not accessed in the destroy function; leave uninitialized */

        /* assume the string is valid before the call */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state before calling */
    struct aws_string *old_ptr = str;
    struct aws_string old;
    if (str != NULL) {
        old = *str; /* copy the whole struct (shallow copy) */
    }

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */

    /* If the string was non‑NULL and had a non‑NULL allocator, the memory is
       released.  The pointer variable itself must remain unchanged, but the
       memory it pointed to must not be dereferenced. */
    if (old_ptr != NULL && old_ptr->allocator != NULL) {
        /* The pointer value is unchanged */
        assert(str == old_ptr);
        /* No further dereference of str is allowed – we simply avoid any
           accesses to its fields here. */
    } else {
        /* In all other cases the function does nothing, so the struct must be
           unchanged. */
        if (str != NULL) {
            assert(str->allocator == old.allocator);
            assert(str->len == old.len);
            /* The flexible array member `bytes` is not modified; a shallow
               comparison suffices for the harness. */
        } else {
            assert(str == NULL);
        }

        /* 5. Validity invariant must still hold */
        assert(aws_string_is_valid(str));
    }
}
