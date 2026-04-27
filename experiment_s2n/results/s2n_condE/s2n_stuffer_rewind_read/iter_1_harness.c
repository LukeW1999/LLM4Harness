#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_rewind_read_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint32_t size = nondet_uint32_t();

    // Save old stuffer state for later comparison
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_blob_data;
    save_byte_from_blob(&stuffer->blob, &old_blob_data);

    // Assume stuffer is valid initially
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Assume size is within valid range
    __CPROVER_assume(size <= stuffer->read_cursor);

    int result = s2n_stuffer_rewind_read(stuffer, size);

    if (result == S2N_SUCCESS) {
        // Check that read_cursor is updated correctly
        assert(stuffer->read_cursor == old_stuffer.read_cursor - size);

        // Check that other fields remain unchanged
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);

        // Check blob data immutability
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);

        // Check stuffer validity
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        // Check that no fields are changed on failure
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);

        // Check blob data immutability
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);

        // Check stuffer validity
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    }
}
