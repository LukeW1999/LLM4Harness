#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* Minimal s2n definitions needed for the harness */

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

/* Forward declarations for s2n functions used in implementation */
int s2n_stuffer_skip_read(struct s2n_stuffer *stuffer, uint32_t n);

/* Nondet helpers */
uint32_t nondet_uint32(void);
int nondet_int(void);

/* s2n_stuffer_data_available returns write_cursor - read_cursor */
static inline uint32_t s2n_stuffer_data_available(const struct s2n_stuffer *stuffer) {
    return stuffer->write_cursor - stuffer->read_cursor;
}

/* Validity predicate for s2n_stuffer */
static bool s2n_stuffer_is_valid(const struct s2n_stuffer *stuffer) {
    if (stuffer == NULL) return false;
    if (stuffer->read_cursor > stuffer->write_cursor) return false;
    if (stuffer->write_cursor > stuffer->high_water_mark) return false;
    if (stuffer->high_water_mark > stuffer->blob.size) return false;
    if (stuffer->blob.size > 0 && stuffer->blob.data == NULL) return false;
    return true;
}

void s2n_stuffer_skip_read_harness(void) {
    /* 1. Declare and initialize the stuffer non-deterministically */
    struct s2n_stuffer stuffer;

    /* Set up blob */
    uint32_t blob_size = nondet_uint32();
    __CPROVER_assume(blob_size <= 1024); /* bound state space */
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32();
    stuffer.blob.growable = nondet_int() ? 1 : 0;

    if (blob_size > 0) {
        stuffer.blob.data = malloc(blob_size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* Set up cursors with valid constraints */
    stuffer.read_cursor = nondet_uint32();
    stuffer.write_cursor = nondet_uint32();
    stuffer.high_water_mark = nondet_uint32();

    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);
    __CPROVER_assume(stuffer.high_water_mark <= blob_size);

    stuffer.alloced = nondet_int() ? 1 : 0;
    stuffer.growable = nondet_int() ? 1 : 0;
    stuffer.tainted = nondet_int() ? 1 : 0;

    /* Assume stuffer is valid before the call */
    __CPROVER_assume(s2n_stuffer_is_valid(&stuffer));

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;
    uint32_t old_read_cursor = stuffer.read_cursor;
    uint32_t old_write_cursor = stuffer.write_cursor;
    uint32_t old_high_water_mark = stuffer.high_water_mark;
    uint32_t data_available = s2n_stuffer_data_available(&stuffer);

    /* 3. Non-deterministic skip amount */
    uint32_t n = nondet_uint32();

    /* 4. Call function under test */
    int result = s2n_stuffer_skip_read(&stuffer, n);

    /* 5. Assert postconditions */
    if (result == S2N_SUCCESS) {
        /* On success: n <= data_available, read_cursor advances by n */
        assert(n <= data_available);
        assert(stuffer.read_cursor == old_read_cursor + n);

        /* Unchanged fields on success */
        assert(stuffer.write_cursor == old_write_cursor);
        assert(stuffer.high_water_mark == old_high_water_mark);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);

        /* Validity must hold after success */
        assert(s2n_stuffer_is_valid(&stuffer));
    } else {
        /* On failure: stuffer unchanged */
        assert(stuffer.read_cursor == old_read_cursor);
        assert(stuffer.write_cursor == old_write_cursor);
        assert(stuffer.high_water_mark == old_high_water_mark);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);

        /* Validity must hold after failure too */
        assert(s2n_stuffer_is_valid(&stuffer));
    }
}
