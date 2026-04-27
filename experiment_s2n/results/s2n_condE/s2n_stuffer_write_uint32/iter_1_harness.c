#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_write_uint32_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint32_t u = nondet_uint32_t();

    // Assume the stuffer is initially valid
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Save the old state of the stuffer for later comparison
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_data;
    save_byte_from_blob(&stuffer->blob, &old_data);

    int result = s2n_stuffer_write_uint32(stuffer, u);

    if (result == S2N_SUCCESS) {
        // Assert that the stuffer remains valid after the operation
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

        // Assert that the write_cursor has increased by the size of the written data
        assert(stuffer->write_cursor == old_stuffer.write_cursor + sizeof(uint32_t));

        // Assert that other fields remain unchanged
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);

        // Assert that the blob data has not changed elsewhere
        assert_byte_from_blob_matches(&stuffer->blob, &old_data);
    } else {
        // Assert that the stuffer remains valid even on failure
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

        // Assert that no fields have changed
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

        // Assert that the blob data has not changed
        assert_byte_from_blob_matches(&stuffer->blob, &old_data);
    }
}
