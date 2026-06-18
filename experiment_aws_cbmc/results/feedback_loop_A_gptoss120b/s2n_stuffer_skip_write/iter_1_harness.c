#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Definitions from s2n */
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

/* Return codes */
#define S2N_SUCCESS 0
#define S2N_ERROR   -1

/* External functions from s2n */
extern int s2n_stuffer_skip_write(struct s2n_stuffer *stuffer, const uint32_t n);
extern int s2n_stuffer_validate(const struct s2n_stuffer *stuffer);

/* Bounding constants for the harness */
#define MAX_BLOB_SIZE 256

void s2n_stuffer_skip_write_harness(void) {
    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;

    /* nondet blob size and allocated */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= MAX_BLOB_SIZE);
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    stuffer.blob.growable = nondet_bool();

    /* allocate blob data */
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* cursors within bounds */
    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.blob.size);
    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    stuffer.alloced   = nondet_bool();
    stuffer.growable  = nondet_bool();
    stuffer.tainted   = nondet_bool();

    /* 2. Assume the stuffer is initially valid */
    __CPROVER_assume(s2n_stuffer_validate(&stuffer) == S2N_SUCCESS);

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 4. Nondeterministic skip amount */
    uint32_t n = nondet_uint32_t();

    /* 5. Call the function under test */
    int result = s2n_stuffer_skip_write(&stuffer, n);

    /* 6. Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* write cursor advanced by n */
        assert(stuffer.write_cursor == old.write_cursor + n);
        /* read cursor unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        /* high water mark is the max of old high water and new write cursor */
        uint32_t new_write = old.write_cursor + n;
        uint32_t expected_hwm = (old.high_water_mark > new_write) ? old.high_water_mark : new_write;
        assert(stuffer.high_water_mark == expected_hwm);
        /* blob fields unchanged */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        /* stuffer flags unchanged */
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
        /* cursors stay within bounds */
        assert(stuffer.write_cursor <= stuffer.blob.size);
        assert(stuffer.read_cursor <= stuffer.blob.size);
        assert(stuffer.high_water_mark <= stuffer.blob.size);
    } else {
        /* on failure the stuffer must be unchanged */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    }

    /* 7. The stuffer must remain valid */
    assert(s2n_stuffer_validate(&stuffer) == S2N_SUCCESS);

    return 0;
}
