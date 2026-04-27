#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_write_uint32_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(stuffer->blob.data != NULL);
    __CPROVER_assume(stuffer->write_cursor <= stuffer->blob.size);

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 3. Call function under test */
    uint32_t u = nondet_uint32_t();
    int result = s2n_stuffer_write_uint32(stuffer, u);

    /* 4. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        assert(stuffer->write_cursor == old_stuffer.write_cursor + sizeof(uint32_t));
        assert(stuffer->high_water_mark >= old_stuffer.high_water_mark);
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
    }

    /* 5. Assert the stuffer data (blob) is unmodified (if applicable) */
    assert_byte_from_blob_matches(&stuffer->blob, &old_byte);

    /* 6. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
