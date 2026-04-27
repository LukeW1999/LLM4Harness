#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_write_uint8_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    /* Ensure there is enough space to write a uint8_t */
    __CPROVER_assume(stuffer->blob.size >= stuffer->write_cursor + sizeof(uint8_t));

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 3. Call function under test */
    uint8_t u = nondet_uint8_t();
    int result = s2n_stuffer_write_uint8(stuffer, u);

    /* 4. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        assert(stuffer->write_cursor == old_stuffer.write_cursor + sizeof(uint8_t));
        assert(stuffer->high_water_mark == (old_stuffer.high_water_mark < stuffer->write_cursor ? stuffer->write_cursor : old_stuffer.high_water_mark));
        /* Assert that the byte written is correct */
        assert(stuffer->blob.data[old_stuffer.write_cursor] == u);
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
    }

    /* 5. Assert the stuffer data (blob) is unmodified except at the write_cursor */
    if (result != S2N_SUCCESS || stuffer->write_cursor == old_stuffer.write_cursor) {
        assert_byte_from_blob_matches(&stuffer->blob, &old_byte);
    }

    /* 6. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
