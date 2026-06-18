#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* Bounding macro – adjust as needed for the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

/* s2n data structures (as defined in the library) */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned growable : 1;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced : 1;
    unsigned int growable : 1;
    unsigned int tainted : 1;
};

/* Function under test – prototype only */
int s2n_stuffer_write_bytes(struct s2n_stuffer *stuffer,
                            const uint8_t *data,
                            const uint32_t size);

/* Helper to bound the internal blob state */
static void assume_valid_stuffer(const struct s2n_stuffer *s)
{
    /* cursors must be within the blob */
    __CPROVER_assume(s->read_cursor <= s->write_cursor);
    __CPROVER_assume(s->write_cursor <= s->blob.size);
    __CPROVER_assume(s->high_water_mark <= s->blob.size);

    /* allocated must be at least size when allocated > 0 */
    __CPROVER_assume(s->blob.allocated == 0 || s->blob.allocated >= s->blob.size);
}

/* Harness */
void s2n_stuffer_write_bytes_harness(void)
{
    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;

    /* blob data allocation */
    uint32_t blob_buf_len = nondet_uint32_t();
    __CPROVER_assume(blob_buf_len <= MAX_BUFFER_SIZE);
    stuffer.blob.data = (uint8_t *)malloc(blob_buf_len);
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= blob_buf_len);
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated == 0 || stuffer.blob.allocated >= stuffer.blob.size);
    stuffer.blob.growable = nondet_bool();

    /* cursors and flags */
    stuffer.read_cursor   = nondet_uint32_t();
    stuffer.write_cursor  = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced       = nondet_bool();
    stuffer.growable      = nondet_bool();
    stuffer.tainted       = nondet_bool();

    assume_valid_stuffer(&stuffer);

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Prepare input buffer */
    uint32_t in_len = nondet_uint32_t();
    __CPROVER_assume(in_len <= MAX_BUFFER_SIZE);
    uint8_t *in = (uint8_t *)malloc(in_len);
    /* The content of *in is irrelevant for the proof – it may be nondet */
    (void)in; /* silence unused warning */

    /* 4. Call the function under test */
    int result = s2n_stuffer_write_bytes(&stuffer, in, in_len);

    /* 5. Post‑condition checks */

    if (result == 0) { /* S2N_SUCCESS */
        /* write_cursor must advance by the number of bytes written */
        assert(stuffer.write_cursor == old.write_cursor + in_len);

        /* high_water_mark is the maximum of the old high_water_mark and the new write_cursor */
        uint32_t expected_hwm = old.high_water_mark;
        if (stuffer.write_cursor > expected_hwm) {
            expected_hwm = stuffer.write_cursor;
        }
        assert(stuffer.high_water_mark == expected_hwm);
    } else {
        /* On failure the stuffer must remain unchanged */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
    }

    /* 6. Fields that must never change */
    assert(stuffer.blob.data == old.blob.data);
    assert(stuffer.blob.size == old.blob.size);
    assert(stuffer.blob.allocated == old.blob.allocated);
    assert(stuffer.blob.growable == old.blob.growable);
    assert(stuffer.read_cursor == old.read_cursor);
    assert(stuffer.alloced == old.alloced);
    assert(stuffer.growable == old.growable);
    assert(stuffer.tainted == old.tainted);

    /* 7. Basic validity invariant after the call */
    assume_valid_stuffer(&stuffer);
}
