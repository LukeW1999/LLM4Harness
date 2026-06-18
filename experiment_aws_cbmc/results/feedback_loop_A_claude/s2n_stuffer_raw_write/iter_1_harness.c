#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

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

void s2n_stuffer_raw_write_harness(void) {
    struct s2n_stuffer stuffer;

    /* Non-deterministic initialization of stuffer fields */
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

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
        stuffer.blob.data = nondet_bool() ? NULL : malloc(1);
    }

    uint32_t data_len = nondet_uint32_t();
    __CPROVER_assume(data_len <= 64);

    /* Save old state */
    struct s2n_stuffer old = stuffer;
    uint32_t old_write_cursor = stuffer.write_cursor;

    /* Call function under test */
    void *result = s2n_stuffer_raw_write(&stuffer, data_len);

    if (result != NULL) {
        /* Success path */
        /* write_cursor advanced by data_len */
        assert(stuffer.write_cursor == old_write_cursor + data_len);
        /* tainted is set */
        assert(stuffer.tainted == 1);
        /* blob data unchanged */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        /* result points into blob */
        assert(stuffer.blob.data != NULL);
        assert(result == (void *)(stuffer.blob.data + stuffer.write_cursor - data_len));
        /* read_cursor unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        /* alloced, growable unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        /* blob allocated unchanged */
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        /* Stuffer invariants still hold */
        assert(stuffer.read_cursor <= stuffer.write_cursor);
        assert(stuffer.write_cursor <= stuffer.blob.size);
    } else {
        /* Failure path - either skip_write failed or blob.data is NULL */
        /* tainted may or may not be set depending on where failure occurred */
        /* blob unchanged */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
    }
}

void s2n_stuffer_raw_write_harness(void) {
    s2n_stuffer_raw_write_harness();
    return 0;
}
