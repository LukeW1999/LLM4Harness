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

/* External declarations for s2n functions */
int s2n_stuffer_alloc(struct s2n_stuffer *stuffer, const uint32_t size);

/* Nondet helper */
uint32_t nondet_uint32_t(void);

void s2n_stuffer_alloc_harness(void) {
    /* 1. Declare inputs */
    struct s2n_stuffer stuffer;
    uint32_t size = nondet_uint32_t();

    /* Bound the size to keep verification tractable */
    __CPROVER_assume(size <= 1024);

    /* 2. Call function under test */
    int result = s2n_stuffer_alloc(&stuffer, size);

    /* 3. Assert postconditions */
    if (result == 0 /* S2N_SUCCESS */) {
        assert(stuffer.alloced == 1);
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
        assert(stuffer.tainted == 0);

        /* blob should have been allocated with the requested size */
        assert(stuffer.blob.size == size);
        assert(stuffer.blob.allocated >= size);
        assert(stuffer.blob.growable == 1);

        /* If size > 0, data pointer should be non-null */
        if (size > 0) {
            assert(stuffer.blob.data != NULL);
        }

        assert(stuffer.read_cursor <= stuffer.write_cursor);
        assert(stuffer.write_cursor <= stuffer.blob.size);
    } else {
        assert(result != 0);
    }
}

void s2n_stuffer_alloc_harness(void) {
    s2n_stuffer_alloc_harness();
    return 0;
}
