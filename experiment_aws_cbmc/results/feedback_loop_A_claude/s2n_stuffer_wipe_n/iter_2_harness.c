#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

/* s2n types needed */
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

/* Declare the function under test */
int s2n_stuffer_wipe_n(struct s2n_stuffer *stuffer, const uint32_t n);

/* Validity predicate for s2n_stuffer */
bool s2n_stuffer_is_valid(const struct s2n_stuffer *stuffer) {
    if (stuffer == NULL) return false;
    if (stuffer->read_cursor > stuffer->write_cursor) return false;
    if (stuffer->write_cursor > stuffer->blob.size) return false;
    if (stuffer->high_water_mark < stuffer->write_cursor) return false;
    if (stuffer->blob.size > 0 && stuffer->blob.data == NULL) return false;
    return true;
}

void s2n_stuffer_wipe_n_harness(void) {
    /* 1. Declare and set up stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic blob size, bounded for tractability */
    uint32_t blob_size;
    __CPROVER_assume(blob_size <= 64);

    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= blob_size);
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set up cursors satisfying validity */
    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= blob_size);

    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);

    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark >= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Verify precondition holds */
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;
    uint8_t *old_data = stuffer.blob.data;

    /* 3. Non-deterministic size parameter */
    uint32_t n = nondet_uint32_t();

    /* 4. Call function under test */
    int result = s2n_stuffer_wipe_n(&stuffer, n);

    /* 5. Assert postconditions */
    if (result == 0) { /* S2N_SUCCESS */
        /* Compute expected wipe_size */
        uint32_t wipe_size = (n < old.write_cursor) ? n : old.write_cursor;
        uint32_t expected_write_cursor = old.write_cursor - wipe_size;
        uint32_t expected_read_cursor = (old.read_cursor < expected_write_cursor)
                                         ? old.read_cursor
                                         : expected_write_cursor;

        /* Changed fields */
        assert(stuffer.write_cursor == expected_write_cursor);
        assert(stuffer.read_cursor == expected_read_cursor);

        /* Unchanged fields */
        assert(stuffer.blob.data == old_data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);

        /* Validity invariant */
        assert(s2n_stuffer_is_valid(&stuffer));
    } else {
        /* On failure, stuffer should be unchanged */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.blob.data == old_data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.high_water_mark == old.high_water_mark);
    }

    return 0;
}
