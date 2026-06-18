#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to nondeterministically decide a boolean value */
static bool nondet_bool(void);
static size_t nondet_size_t(void);

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    /* Non‑deterministically decide whether we have a string */
    if (nondet_bool()) {
        /* Choose a non‑deterministic length for the flexible array member */
        size_t len = nondet_size_t();
        __CPROVER_assume(len < 1024);               /* keep allocation bounded */

        /* Allocate the struct plus the flexible array */
        str = __CPROVER_allocate(sizeof(struct aws_string) + len, 0);
        __CPROVER_assume(str != NULL);

        /* Initialise the fields */
        str->allocator = aws_default_allocator();
        str->len = len;

        /* Initialise the byte contents (including the implicit null terminator) */
        for (size_t i = 0; i < len + 1; ++i) {
            ((uint8_t *)str->bytes)[i] = (uint8_t) nondet_size_t();
        }
    }

    /* Structural validity assumption required by the specification */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    /* Call the function under verification */
    aws_string_destroy(str);

    /* --------------------------------------------------------------------- */
    /* Post‑condition checks */
    /* --------------------------------------------------------------------- */

    /* 1. Return‑value / error‑code correctness: the function returns void,
       so there is nothing to check here. */

    /* 2. Output‑buffer length / capacity invariants:
       The function does not produce any output buffers, so no length
       invariants need to be asserted. */

    /* 3. Frame conditions (memory not modified beyond the contract) */

    /* If the string was NULL or had a NULL allocator, the function must not
       modify the string object. */
    if (str == NULL || str->allocator == NULL) {
        /* The string (if any) must still satisfy its validity predicate. */
        assert(str == NULL || aws_string_is_valid(str));
    }

    /* If the string had a non‑NULL allocator, the memory may have been
       released.  In that case we cannot safely dereference `str` after the
       call, so we simply avoid accessing it.  The absence of a crash
       (i.e., no undefined behaviour) is the only guarantee we can assert
       in this harness. */
    return 0;
}

/* Definitions of the nondeterministic helpers for CBMC */
bool nondet_bool(void) {
    bool b;
    return b;
}

size_t nondet_size_t(void) {
    size_t s;
    return s;
}
