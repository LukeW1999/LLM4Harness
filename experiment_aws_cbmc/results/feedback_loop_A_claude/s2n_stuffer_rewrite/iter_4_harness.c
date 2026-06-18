#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* s2n struct definitions */
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
int s2n_stuffer_rewrite(struct s2n_stuffer *stuffer);

/* Nondet declarations */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

void s2n_stuffer_rewrite_harness(void) {
    /* 1. Declare and initialize stuffer non-deterministically */
    struct s2n_stuffer stuffer;

    /* Set up blob data non-deterministically */
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Allocate data buffer if size > 0 */
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set cursor and mark fields non-deterministically */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Assume valid stuffer preconditions */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);
    if (stuffer.blob.size > 0) {
        __CPROVER_assume(stuffer.blob.data != NULL);
    }

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Call function under test */
    int result = s2n_stuffer_rewrite(&stuffer);

    /* 4. Assert postconditions */
    if (result == 0) {
        /* Success: write_cursor and read_cursor reset to 0 */
        assert(stuffer.write_cursor == 0);
        assert(stuffer.read_cursor == 0);

        /* Unchanged fields on success */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    } else {
        /* Failure: stuffer should be unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    }

    return 0;
}
