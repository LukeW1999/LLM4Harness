#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_alloc_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint32_t size = nondet_uint32_t();

    /* 2. Assume preconditions */
    __CPROVER_assume(size > 0);
    __CPROVER_assume(!stuffer->alloced);
    __CPROVER_assume(stuffer->blob.data == NULL);
    __CPROVER_assume(stuffer->blob.size == 0);
    __CPROVER_assume(stuffer->blob.allocated == 0);
    __CPROVER_assume(stuffer->read_cursor == 0);
    __CPROVER_assume(stuffer->write_cursor == 0);
    __CPROVER_assume(stuffer->high_water_mark == 0);
    __CPROVER_assume(stuffer->growable == 0);
    __CPROVER_assume(stuffer->tainted == 0);

    /* 3. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 4. Call function under test */
    int result = s2n_stuffer_alloc(stuffer, size);

    /* 5. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        assert(stuffer->blob.data != NULL);
        assert(stuffer->blob.size == size);
        assert(stuffer->blob.allocated == size);
        assert(stuffer->read_cursor == 0);
        assert(stuffer->write_cursor == 0);
        assert(stuffer->high_water_mark == 0);
        assert(stuffer->alloced == 1);
        assert(stuffer->growable == 0);
        assert(stuffer->tainted == 0);
    } else {
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
    }

    /* 6. Assert the stuffer data (blob) is unmodified (if applicable) */
    if (result != S2N_SUCCESS) {
        assert_byte_from_blob_matches(&stuffer->blob, &old_byte);
    }

    /* 7. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
