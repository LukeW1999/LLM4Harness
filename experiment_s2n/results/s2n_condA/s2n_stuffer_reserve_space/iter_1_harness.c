#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_reserve_space_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    uint32_t n = nondet_uint32_t();

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 3. Call function under test */
    int result = s2n_stuffer_reserve_space(stuffer, n);

    /* 4. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        if (s2n_stuffer_space_remaining(&old_stuffer) < n) {
            assert(stuffer->blob.size >= old_stuffer.blob.size + n);
        } else {
            assert(stuffer->blob.size == old_stuffer.blob.size);
        }
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->tainted == old_stuffer.tainted);
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);
    }

    /* 5. Assert the stuffer data (blob) is unmodified (if applicable) */
    assert_byte_from_blob_matches(&stuffer->blob, &old_byte);

    /* 6. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
