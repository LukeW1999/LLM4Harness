#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_read_uint8_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint8_t u;
    struct s2n_stuffer old_stuffer = *stuffer;

    // Assume the stuffer and its blob are valid
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(s2n_result_is_ok(s2n_blob_validate(&stuffer->blob)));

    // Assume there is enough data available to read a uint8_t
    __CPROVER_assume(s2n_stuffer_data_available(stuffer) >= sizeof(uint8_t));

    // Save a byte from the blob for later immutability check
    struct store_byte_from_buffer saved_byte;
    save_byte_from_blob(&stuffer->blob, &saved_byte);

    int result = s2n_stuffer_read_uint8(stuffer, &u);

    if (result == S2N_SUCCESS) {
        // Assert the stuffer is still valid after reading
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

        // Assert the read_cursor has moved correctly
        assert(stuffer->read_cursor == old_stuffer.read_cursor + sizeof(uint8_t));

        // Assert the write_cursor remains unchanged
        assert(stuffer->write_cursor == old_stuffer.write_cursor);

        // Assert other fields remain unchanged
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);

        // Assert the byte from the blob matches the saved byte
        assert_byte_from_blob_matches(&stuffer->blob, &saved_byte);
    } else {
        // On failure, assert the stuffer remains unchanged
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer->alloced == old_stuffer.alloced);
        assert(stuffer->growable == old_stuffer.growable);
        assert(stuffer->tainted == old_stuffer.tainted);
    }
}
