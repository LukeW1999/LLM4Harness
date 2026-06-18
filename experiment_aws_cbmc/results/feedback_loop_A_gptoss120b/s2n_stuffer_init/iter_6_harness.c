#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

uint32_t nondet_uint32_t(void);
uintptr_t nondet_uintptr_t(void);
_Bool nondet_bool(void);

void s2n_stuffer_init_harness(void) {
    const uint32_t MAX_BUFFER_SIZE = 1024U;

    /* 1. Declare and nondeterministically initialize inputs */
    struct s2n_blob in_blob = {0};
    in_blob.data = NULL;
    in_blob.size = nondet_uint32_t();
    in_blob.allocated = nondet_uint32_t();
    in_blob.growable = nondet_bool();

    /* Bounding assumptions for the blob */
    __CPROVER_assume(in_blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(in_blob.allocated <= MAX_BUFFER_SIZE);
    __CPROVER_assume(in_blob.allocated == 0 || in_blob.allocated >= in_blob.size);

    /* Allocate the data buffer on the stack if size is non‑zero */
    uint8_t buffer[MAX_BUFFER_SIZE];
    if (in_blob.size > 0) {
        in_blob.data = buffer;
    }

    /* 2. Nondeterministically initialize the stuffer */
    struct s2n_stuffer st;
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

    /* 3. Save old state for failure‑path checks */
    struct s2n_stuffer old = st;

    /* 4. Call the function under test */
    int result = s2n_stuffer_init(&st, &in_blob);

    /* 5. Post‑condition checks */
    if (result == 0) { /* S2N_SUCCESS */
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

    /* 6. Invariant that must always hold */
    assert(s2n_stuffer_validate(&st));
}
