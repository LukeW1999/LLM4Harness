#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_skip_read_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *blob = cbmc_allocate_s2n_blob();
    stuffer->blob = *blob;

    // Save initial state for later comparison
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_data;
    save_byte_from_blob(&stuffer->blob, &old_data);

    // Non-deterministic input
    uint32_t n = nondet_uint32_t();

    // Precondition: stuffer must be valid
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Precondition: there must be enough data available to skip
    __CPROVER_assume(s2n_stuffer_data_available(stuffer) >= n);

    int result = s2n_stuffer_skip_read(stuffer, n);

    // Postcondition: check the result and the state of the stuffer
    if (result == S2N_SUCCESS) {
        // Fields that should have changed
        assert(stuffer->read_cursor == old_stuffer.read_cursor + n);

        // Fields that should remain unchanged
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);

        // Immutability of blob data
        assert_byte_from_blob_matches(&stuffer->blob, &old_data);

        // Validity invariant
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        // Fields should remain unchanged on failure
        assert(*stuffer == old_stuffer);

        // Immutability of blob data
        assert_byte_from_blob_matches(&stuffer->blob, &old_data);
    }
}
