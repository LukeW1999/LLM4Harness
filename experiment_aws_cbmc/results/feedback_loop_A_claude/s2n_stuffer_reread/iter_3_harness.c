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

/* s2n return codes */
#define S2N_SUCCESS 0
#define S2N_FAILURE -1

/* Forward declaration of the function under test */
int s2n_stuffer_reread(struct s2n_stuffer *stuffer);

/* Nondet helpers */
uint32_t nondet_uint32_t(void);
bool nondet_bool(void);

/* s2n_stuffer validity predicate */
static bool s2n_stuffer_is_valid(const struct s2n_stuffer *stuffer) {
    if (stuffer == NULL) return false;
    if (stuffer->read_cursor > stuffer->write_cursor) return false;
    if (stuffer->write_cursor > stuffer->high_water_mark) return false;
    if (stuffer->high_water_mark > stuffer->blob.size) return false;
    if (stuffer->blob.size > 0 && stuffer->blob.data == NULL) return false;
    return true;
}

void s2n_stuffer_reread_harness(void) {
    /* 1. Declare and initialize the stuffer non-deterministically */
    struct s2n_stuffer stuffer;

    /* Set up blob */
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Bound the size to keep verification tractable */
    __CPROVER_assume(stuffer.blob.size <= 1024);

    /* Allocate data if size > 0 */
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set cursors non-deterministically but satisfying validity */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    /* Enforce validity preconditions */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* Precondition: stuffer must be valid */
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Call function under test */
    int result = s2n_stuffer_reread(&stuffer);

    /* 4. Assert postconditions */
    if (result == S2N_SUCCESS) {
        /* read_cursor is reset to 0 on success */
        assert(stuffer.read_cursor == 0);

        /* All other fields unchanged */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    } else {
        /* On failure, nothing should have changed */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    }

    /* 5. Validity invariant must hold after the call */
    assert(s2n_stuffer_is_valid(&stuffer));

    return 0;
}
