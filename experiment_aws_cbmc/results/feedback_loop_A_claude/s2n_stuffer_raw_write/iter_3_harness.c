#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Minimal s2n definitions needed */
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

void *s2n_stuffer_raw_write(struct s2n_stuffer *stuffer, const uint32_t data_len);

/* Nondet helpers */
uint32_t nondet_uint32(void);
int nondet_int(void);

void s2n_stuffer_raw_write_harness(void) {
    struct s2n_stuffer stuffer;

    /* Non-deterministic initialization of stuffer fields */
    stuffer.blob.size = nondet_uint32();
    stuffer.blob.allocated = nondet_uint32();
    stuffer.blob.growable = nondet_int() ? 1 : 0;
    stuffer.read_cursor = nondet_uint32();
    stuffer.write_cursor = nondet_uint32();
    stuffer.high_water_mark = nondet_uint32();
    stuffer.alloced = nondet_int() ? 1 : 0;
    stuffer.growable = nondet_int() ? 1 : 0;
    stuffer.tainted = nondet_int() ? 1 : 0;

    /* Bound blob size to keep state space manageable */
    __CPROVER_assume(stuffer.blob.size <= 64);

    /* Valid stuffer invariants */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* Allocate blob data non-deterministically */
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    uint32_t data_len = nondet_uint32();
    __CPROVER_assume(data_len <= 64);

    /* Save old state */
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint8_t *old_blob_data = stuffer.blob.data;
    uint32_t old_blob_size = stuffer.blob.size;
    uint32_t old_blob_allocated = stuffer.blob.allocated;
    unsigned old_blob_growable = stuffer.blob.growable;
    unsigned old_alloced = stuffer.alloced;
    unsigned old_growable = stuffer.growable;

    /* Call function under test */
    void *result = s2n_stuffer_raw_write(&stuffer, data_len);

    if (result != NULL) {
        /* Success path */
        /* write_cursor advanced by data_len */
        assert(stuffer.write_cursor == old_write_cursor + data_len);
        /* tainted is set */
        assert(stuffer.tainted == 1);
        /* blob data unchanged */
        assert(stuffer.blob.data == old_blob_data);
        assert(stuffer.blob.size == old_blob_size);
        /* result points into blob */
        assert(stuffer.blob.data != NULL);
        assert(result == (void *)(stuffer.blob.data + old_write_cursor));
        /* read_cursor unchanged */
        assert(stuffer.read_cursor == old_read_cursor);
        /* alloced, growable unchanged */
        assert(stuffer.alloced == old_alloced);
        assert(stuffer.growable == old_growable);
        /* blob allocated unchanged */
        assert(stuffer.blob.allocated == old_blob_allocated);
        assert(stuffer.blob.growable == old_blob_growable);
        /* Stuffer invariants still hold */
        assert(stuffer.read_cursor <= stuffer.write_cursor);
        assert(stuffer.write_cursor <= stuffer.blob.size);
    } else {
        /* Failure path */
        assert(stuffer.blob.data == old_blob_data);
        assert(stuffer.blob.size == old_blob_size);
        assert(stuffer.blob.allocated == old_blob_allocated);
        assert(stuffer.blob.growable == old_blob_growable);
        assert(stuffer.read_cursor == old_read_cursor);
        assert(stuffer.alloced == old_alloced);
        assert(stuffer.growable == old_growable);
    }

    return 0;
}
