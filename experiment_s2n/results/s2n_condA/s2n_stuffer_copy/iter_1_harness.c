#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/cbmc_utils.h>

void s2n_stuffer_copy_harness() {
    /* 1. Allocate and validate inputs non-deterministically */
    struct s2n_stuffer *from = cbmc_allocate_s2n_stuffer();
    struct s2n_stuffer *to = cbmc_allocate_s2n_stuffer();
    uint32_t len = nondet_uint32_t();

    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(from)));
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(to)));
    __CPROVER_assume(len <= s2n_stuffer_data_available(from));
    __CPROVER_assume(len <= s2n_stuffer_space_remaining(to));

    /* 2. Save old state BEFORE calling */
    struct s2n_stuffer old_from = *from;
    struct s2n_stuffer old_to = *to;
    struct store_byte_from_buffer old_from_byte;
    struct store_byte_from_buffer old_to_byte;
    save_byte_from_blob(&from->blob, &old_from_byte);
    save_byte_from_blob(&to->blob, &old_to_byte);

    /* 3. Call function under test */
    int result = s2n_stuffer_copy(from, to, len);

    /* 4. Assert postconditions for success path */
    if (result == S2N_SUCCESS) {
        assert(from->read_cursor == old_from.read_cursor + len);
        assert(to->write_cursor == old_to.write_cursor + len);
    } else {
        assert(from->read_cursor == old_from.read_cursor);
        assert(to->write_cursor == old_to.write_cursor);
    }

    /* 5. Assert the stuffer data (blob) is unmodified (if applicable) */
    assert_byte_from_blob_matches(&from->blob, &old_from_byte);
    assert_byte_from_blob_matches(&to->blob, &old_to_byte);

    /* 6. Assert validity invariant is preserved */
    assert(s2n_result_is_ok(s2n_stuffer_validate(from)));
    assert(s2n_result_is_ok(s2n_stuffer_validate(to)));
}
