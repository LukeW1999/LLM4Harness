#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* Minimal s2n struct definitions needed for the harness */
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

/* Forward declarations for s2n functions used internally */
int s2n_stuffer_skip_read(struct s2n_stuffer *stuffer, uint32_t data_len);
void *s2n_stuffer_raw_read(struct s2n_stuffer *stuffer, uint32_t data_len);

void s2n_stuffer_raw_read_harness(void) {
    /* 1. Declare and initialize the stuffer non-deterministically */
    struct s2n_stuffer stuffer;

    /* Initialize blob fields non-deterministically */
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Allocate data buffer non-deterministically (may be NULL) */
    if (nondet_bool()) {
        /* Bound the buffer size to keep verification tractable */
        uint32_t buf_size = stuffer.blob.size;
        if (buf_size > 0) {
            stuffer.blob.data = malloc(buf_size);
            __CPROVER_assume(stuffer.blob.data != NULL);
        } else {
            stuffer.blob.data = NULL;
        }
    } else {
        stuffer.blob.data = NULL;
    }

    /* Initialize cursor fields non-deterministically */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Preconditions: cursors must be within blob size */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* If data is non-NULL, blob.size must be consistent */
    if (stuffer.blob.data != NULL) {
        __CPROVER_assume(stuffer.blob.size > 0);
    }

    /* data_len to read */
    uint32_t data_len = nondet_uint32_t();

    /* Save old state */
    struct s2n_stuffer old = stuffer;
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_high_water_mark = stuffer.high_water_mark;

    /* Call function under test */
    void *result = s2n_stuffer_raw_read(&stuffer, data_len);

    /* Postconditions */
    if (result != NULL) {
        /* Success path: read_cursor advanced by data_len */
        /* tainted must be set */
        assert(stuffer.tainted == 1);

        /* blob fields unchanged */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);

        /* write_cursor unchanged */
        assert(stuffer.write_cursor == old_write_cursor);

        /* high_water_mark unchanged (skip_read doesn't modify it) */
        assert(stuffer.high_water_mark == old_high_water_mark);

        /* alloced and growable unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);

        /* result points into the blob data */
        if (stuffer.blob.data != NULL) {
            assert(result == (void *)(stuffer.blob.data + old_read_cursor));
        }

        /* read_cursor advanced */
        assert(stuffer.read_cursor == old_read_cursor + data_len);

    } else {
        /* Failure path: either skip_read failed or blob.data was NULL */
        /* tainted is set regardless (implementation sets it before checking data) */
        /* Actually looking at the implementation: tainted is set after PTR_GUARD_POSIX */
        /* If PTR_GUARD_POSIX fails (skip_read fails), tainted is NOT set */
        /* If skip_read succeeds but data is NULL, tainted IS set */

        /* blob fields unchanged */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);

        /* alloced and growable unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
    }
}

void s2n_stuffer_raw_read_harness(void) {
    s2n_stuffer_raw_read_harness();
    return 0;
}
