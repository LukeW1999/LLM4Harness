#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_is_consumed_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    struct s2n_blob *old_blob = &stuffer->blob;

    // Save bytes from the blob for immutability checks
    struct store_byte_from_buffer old_blob_data;
    save_byte_from_blob(old_blob, &old_blob_data);

    // Assume stuffer is valid
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    // Capture old values for unchanged fields
    uint32_t old_read_cursor = stuffer->read_cursor;
    uint32_t old_write_cursor = stuffer->write_cursor;
    uint32_t old_high_water_mark = stuffer->high_water_mark;
    bool old_alloced = stuffer->alloced;
    bool old_growable = stuffer->growable;
    bool old_tainted = stuffer->tainted;

    // Call the function
    bool result = s2n_stuffer_is_consumed(stuffer);

    // Assert immutability of blob data
    assert_byte_from_blob_matches(old_blob, &old_blob_data);

    // Assert immutability of unchanged fields
    assert(stuffer->blob.data == old_blob->data);
    assert(stuffer->blob.size == old_blob->size);
    assert(stuffer->blob.allocated == old_blob->allocated);
    assert(stuffer->blob.growable == old_blob->growable);
    assert(stuffer->read_cursor == old_read_cursor);
    assert(stuffer->write_cursor == old_write_cursor);
    assert(stuffer->high_water_mark == old_high_water_mark);
    assert(stuffer->alloced == old_alloced);
    assert(stuffer->growable == old_growable);
    assert(stuffer->tainted == old_tainted);

    // Assert validity invariant
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
