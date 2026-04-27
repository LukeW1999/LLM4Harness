#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_free_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct store_byte_from_buffer old_blob_data;

    // Save the original blob data for immutability checks
    save_byte_from_blob(&stuffer->blob, &old_blob_data);

    // Assume the stuffer is initially valid
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Capture the original state of the stuffer
    struct s2n_stuffer old_stuffer = *stuffer;

    int result = s2n_stuffer_free(stuffer);

    if (result == S2N_SUCCESS) {
        // On success, the stuffer should be reset to all zeros
        assert(stuffer->read_cursor == 0);
        assert(stuffer->write_cursor == 0);
        assert(stuffer->high_water_mark == 0);
        assert(stuffer->alloced == 0);
        assert(stuffer->growable == 0);
        assert(stuffer->tainted == 0);

        // Blob should also be reset to all zeros
        assert(stuffer->blob.data == NULL);
        assert(stuffer->blob.size == 0);
        assert(stuffer->blob.allocated == 0);
        assert(stuffer->blob.growable == 0);

        // Validate the stuffer after freeing
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        // On failure, the stuffer should remain unchanged
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);

        // Blob should also remain unchanged
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);

        // Check that the blob data has not been modified
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);

        // Validate the stuffer remains valid after failed free
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    }
}
