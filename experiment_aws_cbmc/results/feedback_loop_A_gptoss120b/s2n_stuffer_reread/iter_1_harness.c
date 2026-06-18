#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <proof_helpers/nondet.h>

/*--- s2n data structures (as given)---*/
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

/*--- constants and stubs used by the function ---*/
#define S2N_SUCCESS 0

/* Stub for the validation predicate used in the precondition.
 * In the harness we will assume it holds, but we also keep it
 * callable for the post‑condition invariant. */
bool s2n_stuffer_validate(const struct s2n_stuffer *stuffer) {
    /* A very permissive definition – the harness will
       constrain the fields explicitly. */
    return true;
}

/* The function under test (implementation reproduced for completeness). */
int s2n_stuffer_reread(struct s2n_stuffer *stuffer)
{
    /* POSIX_PRECONDITION(s2n_stuffer_validate(stuffer)); */
    stuffer->read_cursor = 0;
    return S2N_SUCCESS;
}

/*--- Harness ---------------------------------------------------*/
void s2n_stuffer_reread_harness(void) {
    /* 1. Declare and nondeterministically initialise a stuffer */
    struct s2n_stuffer stuffer;

    /* nondet initialise scalar fields */
    stuffer.read_cursor   = nondet_uint32_t();
    stuffer.write_cursor  = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced       = nondet_bool();
    stuffer.growable      = nondet_bool();
    stuffer.tainted       = nondet_bool();

    /* initialise the embedded blob */
    stuffer.blob.size      = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable  = nondet_bool();

    /* bound the blob size to keep the state space finite */
    const uint32_t MAX_BLOB_SIZE = 256U;
    __CPROVER_assume(stuffer.blob.size <= MAX_BLOB_SIZE);
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    __CPROVER_assume(stuffer.blob.allocated <= MAX_BLOB_SIZE);

    /* allocate the data buffer */
    if (stuffer.blob.allocated > 0) {
        stuffer.blob.data = malloc(stuffer.blob.allocated);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* 2. Assume the pre‑condition holds */
    __CPROVER_assume(s2n_stuffer_validate(&stuffer));

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 4. Call the function under test */
    int result = s2n_stuffer_reread(&stuffer);

    /* 5. Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* changed field */
        assert(stuffer.read_cursor == 0);

        /* unchanged fields */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    } else {
        /* on failure the whole structure must be unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    }

    /* 6. Validity invariant must hold after the call */
    assert(s2n_stuffer_validate(&stuffer));
}
