#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_resize_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    /* Precondition: stuffer is not tainted */
    __CPROVER_assume(!stuffer->tainted);

    /* Precondition: stuffer is growable */
    __CPROVER_assume(stuffer->growable);

    /* Non-deterministic size */
    uint32_t size = nondet_uint32_t();

    /* Additional precondition: size should not exceed maximum allowed value */
    __CPROVER_assume(size <= UINT32_MAX / 2);

    /* Additional precondition: size should not be less than the current write_cursor */
    __CPROVER_assume(size >= stuffer->write_cursor);

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_stuffer = *stuffer;
    struct store_byte_from_buffer old_byte;
    save_byte_from_blob(&stuffer->blob, &old_byte);

    /* 3. Call function under test */
    int result = s2n_stuffer_resize(stuffer, size);

    /* 4. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        if (size == old_stuffer.blob.size) {
            /* No change expected */
            assert(*stuffer == old_stuffer);
        } else if (size == 0) {
            /* Blob should be wiped and freed */
            assert(stuffer->blob.data == NULL);
            assert(stuffer->blob.size == 0);
            assert(stuffer->read_cursor == 0);
            assert(stuffer->write_cursor == 0);
            assert(stuffer->high_water_mark == 0);
        } else if (size < old_stuffer.blob.size) {
            /* Blob size reduced, cursors and high water mark adjusted */
            assert(stuffer->blob.size == size);
            assert(stuffer->read_cursor <= size);
            assert(stuffer->write_cursor <= size);
            assert(stuffer->high_water_mark <= size);
        } else {
            /* Blob size increased, data pointer may change but other fields should be consistent */
            assert(stuffer->blob.size == size);
            assert(stuffer->read_cursor == old_stuffer.read_cursor);
            assert(stuffer->write_cursor == old_stuffer.write_cursor);
            assert(stuffer->high_water_mark == old_stuffer.high_water_mark);
        }
    } else {
        /* On failure, stuffer should remain unchanged */
        assert(*stuffer == old_stuffer);
    }

    /* 5. Assert the stuffer data (blob) is unmodified (if applicable) */
    if (size != 0 && size >= old_stuffer.blob.size) {
        assert_byte_from_blob_matches(&stuffer->blob, &old_byte);
    }

    /* 6. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
