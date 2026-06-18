#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Minimal s2n type definitions needed for the harness */
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

/* Forward declarations for s2n functions */
int s2n_stuffer_read(struct s2n_stuffer *stuffer, struct s2n_blob *out);

/* Nondet helpers */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

void s2n_stuffer_read_harness(void) {
    /* 1. Declare and initialize the stuffer */
    struct s2n_stuffer stuffer;

    /* Initialize blob data */
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= 64); /* bound for tractability */
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= blob_size || stuffer.blob.allocated == 0);
    
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Initialize stuffer cursors */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    
    /* Ensure valid cursor state */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= blob_size);
    __CPROVER_assume(stuffer.high_water_mark >= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* 2. Declare and initialize the output blob */
    struct s2n_blob out;
    uint32_t out_size = nondet_uint32_t();
    __CPROVER_assume(out_size <= 64); /* bound for tractability */
    out.size = out_size;
    out.allocated = nondet_uint32_t();
    out.growable = nondet_bool() ? 1 : 0;
    
    if (out_size > 0) {
        out.data = malloc(out_size);
        __CPROVER_assume(out.data != NULL);
    } else {
        out.data = NULL;
    }

    /* 3. Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    struct s2n_blob old_out = out;

    /* 4. Call function under test */
    int result = s2n_stuffer_read(&stuffer, &out);

    /* 5. Assert postconditions */
    
    /* The out blob fields should not be modified by s2n_stuffer_read itself */
    assert(out.size == old_out.size);
    assert(out.data == old_out.data);
    assert(out.allocated == old_out.allocated);
    assert(out.growable == old_out.growable);

    /* The stuffer blob pointer and size should not change */
    assert(stuffer.blob.data == old_stuffer.blob.data);
    assert(stuffer.blob.size == old_stuffer.blob.size);
    assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
    assert(stuffer.blob.growable == old_stuffer.blob.growable);

    /* Stuffer metadata should not change */
    assert(stuffer.alloced == old_stuffer.alloced);
    assert(stuffer.growable == old_stuffer.growable);
    assert(stuffer.tainted == old_stuffer.tainted);
    assert(stuffer.write_cursor == old_stuffer.write_cursor);
    assert(stuffer.high_water_mark == old_stuffer.high_water_mark);

    if (result == 0) {
        /* Success: read_cursor should have advanced by out.size */
        assert(stuffer.read_cursor == old_stuffer.read_cursor + old_out.size);
    } else {
        /* Failure: read_cursor should be unchanged */
        assert(stuffer.read_cursor == old_stuffer.read_cursor);
    }
}

void s2n_stuffer_read_harness(void) {
    s2n_stuffer_read_harness();
    return 0;
}
