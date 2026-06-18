#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

/* Replicated definitions from s2n headers */
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

/* External functions used by the implementation */
extern int s2n_stuffer_alloc(struct s2n_stuffer *stuffer, const uint32_t size);
extern int s2n_stuffer_validate(const struct s2n_stuffer *stuffer);
extern int s2n_stuffer_growable_alloc(struct s2n_stuffer *stuffer, uint32_t size);
#define S2N_SUCCESS 0

void s2n_stuffer_growable_alloc_harness(void) {
    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer st;
    st.blob.data = NULL;
    st.blob.size = nondet_uint32_t();
    st.blob.allocated = nondet_uint32_t();
    st.blob.growable = nondet_bool();
    st.read_cursor = nondet_uint32_t();
    st.write_cursor = nondet_uint32_t();
    st.high_water_mark = nondet_uint32_t();
    st.alloced = nondet_bool();
    st.growable = nondet_bool();
    st.tainted = nondet_bool();

    /* 2. Apply bounding assumptions */
    __CPROVER_assume(st.blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(st.blob.allocated <= MAX_BUFFER_SIZE);
    __CPROVER_assume(st.read_cursor <= st.blob.size);
    __CPROVER_assume(st.write_cursor <= st.blob.size);
    __CPROVER_assume(st.high_water_mark <= st.blob.size);

    /* 3. Allocate the blob's data buffer if it is supposed to be allocated */
    if (st.blob.allocated > 0) {
        st.blob.data = malloc(st.blob.allocated);
        __CPROVER_assume(st.blob.data != NULL);
    }

    /* 4. Assume the stuffer is valid before the call */
    __CPROVER_assume(s2n_stuffer_validate(&st));

    /* 5. Save the old state for immutability checks */
    struct s2n_stuffer old = st;

    /* 6. Nondeterministic size argument, bounded */
    uint32_t size = nondet_uint32_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    /* 7. Call the function under test */
    int result = s2n_stuffer_growable_alloc(&st, size);

    /* 8. Post‑condition checks for both success and failure paths */
    if (result == S2N_SUCCESS) {
        assert(st.growable == 1);
        assert(st.read_cursor == old.read_cursor);
        assert(st.write_cursor == old.write_cursor);
        assert(st.high_water_mark == old.high_water_mark);
        assert(st.alloced == old.alloced);
        assert(st.tainted == old.tainted);
    } else {
        assert(st.growable == old.growable);
        assert(st.read_cursor == old.read_cursor);
        assert(st.write_cursor == old.write_cursor);
        assert(st.high_water_mark == old.high_water_mark);
        assert(st.alloced == old.alloced);
        assert(st.tainted == old.tainted);
        assert(st.blob.size == old.blob.size);
        assert(st.blob.allocated == old.blob.allocated);
        assert(st.blob.data == old.blob.data);
        assert(st.blob.growable == old.blob.growable);
    }

    /* 9. Validity invariant must hold after the call */
    assert(s2n_stuffer_validate(&st));
}
