#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <proof_helpers/nondet.h>

/* Definitions copied from the library header */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* Function under test (implementation provided) */
bool s2n_stuffer_is_consumed(struct s2n_stuffer *stuffer)
{
    return stuffer && (stuffer->read_cursor == stuffer->write_cursor) && !stuffer->tainted;
}

/* Harness */
void s2n_stuffer_is_consumed_harness(void) {
    struct s2n_stuffer *stuffer;
    /* nondeterministically decide whether the pointer is NULL */
    if (nondet_bool()) {
        stuffer = NULL;
    } else {
        stuffer = malloc(sizeof(*stuffer));
        __CPROVER_assume(stuffer != NULL);

        /* nondeterministically initialise the fields */
        stuffer->blob.data = nondet_bool() ? NULL : malloc(nondet_uint32_t());
        stuffer->blob.size = nondet_uint32_t();
        stuffer->blob.allocated = nondet_uint32_t();
        stuffer->blob.growable = nondet_bool();

        stuffer->read_cursor = nondet_uint32_t();
        stuffer->write_cursor = nondet_uint32_t();
        stuffer->high_water_mark = nondet_uint32_t();
        stuffer->alloced = nondet_bool();
        stuffer->growable = nondet_bool();
        stuffer->tainted = nondet_bool();
    }

    /* Save old state when the pointer is non‑NULL */
    struct s2n_stuffer old;
    if (stuffer != NULL) {
        old = *stuffer;
    }

    /* Call the function under test */
    bool result = s2n_stuffer_is_consumed(stuffer);

    /* Post‑condition: result must equal the logical condition */
    bool expected = (stuffer != NULL &&
                     stuffer->read_cursor == stuffer->write_cursor &&
                     !stuffer->tainted);
    assert(result == expected);

    /* Unchanged fields when the pointer is non‑NULL */
    if (stuffer != NULL) {
        assert(stuffer->read_cursor   == old.read_cursor);
        assert(stuffer->write_cursor  == old.write_cursor);
        assert(stuffer->high_water_mark == old.high_water_mark);
        assert(stuffer->alloced       == old.alloced);
        assert(stuffer->growable      == old.growable);
        assert(stuffer->tainted       == old.tainted);

        assert(stuffer->blob.data     == old.blob.data);
        assert(stuffer->blob.size     == old.blob.size);
        assert(stuffer->blob.allocated== old.blob.allocated);
        assert(stuffer->blob.growable == old.blob.growable);
    }

    /* No additional validity predicates are defined for s2n_stuffer,
       but the structure must remain well‑formed (no NULL dereference). */
    if (stuffer != NULL) {
        /* The blob pointer, if non‑NULL, must be a valid pointer (no further checks). */
        assert(stuffer->blob.data == old.blob.data);
    }
}
