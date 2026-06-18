#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stdint.h>

/* Forward declaration of the function under test */
void *s2n_stuffer_raw_write(struct s2n_stuffer *stuffer, const uint32_t data_len);

/* Stub for s2n_stuffer_skip_write – the real implementation is not needed for the harness.
 * It returns 0 on success (enough space) and -1 on failure (insufficient space).
 * It also updates the write_cursor and high_water_mark on success.
 */
int s2n_stuffer_skip_write(struct s2n_stuffer *stuffer, uint32_t data_len)
{
    if (stuffer->write_cursor + data_len > stuffer->blob.size) {
        return -1;
    }
    stuffer->write_cursor += data_len;
    if (stuffer->write_cursor > stuffer->high_water_mark) {
        stuffer->high_water_mark = stuffer->write_cursor;
    }
    return 0;
}

void s2n_stuffer_raw_write_harness(void)
{
    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;

    /* Allocate a bounded buffer for the blob on the stack */
    uint8_t buffer[MAX_BUFFER_SIZE];
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= MAX_BUFFER_SIZE);
    stuffer.blob.data = buffer;
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = blob_size;
    stuffer.blob.growable = nondet_bool();

    /* Initialise the other stuffer fields */
    stuffer.read_cursor      = nondet_uint32_t();
    stuffer.write_cursor     = nondet_uint32_t();
    stuffer.high_water_mark  = nondet_uint32_t();
    stuffer.alloced          = nondet_bool();
    stuffer.growable         = nondet_bool();
    stuffer.tainted          = nondet_bool();

    /* Impose basic invariants that must hold before the call */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Choose a nondeterministic data_len */
    uint32_t data_len = nondet_uint32_t();

    /* 4. Call the function under test */
    void *result = s2n_stuffer_raw_write(&stuffer, data_len);

    /* 5. Post‑condition checks */
    if (result != NULL) {
        /* Success path – the write should have succeeded */
        assert(stuffer.tainted == true);
        assert(stuffer.write_cursor == old.write_cursor + data_len);
        assert(stuffer.high_water_mark >= old.high_water_mark);
        assert(stuffer.high_water_mark == stuffer.write_cursor ||
               stuffer.high_water_mark == old.high_water_mark);
        assert(result == (void *)(stuffer.blob.data + old.write_cursor));
    } else {
        /* Failure path – the stuffer must be unchanged */
        assert(stuffer.tainted == old.tainted);
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
    }

    /* 6. Fields that must never change regardless of outcome */
    assert(stuffer.blob.data == old.blob.data);
    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);
    assert(stuffer.read_cursor == old.read_cursor);
    assert(stuffer.alloced == old.alloced);
    assert(stuffer.growable == old.growable);

    /* 7. Basic validity invariants after the call */
    assert(stuffer.read_cursor <= stuffer.write_cursor);
    assert(stuffer.write_cursor <= stuffer.blob.size);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
}
