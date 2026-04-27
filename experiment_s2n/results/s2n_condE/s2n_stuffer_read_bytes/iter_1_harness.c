#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_read_bytes_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *out = cbmc_allocate_s2n_blob();
    uint32_t n = nondet_uint32_t();

    // Assume stuffer and out are valid
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(s2n_result_is_ok(s2n_blob_validate(out)));

    // Assume out has enough space
    __CPROVER_assume(out->size >= n);

    // Save bytes from stuffer for immutability checks
    struct store_byte_from_buffer old_stuffer_bytes;
    save_byte_from_blob(&stuffer->blob, &old_stuffer_bytes);

    // Save old stuffer state
    struct s2n_stuffer old_stuffer = *stuffer;

    // Call the function
    int result = s2n_stuffer_read_bytes(stuffer, out->data, n);

    // Check return value and postconditions
    if (result == S2N_SUCCESS) {
        // Check that the correct number of bytes were read
        assert(stuffer->read_cursor == old_stuffer.read_cursor + n);
        // Check that other fields remain unchanged
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        // Check that the stuffer is still valid
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
        // Check that the output blob has the correct data
        for (uint32_t i = 0; i < n; i++) {
            assert(out->data[i] == old_stuffer.blob.data[old_stuffer.read_cursor + i]);
        }
    } else {
        // On failure, stuffer should remain unchanged
        assert(*stuffer == old_stuffer);
        // Check that the stuffer is still valid
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    }

    // Check that the blob data has not been modified
    assert_byte_from_blob_matches(&stuffer->blob, &old_stuffer_bytes);
}
