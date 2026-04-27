#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_skip_write_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint32_t n = nondet_uint32_t();

    // Save old values for immutability checks
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_blob_data;
    save_byte_from_blob(&stuffer->blob, &old_blob_data);

    // Assume preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(n <= s2n_stuffer_space_remaining(stuffer));

    int result = s2n_stuffer_skip_write(stuffer, n);

    // Postconditions
    if (result == S2N_SUCCESS) {
        assert(stuffer->write_cursor == old_stuffer.write_cursor + n);
        assert(stuffer->high_water_mark == MAX(old_stuffer.high_water_mark, old_stuffer.write_cursor + n));
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
    }
}
