#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 1024
#define S2N_SUCCESS 0

extern int s2n_stuffer_alloc(struct s2n_stuffer *stuffer, const uint32_t size);
extern int s2n_stuffer_validate(const struct s2n_stuffer *stuffer);
extern int s2n_stuffer_growable_alloc(struct s2n_stuffer *stuffer, uint32_t size);

void s2n_stuffer_growable_alloc_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

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

    __CPROVER_assume(st.blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(st.blob.allocated <= MAX_BUFFER_SIZE);
    __CPROVER_assume(st.read_cursor <= st.blob.size);
    __CPROVER_assume(st.write_cursor <= st.blob.size);
    __CPROVER_assume(st.high_water_mark <= st.blob.size);

    if (st.blob.allocated > 0) {
        st.blob.data = malloc(st.blob.allocated);
        __CPROVER_assume(st.blob.data != NULL);
    }

    __CPROVER_assume(s2n_stuffer_validate(&st));

    struct s2n_stuffer old = st;

    uint32_t size = nondet_uint32_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    int result = s2n_stuffer_growable_alloc(&st, size);

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

    assert(s2n_stuffer_validate(&st));
}
