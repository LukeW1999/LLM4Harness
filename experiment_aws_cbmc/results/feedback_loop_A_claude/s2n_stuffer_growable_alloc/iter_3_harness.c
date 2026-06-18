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
int s2n_stuffer_growable_alloc(struct s2n_stuffer *stuffer, const uint32_t size);

void s2n_stuffer_growable_alloc_harness(void) {
    /* 1. Declare and initialize the stuffer */
    struct s2n_stuffer stuffer;
    
    /* Non-deterministic size - bound it to avoid state space explosion */
    uint32_t size;
    __CPROVER_assume(size <= 1024); /* bound size for tractability */
    
    /* Initialize stuffer to a known state (uninitialized/zeroed) */
    __CPROVER_assume(stuffer.read_cursor == 0);
    __CPROVER_assume(stuffer.write_cursor == 0);
    __CPROVER_assume(stuffer.high_water_mark == 0);
    __CPROVER_assume(stuffer.alloced == 0);
    __CPROVER_assume(stuffer.growable == 0);
    __CPROVER_assume(stuffer.tainted == 0);
    __CPROVER_assume(stuffer.blob.data == NULL);
    __CPROVER_assume(stuffer.blob.size == 0);
    __CPROVER_assume(stuffer.blob.allocated == 0);
    __CPROVER_assume(stuffer.blob.growable == 0);

    /* 2. Call function under test */
    int result = s2n_stuffer_growable_alloc(&stuffer, size);

    /* 3. Assert postconditions */
    if (result == 0) { /* S2N_SUCCESS */
        /* On success: stuffer is allocated and growable */
        assert(stuffer.growable == 1);
        assert(stuffer.alloced == 1);
        
        /* The blob should be allocated with at least 'size' bytes */
        assert(stuffer.blob.allocated >= size);
        
        /* Read and write cursors should be at 0 after fresh alloc */
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        
        /* Tainted should remain 0 after a fresh alloc */
        assert(stuffer.tainted == 0);
        
        /* If size > 0, data pointer should be non-null */
        if (size > 0) {
            assert(stuffer.blob.data != NULL);
        }
        
        /* Blob should be growable since stuffer is growable */
        assert(stuffer.blob.growable == 1);
    } else {
        /* On failure: result should be -1 (S2N_FAILURE) */
        assert(result == -1);
        
        /* On failure, growable should not be set if alloc failed */
        assert(stuffer.growable == 0);
    }

    return 0;
}
