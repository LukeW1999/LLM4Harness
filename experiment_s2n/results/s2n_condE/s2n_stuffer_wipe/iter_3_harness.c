#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_wipe_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct store_byte_from_buffer old_bytes;
    save_byte_from_blob(&stuffer->blob, &old_bytes);

    // Assume the stuffer is valid before the call
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    int result = s2n_stuffer_wipe(stuffer);

    if (result == S2N_SUCCESS) {
        // On success, the write_cursor should be reset to 0
        assert(stuffer->write_cursor == 0);
        assert(stuffer->read_cursor == 0);
        assert(stuffer->high_water_mark == 0);

        // Other fields should remain unchanged
        assert(stuffer->blob.data == old_bytes.byte);
        assert(stuffer->blob.size == old_bytes.size);
        assert(stuffer->alloced == old_bytes.alloced);
        assert(stuffer->growable == old_bytes.growable);
        assert(stuffer->tainted == old_bytes.tainted);

        // Blob data should be unchanged
        assert_byte_from_blob_matches(&stuffer->blob, &old_bytes);

        // Validate the stuffer after wiping
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        // On failure, the stuffer fields should remain unchanged
        assert(stuffer->read_cursor == old_bytes.read_cursor);
        assert(stuffer->write_cursor == old_bytes.write_cursor);
        assert(stuffer->high_water_mark == old_bytes.high_water_mark);
        assert(stuffer->blob.data == old_bytes.byte);
        assert(stuffer->blob.size == old_bytes.size);
        assert(stuffer->alloced == old_bytes.alloced);
        assert(stuffer->growable == old_bytes.growable);
        assert(stuffer->tainted == old_bytes.tainted);

        // Blob data should be unchanged
        assert_byte_from_blob_matches(&stuffer->blob, &old_bytes);
    }
}
