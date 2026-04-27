#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_wipe_n_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    uint32_t n = nondet_uint32_t();

    /* 2. Add assumptions to ensure n is within a valid range */
    __CPROVER_assume(n <= stuffer->write_cursor);

    /* 3. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 4. Call function under test */
    int result = s2n_stuffer_wipe_n(stuffer, n);

    /* 5. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        uint32_t wipe_size = MIN(n, old_stuffer.write_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor - wipe_size);
        assert(stuffer->read_cursor == MIN(old_stuffer.read_cursor, old_stuffer.write_cursor - wipe_size));
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
    }

    /* 6. Assert the stuffer data (blob) is unmodified (if applicable) */
    if (n == 0) {
        assert_byte_from_blob_matches(&stuffer->blob, &old_byte);
    }

    /* 7. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
