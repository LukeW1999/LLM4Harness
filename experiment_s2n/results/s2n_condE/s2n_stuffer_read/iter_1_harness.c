#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_read_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *out = cbmc_allocate_s2n_blob();

    // Save initial state for later assertions
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer stuffer_blob_storage;
    save_byte_from_blob(&stuffer->blob, &stuffer_blob_storage);

    // Assume preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(s2n_result_is_ok(s2n_blob_validate(out)));
    __CPROVER_assume(out->data != NULL);
    __CPROVER_assume(out->size <= s2n_stuffer_data_available(stuffer));

    int result = s2n_stuffer_read(stuffer, out);

    // Postconditions
    if (result == S2N_SUCCESS) {
        assert(stuffer->read_cursor == old_stuffer.read_cursor + out->size);
        assert_byte_from_blob_matches(&stuffer->blob, &stuffer_blob_storage);
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert_byte_from_blob_matches(&stuffer->blob, &stuffer_blob_storage);
    }
}
