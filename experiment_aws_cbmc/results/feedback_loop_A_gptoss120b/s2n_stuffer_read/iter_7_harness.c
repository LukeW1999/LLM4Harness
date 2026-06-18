#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 1024U

void s2n_stuffer_read_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct s2n_blob out;
    out.size = nondet_uint32_t();
    __CPROVER_assume(out.size <= MAX_BUFFER_SIZE);
    out.allocated = nondet_uint32_t();
    __CPROVER_assume(out.allocated >= out.size);
    out.growable = nondet_bool();

    if (out.size > 0) {
        out.data = (uint8_t *)aws_mem_acquire(allocator, out.size);
        __CPROVER_assume(out.data != NULL);
    } else {
        out.data = NULL;
    }

    struct s2n_stuffer st;
    st.blob.size = nondet_uint32_t();
    __CPROVER_assume(st.blob.size <= MAX_BUFFER_SIZE);
    st.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(st.blob.allocated >= st.blob.size);
    st.blob.growable = nondet_bool();

    if (st.blob.size > 0) {
        st.blob.data = (uint8_t *)aws_mem_acquire(allocator, st.blob.size);
        __CPROVER_assume(st.blob.data != NULL);
    } else {
        st.blob.data = NULL;
    }

    st.read_cursor = nondet_uint32_t();
    __CPROVER_assume(st.read_cursor <= st.blob.size);
    st.write_cursor = nondet_uint32_t();
    __CPROVER_assume(st.read_cursor <= st.write_cursor);
    __CPROVER_assume(st.write_cursor <= st.blob.size);
    st.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(st.high_water_mark <= st.blob.size);

    st.alloced = nondet_bool();
    st.growable = nondet_bool();
    st.tainted = nondet_bool();

    struct s2n_stuffer old_st = st;
    struct s2n_blob   old_out = out;

    int result = s2n_stuffer_read(&st, &out);

    assert(result == 0 || result == -1);

    if (result == 0) {
        assert(st.read_cursor == old_st.read_cursor + out.size);
    } else {
        assert(st.read_cursor == old_st.read_cursor);
        assert(st.write_cursor == old_st.write_cursor);
        assert(st.high_water_mark == old_st.high_water_mark);
        assert(st.alloced == old_st.alloced);
        assert(st.growable == old_st.growable);
        assert(st.tainted == old_st.tainted);
        assert(st.blob.data == old_st.blob.data);
        assert(st.blob.size == old_st.blob.size);
        assert(st.blob.allocated == old_st.blob.allocated);
        assert(st.blob.growable == old_st.blob.growable);
    }

    assert(out.data == old_out.data);
    assert(out.size == old_out.size);
    assert(out.allocated == old_out.allocated);
    assert(out.growable == old_out.growable);

    assert(st.blob.data == old_st.blob.data);
    assert(st.blob.size == old_st.blob.size);
    assert(st.blob.allocated == old_st.blob.allocated);
    assert(st.blob.growable == old_st.blob.growable);
    assert(st.write_cursor == old_st.write_cursor);
    assert(st.high_water_mark == old_st.high_water_mark);
    assert(st.alloced == old_st.alloced);
    assert(st.growable == old_st.growable);
    assert(st.tainted == old_st.tainted);

    assert(st.read_cursor <= st.write_cursor);
    assert(st.write_cursor <= st.blob.size);
    assert(out.size <= out.allocated);
}
