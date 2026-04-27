#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_read_uint32_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct store_byte_from_buffer old_data;
    uint32_t u;

    /* Assume stuffer and blob are valid */
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    __CPROVER_assume(stuffer->blob.data != NULL);
    __CPROVER_assume(stuffer->blob.size >= sizeof(uint32_t));
    __CPROVER_assume(stuffer->write_cursor - stuffer->read_cursor >= sizeof(uint32_t));

    /* Save the old data for immutability checks */
    save_byte_from_buffer(stuffer->blob.data, stuffer->blob.size, &old_data);

    /* Store old values for postcondition checks */
    struct s2n_stuffer old_stuffer = *stuffer;

    int result = s2n_stuffer_read_uint32(stuffer, &u);

    /* Check postconditions based on the result */
    if (result == S2N_SUCCESS) {
        /* Check that the read_cursor has moved forward by 4 bytes */
        assert(stuffer->read_cursor == old_stuffer.read_cursor + sizeof(uint32_t));
        /* Check that the write_cursor remains unchanged */
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        /* Check that the blob data remains unchanged except for the read part */
        assert_bytes_match(stuffer->blob.data, old_stuffer.blob.data, old_stuffer.read_cursor);
        assert_bytes_match(stuffer->blob.data + old_stuffer.read_cursor + sizeof(uint32_t), old_stuffer.blob.data + old_stuffer.read_cursor + sizeof(uint32_t), stuffer->blob.size - (old_stuffer.read_cursor + sizeof(uint32_t)));
        /* Check that the blob size remains unchanged */
        assert(stuffer->blob.size == old_stuffer.blob.size);
        /* Check that the stuffer remains valid */
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        /* Check that the stuffer remains unchanged on failure */
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
        assert(stuffer->write_cursor == old_stuffer.write_cursor);
        assert(stuffer->blob.data == old_stuffer.blob.data);
        assert(stuffer->blob.size == old_stuffer.blob.size);
        /* Check that the stuffer remains valid */
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    }
}
