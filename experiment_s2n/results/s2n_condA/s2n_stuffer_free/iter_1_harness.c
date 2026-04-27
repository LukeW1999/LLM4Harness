#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_free_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 3. Call function under test */
    int result = s2n_stuffer_free(stuffer);

    /* 4. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        assert(stuffer->read_cursor == 0);
        assert(stuffer->write_cursor == 0);
        assert(stuffer->high_water_mark == 0);
        assert(stuffer->alloced == 0);
        assert(stuffer->growable == 0);
        assert(stuffer->tainted == 0);
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
    }

    /* 5. Assert the stuffer data (blob) is unmodified (if applicable) */
    if (!stuffer->alloced) {
        assert_byte_from_blob_matches(&stuffer->blob, &old_byte);
    }

    /* 6. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
