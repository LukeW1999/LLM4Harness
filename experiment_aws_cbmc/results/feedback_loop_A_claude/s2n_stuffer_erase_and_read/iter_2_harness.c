#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

/* s2n struct definitions needed for the harness */
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

/* Forward declaration of the function under test */
int s2n_stuffer_erase_and_read(struct s2n_stuffer *stuffer, struct s2n_blob *out);

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void s2n_stuffer_erase_and_read_harness(void) {
    /* 1. Set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic stuffer fields */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Set up the blob inside stuffer */
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Bound the blob size to keep verification tractable */
    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);

    /* Allocate memory for stuffer blob data non-deterministically */
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Constrain cursors to be within blob size */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* 2. Set up the output blob */
    struct s2n_blob out;
    out.size = nondet_uint32_t();
    out.allocated = nondet_uint32_t();
    out.growable = nondet_bool() ? 1 : 0;

    /* Bound out size */
    __CPROVER_assume(out.size <= MAX_BUFFER_SIZE);

    /* Allocate output data buffer */
    if (out.size > 0) {
        out.data = malloc(out.size);
        __CPROVER_assume(out.data != NULL);
    } else {
        out.data = NULL;
    }

    /* 3. Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    struct s2n_blob old_out = out;

    /* 4. Call the function under test */
    int result = s2n_stuffer_erase_and_read(&stuffer, &out);

    /* 5. Assert postconditions */
    if (result == 0) {
        /* Success: data was read from stuffer into out */
        /* read_cursor should have advanced by out->size */
        assert(stuffer.read_cursor == old_stuffer.read_cursor + old_out.size);

        /* write_cursor, high_water_mark, alloced, growable, tainted unchanged */
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);

        /* blob fields unchanged (except data was zeroed in place) */
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);

        /* out blob metadata unchanged */
        assert(out.size == old_out.size);
        assert(out.data == old_out.data);
        assert(out.allocated == old_out.allocated);
        assert(out.growable == old_out.growable);

    } else {
        /* Failure: out should be unchanged */
        assert(out.size == old_out.size);
        assert(out.data == old_out.data);
        assert(out.allocated == old_out.allocated);
        assert(out.growable == old_out.growable);

        /* stuffer blob metadata unchanged */
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);

        /* write_cursor, high_water_mark, alloced, growable, tainted unchanged */
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    }
}
