#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024U
#endif

void s2n_stuffer_write_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct s2n_stuffer stuffer;
    stuffer.read_cursor     = nondet_uint32_t();
    stuffer.write_cursor    = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced         = nondet_bool();
    stuffer.growable        = nondet_bool();
    stuffer.tainted         = nondet_bool();

    stuffer.blob.size      = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable  = nondet_bool();

    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);

    if (stuffer.blob.size > 0) {
        stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    struct s2n_blob in;
    in.size      = nondet_uint32_t();
    in.allocated = nondet_uint32_t();
    in.growable  = nondet_bool();

    __CPROVER_assume(in.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(in.allocated >= in.size);

    if (in.size > 0) {
        in.data = (uint8_t *)aws_mem_acquire(allocator, in.size);
        __CPROVER_assume(in.data != NULL);
    } else {
        in.data = NULL;
    }

    __CPROVER_assume(s2n_stuffer_validate(&stuffer));
    __CPROVER_assume(s2n_blob_validate(&in));

    struct s2n_stuffer old_stuffer = stuffer;
    struct s2n_blob   old_in       = in;

    int result = s2n_stuffer_write(&stuffer, &in);

    if (result == 0) {
        assert(stuffer.write_cursor == old_stuffer.write_cursor + in.size);
        if (stuffer.write_cursor > old_stuffer.high_water_mark) {
            assert(stuffer.high_water_mark == stuffer.write_cursor);
        } else {
            assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        }
        assert(stuffer.read_cursor == old_stuffer.read_cursor);
        assert(stuffer.alloced   == old_stuffer.alloced);
        assert(stuffer.growable  == old_stuffer.growable);
        assert(stuffer.tainted   == old_stuffer.tainted);
        assert(stuffer.blob.data      == old_stuffer.blob.data);
        assert(stuffer.blob.size      == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable  == old_stuffer.blob.growable);
        assert(in.data      == old_in.data);
        assert(in.size      == old_in.size);
        assert(in.allocated == old_in.allocated);
        assert(in.growable  == old_in.growable);
    } else {
        assert(stuffer.read_cursor     == old_stuffer.read_cursor);
        assert(stuffer.write_cursor    == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced         == old_stuffer.alloced);
        assert(stuffer.growable        == old_stuffer.growable);
        assert(stuffer.tainted         == old_stuffer.tainted);
        assert(stuffer.blob.data       == old_stuffer.blob.data);
        assert(stuffer.blob.size       == old_stuffer.blob.size);
        assert(stuffer.blob.allocated  == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable   == old_stuffer.blob.growable);
        assert(in.data      == old_in.data);
        assert(in.size      == old_in.size);
        assert(in.allocated == old_in.allocated);
        assert(in.growable  == old_in.growable);
    }

    assert(s2n_stuffer_validate(&stuffer));
    assert(s2n_blob_validate(&in));
}
