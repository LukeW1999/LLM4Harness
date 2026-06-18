#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>

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

/* s2n return codes */
#define S2N_SUCCESS 0
#define S2N_FAILURE -1

/* Forward declaration of function under test */
int s2n_stuffer_read_uint8(struct s2n_stuffer *stuffer, uint8_t *u);

void s2n_stuffer_read_uint8_harness(void) {
    /* 1. Declare and initialize the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic blob size, bounded to keep state space manageable */
    uint32_t blob_size;
    __CPROVER_assume(blob_size <= 64);

    stuffer.blob.size = blob_size;

    uint32_t blob_allocated;
    __CPROVER_assume(blob_allocated >= blob_size || blob_allocated == 0);
    stuffer.blob.allocated = blob_allocated;

    unsigned int blob_growable;
    stuffer.blob.growable = blob_growable & 1;

    /* Allocate blob data if size > 0 */
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set up cursors consistently */
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;

    /* Assume valid stuffer state */
    __CPROVER_assume(read_cursor <= write_cursor);
    __CPROVER_assume(write_cursor <= blob_size);
    __CPROVER_assume(high_water_mark >= write_cursor);
    __CPROVER_assume(high_water_mark <= blob_size);

    stuffer.read_cursor = read_cursor;
    stuffer.write_cursor = write_cursor;
    stuffer.high_water_mark = high_water_mark;

    unsigned int alloced;
    unsigned int growable;
    unsigned int tainted;
    stuffer.alloced = alloced & 1;
    stuffer.growable = growable & 1;
    stuffer.tainted = tainted & 1;

    /* 2. Declare output parameter */
    uint8_t u;

    /* 3. Save old state */
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_high_water_mark = stuffer.high_water_mark;
    uint32_t old_blob_size = stuffer.blob.size;
    uint8_t *old_blob_data = stuffer.blob.data;
    uint32_t old_blob_allocated = stuffer.blob.allocated;
    unsigned int old_blob_growable = stuffer.blob.growable;
    unsigned int old_alloced = stuffer.alloced;
    unsigned int old_growable = stuffer.growable;
    unsigned int old_tainted = stuffer.tainted;

    /* 4. Call function under test */
    int result = s2n_stuffer_read_uint8(&stuffer, &u);

    /* 5. Assert postconditions */
    if (result == S2N_SUCCESS) {
        /* On success: read_cursor advances by sizeof(uint8_t) = 1 */
        assert(stuffer.read_cursor == old_read_cursor + 1);

        /* write_cursor unchanged */
        assert(stuffer.write_cursor == old_write_cursor);

        /* high_water_mark unchanged */
        assert(stuffer.high_water_mark == old_high_water_mark);

        /* blob fields unchanged */
        assert(stuffer.blob.size == old_blob_size);
        assert(stuffer.blob.data == old_blob_data);
        assert(stuffer.blob.allocated == old_blob_allocated);
        assert(stuffer.blob.growable == old_blob_growable);

        /* Other stuffer flags unchanged */
        assert(stuffer.alloced == old_alloced);
        assert(stuffer.growable == old_growable);
        assert(stuffer.tainted == old_tainted);

        /* There must have been at least 1 byte available to read */
        assert(old_write_cursor >= old_read_cursor + 1);

        /* Stuffer remains valid */
        assert(stuffer.read_cursor <= stuffer.write_cursor);
        assert(stuffer.write_cursor <= stuffer.blob.size);

    } else {
        /* On failure: stuffer should be unchanged */
        assert(stuffer.read_cursor == old_read_cursor);
        assert(stuffer.write_cursor == old_write_cursor);
        assert(stuffer.high_water_mark == old_high_water_mark);
        assert(stuffer.blob.size == old_blob_size);
        assert(stuffer.blob.data == old_blob_data);
        assert(stuffer.blob.allocated == old_blob_allocated);
        assert(stuffer.blob.growable == old_blob_growable);
        assert(stuffer.alloced == old_alloced);
        assert(stuffer.growable == old_growable);
        assert(stuffer.tainted == old_tainted);
    }
}
