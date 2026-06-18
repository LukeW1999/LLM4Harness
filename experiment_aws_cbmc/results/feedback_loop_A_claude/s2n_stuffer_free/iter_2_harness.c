#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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
int s2n_stuffer_free(struct s2n_stuffer *stuffer);

/* Nondet helpers */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

void s2n_stuffer_free_harness(void) {
    /* Stack-allocate a stuffer */
    struct s2n_stuffer stuffer;

    /* Initialize fields non-deterministically */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Initialize blob */
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Allocate blob data if needed */
    if (stuffer.blob.allocated > 0) {
        stuffer.blob.data = malloc(stuffer.blob.allocated);
        __CPROVER_assume(stuffer.blob.data != NULL);
        __CPROVER_assume(stuffer.blob.size <= stuffer.blob.allocated);
    } else {
        stuffer.blob.data = NULL;
        stuffer.blob.size = 0;
    }

    /* Assume valid stuffer state for precondition */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* If alloced, blob must be growable and have allocated memory */
    if (stuffer.alloced) {
        __CPROVER_assume(stuffer.blob.growable == 1);
        __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    }

    /* Call the function under test */
    int result = s2n_stuffer_free(&stuffer);

    /* Postconditions */
    if (result == 0) { /* S2N_SUCCESS */
        assert(stuffer.blob.data == NULL);
        assert(stuffer.blob.size == 0);
        assert(stuffer.blob.allocated == 0);
        assert(stuffer.blob.growable == 0);
        assert(stuffer.read_cursor == 0);
        assert(stuffer.write_cursor == 0);
        assert(stuffer.high_water_mark == 0);
        assert(stuffer.alloced == 0);
        assert(stuffer.growable == 0);
        assert(stuffer.tainted == 0);
    } else {
        assert(result == -1);
    }

    return 0;
}
