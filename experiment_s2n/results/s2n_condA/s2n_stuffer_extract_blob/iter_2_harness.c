#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_extract_blob_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    struct s2n_blob *out = cbmc_allocate_s2n_blob();
    __CPROVER_assume(out != NULL);

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 3. Call function under test */
    int result = s2n_stuffer_extract_blob(stuffer, out);

    /* 4. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        assert(out->size == s2n_stuffer_data_available(&old_stuffer));
        assert(out->data != NULL);
        if (s2n_stuffer_data_available(&old_stuffer) > 0) {
            assert_byte_from_buffer_matches(stuffer->blob.data + stuffer->read_cursor, &old_byte);
        }
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
    }

    /* 5. Assert the stuffer data (blob) is unmodified (if applicable) */
    assert_byte_from_blob_matches(&stuffer->blob, &old_byte);

    /* 6. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    assert(s2n_result_is_ok(s2n_blob_validate(out)));

    /* Ensure at least one assert is present */
    assert(result == S2N_SUCCESS || result == S2N_FAILURE);
}
