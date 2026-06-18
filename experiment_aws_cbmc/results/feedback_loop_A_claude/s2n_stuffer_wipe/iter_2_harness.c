#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Minimal s2n_blob and s2n_stuffer definitions matching the header */
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

int s2n_stuffer_wipe(struct s2n_stuffer *stuffer);

/* Nondet helpers */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

void main(void) {
    /* 1. Declare and set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic blob size, bounded to keep state space manageable */
    uint32_t blob_size = nondet_uint32_t();
    __CPROVER_assume(blob_size <= 64);

    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= blob_size);
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Allocate data if size > 0 */
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set up cursors - must be within valid range */
    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= blob_size);

    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= blob_size);
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);

    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Call function under test */
    int result = s2n_stuffer_wipe(&stuffer);

    /* 4. Assert postconditions */
    if (result == 0) {
        /* On success: cursors should be reset to 0 */
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);

        /* Blob metadata should be unchanged */
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);

        /* Stuffer flags should be unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    } else {
        /* On failure: stuffer should be in a consistent state */
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
    }
}
