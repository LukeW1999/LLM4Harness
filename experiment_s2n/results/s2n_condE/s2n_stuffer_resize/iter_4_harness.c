#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_resize_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *blob = cbmc_allocate_s2n_blob();
    stuffer->blob = *blob;

    // Assume preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(!stuffer->tainted);
    __CPROVER_assume(stuffer->growable);
    __CPROVER_assume(stuffer->blob.size > 0); // Ensure blob size is not zero initially

    // Save old values for immutability checks
    struct store_byte_from_buffer old_blob_data;
    save_byte_from_blob(&stuffer->blob, &old_blob_data);
    uint32_t old_read_cursor = stuffer->read_cursor;
    uint32_t old_write_cursor = stuffer->write_cursor;
    uint32_t old_high_water_mark = stuffer->high_water_mark;
    uint32_t old_blob_size = stuffer->blob.size;

    // Non-deterministic size
    uint32_t size = nondet_uint32_t();
    __CPROVER_assume(size <= UINT32_MAX); // Ensure size is within bounds

    // Call the function
    int result = s2n_stuffer_resize(stuffer, size);

    // Postconditions
    if (result == S2N_SUCCESS) {
        // Validity invariant
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

        // Changed fields
        if (size == stuffer->blob.size) {
            // No changes expected
            assert(stuffer->read_cursor == old_read_cursor);
            assert(stuffer->write_cursor == old_write_cursor);
            assert(stuffer->high_water_mark == old_high_water_mark);
            assert(stuffer->blob.size == old_blob_size);
        } else if (size == 0) {
            assert(stuffer->read_cursor == 0);
            assert(stuffer->write_cursor == 0);
            assert(stuffer->high_water_mark == 0);
            assert(stuffer->blob.size == 0);
        } else if (size < stuffer->blob.size) {
            assert(stuffer->read_cursor <= size);
            assert(stuffer->write_cursor <= size);
            assert(stuffer->high_water_mark <= size);
            assert(stuffer->blob.size == size);
        } else {
            assert(stuffer->blob.size == size);
        }

        // Unchanged fields
        if (stuffer->blob.data != NULL && old_blob_size > 0) {
            assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
        }
    } else {
        // On failure, fields should remain unchanged
        assert(stuffer->read_cursor == old_read_cursor);
        assert(stuffer->write_cursor == old_write_cursor);
        assert(stuffer->high_water_mark == old_high_water_mark);
        assert(stuffer->blob.size == old_blob_size);
        if (stuffer->blob.data != NULL && old_blob_size > 0) {
            assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
        }
    }
}
