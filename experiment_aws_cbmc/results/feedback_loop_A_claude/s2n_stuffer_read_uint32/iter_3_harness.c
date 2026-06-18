#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

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

/* Declare the function under test */
int s2n_stuffer_read_uint32(struct s2n_stuffer *stuffer, uint32_t *u);

/* Nondet helpers */
uint32_t nondet_uint32(void);
int nondet_int(void);

void s2n_stuffer_read_uint32_harness(void) {
    /* 1. Set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Non-deterministic blob size - must be at least 4 bytes for success */
    uint32_t blob_size = nondet_uint32();
    __CPROVER_assume(blob_size <= 64); /* bound state space */
    
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32();
    stuffer.blob.growable = nondet_int() ? 1 : 0;
    
    /* Allocate blob data */
    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set up cursors */
    stuffer.read_cursor = nondet_uint32();
    stuffer.write_cursor = nondet_uint32();
    stuffer.high_water_mark = nondet_uint32();
    stuffer.alloced = nondet_int() ? 1 : 0;
    stuffer.growable = nondet_int() ? 1 : 0;
    stuffer.tainted = nondet_int() ? 1 : 0;

    /* Valid stuffer invariants:
     * read_cursor <= write_cursor <= blob.size */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark >= stuffer.write_cursor);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* 2. Set up output pointer - may be NULL (to test POSIX_ENSURE_REF) */
    uint32_t *u_ptr;
    int use_null = nondet_int();
    uint32_t u_val = 0;
    if (use_null) {
        u_ptr = NULL;
    } else {
        u_ptr = &u_val;
    }

    /* 3. Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_high_water_mark = stuffer.high_water_mark;
    uint32_t old_blob_size = stuffer.blob.size;
    uint8_t *old_blob_data = stuffer.blob.data;

    /* 4. Call function under test */
    int result = s2n_stuffer_read_uint32(&stuffer, u_ptr);

    /* 5. Assert postconditions */
    if (result == 0) { /* S2N_SUCCESS */
        /* u_ptr must not be NULL on success (POSIX_ENSURE_REF would have failed) */
        assert(u_ptr != NULL);
        
        /* read_cursor advances by 4 bytes on success */
        assert(stuffer.read_cursor == old_read_cursor + 4);
        
        /* write_cursor unchanged */
        assert(stuffer.write_cursor == old_write_cursor);
        
        /* blob unchanged */
        assert(stuffer.blob.size == old_blob_size);
        assert(stuffer.blob.data == old_blob_data);
        
        /* high_water_mark unchanged */
        assert(stuffer.high_water_mark == old_high_water_mark);
        
        /* There must have been at least 4 bytes available */
        assert(old_read_cursor + 4 <= old_write_cursor);
        
        /* stuffer flags unchanged */
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        
    } else { /* failure */
        /* On failure, stuffer state should be unchanged */
        assert(stuffer.write_cursor == old_write_cursor);
        assert(stuffer.blob.size == old_blob_size);
        assert(stuffer.blob.data == old_blob_data);
        assert(stuffer.high_water_mark == old_high_water_mark);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
    }

    return 0;
}
