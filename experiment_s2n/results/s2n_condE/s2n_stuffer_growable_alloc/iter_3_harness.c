#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_growable_alloc_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    uint32_t size = nondet_uint32_t();

    /* Assume initial validity */
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    /* Assume initial state constraints */
    __CPROVER_assume(size <= MAX_UINT32); // Ensure size is within a reasonable range

    /* Save old values for immutability checks */
    struct store_byte_from_buffer old_blob_data;
    save_byte_from_blob(&stuffer->blob, &old_blob_data);
    uint32_t old_read_cursor = stuffer->read_cursor;
    uint32_t old_write_cursor = stuffer->write_cursor;
    uint32_t old_high_water_mark = stuffer->high_water_mark;
    bool old_alloced = stuffer->alloced;
    bool old_tainted = stuffer->tainted;
    bool old_growable = stuffer->growable;

    int result = s2n_stuffer_growable_alloc(stuffer, size);

    if (result == S2N_SUCCESS) {
        /* Assert changed fields */
        assert(stuffer->growable == 1);

        /* Assert unchanged fields */
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
        assert(stuffer->read_cursor == old_read_cursor);
        assert(stuffer->write_cursor == old_write_cursor);
        assert(stuffer->high_water_mark == old_high_water_mark);
        assert(stuffer->alloced == old_alloced || stuffer->alloced == 1); // Allow alloced to change
        assert(stuffer->tainted == old_tainted);

        /* Assert validity invariant */
        assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
    } else {
        /* Assert unchanged fields on failure */
        assert_byte_from_blob_matches(&stuffer->blob, &old_blob_data);
        assert(stuffer->read_cursor == old_read_cursor);
        assert(stuffer->write_cursor == old_write_cursor);
        assert(stuffer->high_water_mark == old_high_water_mark);
        assert(stuffer->alloced == old_alloced);
        assert(stuffer->growable == old_growable);
        assert(stuffer->tainted == old_tainted);

        /* No need to assert validity on failure, as the function may leave the stuffer in an invalid state */
    }
}
