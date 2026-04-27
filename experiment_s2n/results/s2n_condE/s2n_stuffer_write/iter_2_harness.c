#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_write_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *in = cbmc_allocate_s2n_blob();

    // Save old values for immutability checks
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer data_storage;
    save_byte_from_blob(in, &data_storage);

    // Assume preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(s2n_result_is_ok(s2n_blob_validate(in)));
    __CPROVER_assume(in->size <= s2n_stuffer_space_remaining(stuffer));

    int result = s2n_stuffer_write(stuffer, in);

    // Postconditions
    if (result == S2N_SUCCESS) {
        // Changed fields
        assert(stuffer->write_cursor == old_stuffer.write_cursor + in->size);
        // High water mark can only increase or stay the same
        assert(stuffer->high_water_mark >= old_stuffer.high_water_mark);
        // Unchanged fields
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        // Immutability of input blob
        assert_byte_from_blob_matches(in, &data_storage);
        // Validity invariant
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        // Unchanged fields on failure
        assert(*stuffer == old_stuffer);
        // Immutability of input blob
        assert_byte_from_blob_matches(in, &data_storage);
    }
}
