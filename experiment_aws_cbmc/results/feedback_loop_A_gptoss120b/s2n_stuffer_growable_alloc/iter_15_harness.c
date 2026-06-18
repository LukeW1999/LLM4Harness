#include <proof_helpers/make_common_data_structures.h>

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
        __CPROVER_assert(st.growable == 1, "growable set on success");
        __CPROVER_assert(st.read_cursor == old.read_cursor, "read_cursor unchanged on success");
        __CPROVER_assert(st.write_cursor == old.write_cursor, "write_cursor unchanged on success");
        __CPROVER_assert(st.high_water_mark == old.high_water_mark, "high_water_mark unchanged on success");
        __CPROVER_assert(st.alloced == old.alloced, "alloced unchanged on success");
        __CPROVER_assert(st.tainted == old.tainted, "tainted unchanged on success");
    } else {
        __CPROVER_assert(st.growable == old.growable, "growable unchanged on failure");
        __CPROVER_assert(st.read_cursor == old.read_cursor, "read_cursor unchanged on failure");
        __CPROVER_assert(st.write_cursor == old.write_cursor, "write_cursor unchanged on failure");
        __CPROVER_assert(st.high_water_mark == old.high_water_mark, "high_water_mark unchanged on failure");
        __CPROVER_assert(st.alloced == old.alloced, "alloced unchanged on failure");
        __CPROVER_assert(st.tainted == old.tainted, "tainted unchanged on failure");
        __CPROVER_assert(st.blob.size == old.blob.size, "blob.size unchanged on failure");
        __CPROVER_assert(st.blob.allocated == old.blob.allocated, "blob.allocated unchanged on failure");
        __CPROVER_assert(st.blob.data == old.blob.data, "blob.data unchanged on failure");
        __CPROVER_assert(st.blob.growable == old.blob.growable, "blob.growable unchanged on failure");
    }

    __CPROVER_assert(s2n_stuffer_validate(&st), "stuffer valid at end");
}
