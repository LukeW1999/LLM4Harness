#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_resize_if_empty_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint32_t size = nondet_uint32_t();

    // Save the old state of the stuffer for later comparison
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_blob_data;
    save_byte_from_blob(&stuffer->blob, &old_blob_data);

    // Assume the preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    int result = s2n_stuffer_resize_if_empty(stuffer, size);

    // Check the postconditions
    if (result == S2N_SUCCESS) {
        // Validate the stuffer after successful resizing
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

        // If the stuffer was empty and needed resizing, check the new blob data
        if (old_stuffer.blob.data == NULL) {
            assert(stuffer->blob.data != NULL);
            assert(stuffer->blob.size == size);
            assert(stuffer->growable == old_stuffer.growable);
            assert(stuffer->tainted == old_stuffer.tainted);
            assert(stuffer->alloced == 1); // Should be alloced after resizing
        } else {
            // If the stuffer was not empty, nothing should have changed
            assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
            assert(stuffer->blob.data == old_stuffer.blob.data);
            assert(stuffer->blob.size == old_stuffer.blob.size);
        }

        // Cursors and high_water_mark should remain unchanged
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
    } else {
        // On failure, the stuffer should remain unchanged
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
        assert(memcmp(stuffer, &old_stuffer, sizeof(struct s2n_stuffer)) == 0);
    }
}
