#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_write_uint8_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint8_t u = nondet_uint8_t();

    // Save old values for immutability checks
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer data_storage;
    save_byte_from_blob(&stuffer->blob, &data_storage);

    // Assume stuffer is valid before the call
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Assume there is enough space in the stuffer
    __CPROVER_assume(s2n_stuffer_space_remaining(stuffer) > 0);

    // Call the function
    int result = s2n_stuffer_write_uint8(stuffer, u);

    // Check postconditions based on the result
    if (result == S2N_SUCCESS) {
        // Check that the write_cursor has increased by 1
        assert(stuffer->write_cursor == old_stuffer.write_cursor + 1);
        // Check that the high_water_mark is updated if necessary
        assert(stuffer->high_water_mark >= old_stuffer.high_water_mark);
        assert(stuffer->high_water_mark <= old_stuffer.high_water_mark + 1);
        // Check that other fields remain unchanged
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);
        // Check that the blob data remains unchanged except at the write_cursor position
        if (old_stuffer.write_cursor < old_stuffer.blob.size) {
            assert(*(stuffer->blob.data + old_stuffer.write_cursor) == u);
        }
        assert_byte_from_blob_matches(&stuffer->blob, &data_storage);
    } else {
        // On failure, check that no fields have changed
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
        // Check that the blob data remains unchanged
        assert_byte_from_blob_matches(&stuffer->blob, &data_storage);
    }

    // Check the validity of the stuffer after the call
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
