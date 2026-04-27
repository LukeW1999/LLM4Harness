#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_rewrite_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct store_byte_from_buffer old_data;
    save_byte_from_blob(&stuffer->blob, &old_data);

    // Assume preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Record old values for unchanged fields
    struct s2n_stuffer old_stuffer = *stuffer;

    int result = s2n_stuffer_rewrite(stuffer);

    // Postconditions
    if (result == S2N_SUCCESS) {
        // Changed fields
        assert(stuffer->write_cursor == 0);
        assert(stuffer->read_cursor == 0);

        // Unchanged fields
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);

        // Immutability checks
        assert_byte_from_blob_matches(&stuffer->blob, &old_data);

        // Validity invariant
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        // Fields unchanged on failure
        assert(*stuffer == old_stuffer);

        // Immutability checks
        assert_byte_from_blob_matches(&stuffer->blob, &old_data);
    }
}
