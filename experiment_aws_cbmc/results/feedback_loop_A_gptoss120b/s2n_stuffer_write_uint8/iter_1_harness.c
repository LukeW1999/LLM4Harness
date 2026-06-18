#include <s2n.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Harness for s2n_stuffer_write_uint8 */
void s2n_stuffer_write_uint8_harness() {
    /* 1. Declare the stuffer and assume it is valid */
    struct s2n_stuffer stuffer;
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Nondeterministic byte to write */
    uint8_t u = nondet_uint8_t();

    /* 4. Call the function under test */
    int result = s2n_stuffer_write_uint8(&stuffer, u);

    /* 5. Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* Success: write cursor advances by one byte */
        assert(stuffer.write_cursor == old.write_cursor + 1);

        /* Unchanged fields */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.capacity == old.blob.capacity);
        assert(stuffer.growable == old.growable);
        assert(stuffer.high_water_mark == (old.high_water_mark > old.write_cursor ? old.high_water_mark : stuffer.write_cursor));
    } else {
        /* Failure: the stuffer must remain unchanged */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.capacity == old.blob.capacity);
        assert(stuffer.growable == old.growable);
        assert(stuffer.high_water_mark == old.high_water_mark);
    }

    /* 6. Invariant: the stuffer is still valid */
    assert(s2n_stuffer_is_valid(&stuffer));
}
