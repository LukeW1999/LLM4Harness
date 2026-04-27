#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_extract_blob_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *out = cbmc_allocate_s2n_blob();

    // Save initial state for immutability checks
    struct store_byte_from_buffer original_stuffer_blob_data;
    save_byte_from_blob(&stuffer->blob, &original_stuffer_blob_data);

    // Assume preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(out != NULL);

    // Store old values for postcondition checks
    struct s2n_stuffer old_stuffer = *stuffer;

    // Call the function under test
    int result = s2n_stuffer_extract_blob(stuffer, out);

    // Postconditions
    if (result == S2N_SUCCESS) {
        // Check that the output blob is valid
        assert(s2n_result_is_ok(s2n_blob_validate(out)));

        // Check that the data was copied correctly
        if (s2n_stuffer_data_available(&old_stuffer) > 0) {
            assert_byte_from_buffer_matches(out->data, &original_stuffer_blob_data);
        }

        // Check that the stuffer's read_cursor and write_cursor are unchanged
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);

        // Check that the stuffer's blob data and size are unchanged
        assert_byte_from_blob_matches(&stuffer->blob, &original_stuffer_blob_data);
        assert(stuffer->blob.size == old_stuffer.blob.size);

        // Check that the stuffer's other fields are unchanged
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
    } else {
        // On failure, the stuffer should remain unchanged
        assert(memcmp(stuffer, &old_stuffer, sizeof(struct s2n_stuffer)) == 0);
    }

    // Check that the stuffer remains valid after the call
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
