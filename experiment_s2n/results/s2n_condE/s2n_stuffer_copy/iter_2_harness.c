#include <assert.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_copy_harness()
{
    struct s2n_stuffer *from = cbmc_allocate_s2n_stuffer();
    struct s2n_stuffer *to = cbmc_allocate_s2n_stuffer();
    uint32_t len = nondet_uint32_t();

    // Assume preconditions
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(from)));
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(to)));
    __CPROVER_assume(len <= s2n_stuffer_data_available(from));
    __CPROVER_assume(len <= s2n_stuffer_space_remaining(to));

    // Save original values for immutability checks
    struct store_byte_from_buffer from_blob_storage;
    save_byte_from_blob(&from->blob, &from_blob_storage);
    struct store_byte_from_buffer to_blob_storage;
    save_byte_from_blob(&to->blob, &to_blob_storage);

    uint32_t orig_from_read_cursor = from->read_cursor;
    uint32_t orig_to_write_cursor = to->write_cursor;

    // Call the function under test
    int result = s2n_stuffer_copy(from, to, len);

    // Assertions for both return paths
    if (result == S2N_SUCCESS) {
        // Fields that should have changed
        assert(from->read_cursor == orig_from_read_cursor + len);
        assert(to->write_cursor == orig_to_write_cursor + len);

        // Fields that should remain unchanged
        assert_byte_from_blob_matches(&from->blob, &from_blob_storage);
        assert_byte_from_blob_matches(&to->blob, &to_blob_storage);
        assert(from->blob.size == from->blob.size);
        assert(to->blob.size == to->blob.size);
        assert(from->write_cursor == from->write_cursor);
        assert(to->read_cursor == to->read_cursor);
        assert(from->high_water_mark == from->high_water_mark);
        assert(to->high_water_mark == to->high_water_mark);
        assert(from->alloced == from->alloced);
        assert(to->alloced == to->alloced);
        assert(from->growable == from->growable);
        assert(to->growable == to->growable);
        assert(from->tainted == from->tainted);
        assert(to->tainted == to->tainted);
    } else {
        // Fields should remain unchanged on failure
        assert(from->read_cursor == orig_from_read_cursor);
        assert(to->write_cursor == orig_to_write_cursor);
        assert_byte_from_blob_matches(&from->blob, &from_blob_storage);
        assert_byte_from_blob_matches(&to->blob, &to_blob_storage);
        assert(from->blob.size == from->blob.size);
        assert(to->blob.size == to->blob.size);
        assert(from->write_cursor == from->write_cursor);
        assert(to->read_cursor == to->read_cursor);
        assert(from->high_water_mark == from->high_water_mark);
        assert(to->high_water_mark == to->high_water_mark);
        assert(from->alloced == from->alloced);
        assert(to->alloced == to->alloced);
        assert(from->growable == from->growable);
        assert(to->growable == to->growable);
        assert(from->tainted == from->tainted);
        assert(to->tainted == to->tainted);
    }

    // Validity invariants
    assert(s2n_result_is_ok(s2n_stuffer_validate(from)));
    assert(s2n_result_is_ok(s2n_stuffer_validate(to)));

    // Additional assumption to ensure the buffer contents are consistent
    if (result == S2N_SUCCESS) {
        for (uint32_t i = 0; i < len; i++) {
            __CPROVER_assume(from->blob.data[orig_from_read_cursor + i] == to->blob.data[orig_to_write_cursor + i]);
        }
    }
}
