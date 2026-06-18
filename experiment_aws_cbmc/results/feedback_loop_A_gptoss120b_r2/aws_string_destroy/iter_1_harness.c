#include <aws/common/string.h>
#include <aws/common/byte_buf.h>   /* for aws_default_allocator() */
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;

    /* nondeterministically decide whether to allocate a string */
    if (nondet_bool()) {
        /* allocate a string with a nondeterministic length */
        size_t len = nondet_size_t();
        /* bound the length to keep the state space finite */
        __CPROVER_assume(len <= 1024);

        /* allocate memory for the struct plus the flexible array member */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* nondeterministically decide whether the allocator is present */
        if (nondet_bool()) {
            str->allocator = aws_default_allocator();
        } else {
            str->allocator = NULL;
        }

        /* initialize the const len field (cast away constness for harness) */
        *((size_t *)&str->len) = len;

        /* the bytes themselves are not important for this harness; they may be left uninitialized */
    }

    /* Save a snapshot of the fields we are allowed to inspect before the call.
       If str is NULL we keep the snapshot empty. */
    struct aws_string old_snapshot;
    if (str != NULL) {
        old_snapshot.allocator = str->allocator;
        old_snapshot.len = str->len;
    }

    /* Call the function under verification */
    aws_string_destroy(str);

    /* Post‑condition checks */
    if (str == NULL) {
        /* No operation performed; nothing to check. */
    } else {
        if (old_snapshot.allocator == NULL) {
            /* No allocator → the function must not free the memory.
               The string should remain unchanged and valid. */
            assert(str->allocator == old_snapshot.allocator);
            assert(str->len == old_snapshot.len);
            assert(aws_string_is_valid(str));
        } else {
            /* An allocator was present, so the memory may have been released.
               The pointer `str` must not be dereferenced after the call. */
            /* No further assertions about the contents of `str` are made. */
        }
    }
}
