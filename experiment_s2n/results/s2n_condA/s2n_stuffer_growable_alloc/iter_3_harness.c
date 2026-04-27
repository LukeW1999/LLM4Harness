#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_growable_alloc_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    uint32_t size = nondet_uint32_t();

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 3. Constrain input to avoid undefined behavior */
    __CPROVER_assume(size <= UINT32_MAX - stuffer->blob.size);
    __CPROVER_assume(stuffer->read_cursor <= stuffer->write_cursor);
    __CPROVER_assume(stuffer->write_cursor <= stuffer->blob.size);
    __CPROVER_assume(stuffer->high_water_mark <= stuffer->blob.size);

    /* 4. Call function under test */
    int result = s2n_stuffer_growable_alloc(stuffer, size);

    /* 5. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        assert(stuffer->growable == 1);
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->tainted == old_stuffer.tainted);
    }

    /* 6. Assert the stuffer data (blob) is unmodified (if applicable) */
    assert_byte_from_blob_matches(&stuffer->blob, &old_byte);

    /* 7. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
