#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

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

/* Forward declarations for s2n functions */
int s2n_stuffer_write_uint8(struct s2n_stuffer *stuffer, const uint8_t u);

#define S2N_SUCCESS 0
#define S2N_FAILURE -1

/* Maximum buffer size for bounding */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

/* Nondet helpers */
uint8_t nondet_uint8_t(void);
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

void s2n_stuffer_write_uint8_harness(void) {
    /* 1. Declare and set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic initialization of stuffer fields */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Set up the blob */
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= MAX_BUFFER_SIZE);
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size || stuffer.blob.allocated == 0);
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Allocate data buffer for the blob */
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Bound cursors to valid ranges */
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* Non-deterministic uint8 value to write */
    uint8_t u = nondet_uint8_t();

    /* 2. Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_read_cursor = stuffer.read_cursor;

    /* 3. Call function under test */
    int result = s2n_stuffer_write_uint8(&stuffer, u);

    /* 4. Assert postconditions */
    if (result == S2N_SUCCESS) {
        /* On success: write_cursor advances by 1 (sizeof(uint8_t)) */
        assert(stuffer.write_cursor == old_write_cursor + 1);
        /* read_cursor should not change */
        assert(stuffer.read_cursor == old_read_cursor);
        /* alloced, growable, tainted flags should remain the same */
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    } else {
        /* On failure: stuffer should be unchanged or in a consistent state */
        assert(stuffer.read_cursor == old_read_cursor);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
    }

    /* 5. Fields that must not change regardless of result */
    assert(stuffer.read_cursor == old_read_cursor);
    assert(stuffer.alloced == old_stuffer.alloced);
    assert(stuffer.growable == old_stuffer.growable);
    assert(stuffer.tainted == old_stuffer.tainted);

    return 0;
}
