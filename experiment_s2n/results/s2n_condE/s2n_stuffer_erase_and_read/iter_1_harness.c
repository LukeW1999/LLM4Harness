#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_erase_and_read_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *out = cbmc_allocate_s2n_blob();

    // Assume the stuffer and out blob are valid
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(s2n_result_is_ok(s2n_blob_validate(out)));

    // Assume out blob has enough space
    __CPROVER_assume(out->size <= s2n_stuffer_data_available(stuffer));

    // Save bytes from stuffer for immutability checks
    struct store_byte_from_buffer old_stuffer_bytes;
    save_byte_from_array(stuffer->blob.data, stuffer->blob.size, &old_stuffer_bytes);

    // Save the original stuffer state
    struct s2n_stuffer old_stuffer = *stuffer;

    int result = s2n_stuffer_erase_and_read(stuffer, out);

    if (result == S2N_SUCCESS) {
        // Assert changed fields
        assert(stuffer->read_cursor == old_stuffer.read_cursor + out->size);

        // Assert unchanged fields
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);

        // Assert immutability of data before read_cursor
        assert_byte_from_buffer_matches(stuffer->blob.data, &old_stuffer_bytes);
    } else {
        // Assert no changes on failure
        assert(*stuffer == old_stuffer);
    }

    // Assert validity invariant
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
