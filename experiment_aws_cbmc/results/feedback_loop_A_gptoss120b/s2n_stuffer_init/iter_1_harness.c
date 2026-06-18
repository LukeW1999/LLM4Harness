#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

/* Forward declarations of validation functions used in the implementation */
bool s2n_blob_validate(const struct s2n_blob *blob);
bool s2n_stuffer_validate(const struct s2n_stuffer *stuffer);

/* Structures from the header (replicated for the harness) */
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

/* Function under test */
int s2n_stuffer_init(struct s2n_stuffer *stuffer, struct s2n_blob *in);

/* Harness */
void s2n_stuffer_init_harness(void) {
    /* 1. Declare and nondeterministically initialize inputs */
    struct s2n_blob in_blob;
    in_blob.data = NULL;
    in_blob.size = nondet_uint32_t();
    in_blob.allocated = nondet_uint32_t();
    in_blob.growable = nondet_bool();

    /* Bounding assumptions for the blob */
    __CPROVER_assume(in_blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(in_blob.allocated <= MAX_BUFFER_SIZE);
    __CPROVER_assume(in_blob.allocated == 0 || in_blob.allocated >= in_blob.size);

    /* Allocate the data buffer if size is non‑zero */
    if (in_blob.size > 0) {
        in_blob.data = malloc(in_blob.size);
        __CPROVER_assume(in_blob.data != NULL);
    }

    struct s2n_stuffer st;
    /* Nondeterministically initialize the stuffer fields (they may contain any values) */
    st.blob.data = (uint8_t *)nondet_uintptr_t();
    st.blob.size = nondet_uint32_t();
    st.blob.allocated = nondet_uint32_t();
    st.blob.growable = nondet_bool();
    st.read_cursor = nondet_uint32_t();
    st.write_cursor = nondet_uint32_t();
    st.high_water_mark = nondet_uint32_t();
    st.alloced = nondet_bool();
    st.growable = nondet_bool();
    st.tainted = nondet_bool();

    /* 2. Save old state for failure‑path checks */
    struct s2n_stuffer old = st;

    /* 3. Call the function under test */
    int result = s2n_stuffer_init(&st, &in_blob);

    /* 4. Post‑condition checks for both success and failure */
    if (result == 0) { /* S2N_SUCCESS */
        /* Fields that must be set on success */
        assert(st.blob.data == in_blob.data);
        assert(st.blob.size == in_blob.size);
        assert(st.blob.allocated == in_blob.allocated);
        assert(st.blob.growable == in_blob.growable);
        assert(st.read_cursor == 0);
        assert(st.write_cursor == 0);
        assert(st.high_water_mark == 0);
        assert(st.alloced == 0);
        assert(st.growable == 0);
        assert(st.tainted == 0);
    } else {
        /* On failure the stuffer must remain unchanged */
        assert(st.blob.data == old.blob.data);
        assert(st.blob.size == old.blob.size);
        assert(st.blob.allocated == old.blob.allocated);
        assert(st.blob.growable == old.blob.growable);
        assert(st.read_cursor == old.read_cursor);
        assert(st.write_cursor == old.write_cursor);
        assert(st.high_water_mark == old.high_water_mark);
        assert(st.alloced == old.alloced);
        assert(st.growable == old.growable);
        assert(st.tainted == old.tainted);
    }

    /* 5. Invariant that must always hold */
    assert(s2n_stuffer_validate(&st));
}
