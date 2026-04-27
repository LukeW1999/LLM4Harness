#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_write_bytes_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *blob = cbmc_allocate_s2n_blob();
    stuffer->blob = *blob;

    // Initialize stuffer with valid state
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Non-deterministic input data
    uint32_t size = nondet_uint32_t();
    __CPROVER_assume(size <= stuffer->blob.size - stuffer->write_cursor); // Ensure there is enough space
    const uint8_t *data = malloc(size);

    // Save initial state for immutability checks
    struct store_byte_from_buffer old_data;
    save_byte_from_buffer(data, size, &old_data);

    // Store old stuffer values for postcondition checks
    struct s2n_stuffer old_stuffer = *stuffer;

    // Call the function under test
    int result = s2n_stuffer_write_bytes(stuffer, data, size);

    // Postconditions
    if (result == S2N_SUCCESS) {
        // Fields that should have changed
        assert(stuffer->write_cursor == old_stuffer.write_cursor + size);
        assert(stuffer->high_water_mark >= old_stuffer.high_water_mark);
        // Fields that should remain unchanged
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer->blob.growable == old_stuffer.blob.growable);
        // Immutability check
        if (size > 0) {
            assert_byte_from_buffer_matches(data, &old_data);
        }
        // Validity invariant
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        // Fields should remain unchanged on failure
        assert(*stuffer == old_stuffer);
        // Immutability check
        if (size > 0) {
            assert_byte_from_buffer_matches(data, &old_data);
        }
    }
}
