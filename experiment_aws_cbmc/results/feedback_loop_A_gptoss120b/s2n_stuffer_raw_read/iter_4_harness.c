#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Nondeterministic helpers provided by CBMC */
extern bool nondet_bool(void);
extern uint32_t nondet_uint32_t(void);

#define MAX_BUFFER_SIZE 1024

/* s2n struct layout (for harness construction) */
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

/* external functions used by the implementation */
int s2n_stuffer_skip_read(struct s2n_stuffer *stuffer, uint32_t data_len);
void *s2n_stuffer_raw_read(struct s2n_stuffer *stuffer, uint32_t data_len);

void s2n_stuffer_raw_read_harness(void) {
    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;
    uint8_t buffer[MAX_BUFFER_SIZE];

    /* blob fields */
    stuffer.blob.data = nondet_bool() ? NULL : buffer;
    stuffer.blob.size = nondet_uint32_t();
    stuffer.blob.allocated = nondet_uint32_t();
    stuffer.blob.growable = nondet_bool();

    /* stuffer cursors and flags */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    /* 2. Impose reasonable bounds / invariants */
    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 4. Nondeterministic data_len respecting the stuffer bounds */
    uint32_t data_len = nondet_uint32_t();
    __CPROVER_assume(data_len <= stuffer.blob.size);
    __CPROVER_assume(data_len <= stuffer.write_cursor - stuffer.read_cursor);

    /* 5. Call the function under test */
    void *result = s2n_stuffer_raw_read(&stuffer, data_len);

    /* 6. Post‑condition checks */
    if (result != NULL) {
        /* success path */
        assert(stuffer.tainted == 1);
        assert(stuffer.read_cursor == old.read_cursor + data_len);
        assert(result == (old.blob.data ? old.blob.data + old.read_cursor : NULL));

        /* fields that must stay unchanged */
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
    } else {
        /* failure path – the stuffer must be unchanged */
        assert(stuffer.tainted == old.tainted);
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
    }

    /* 7. Global validity invariants */
    assert(stuffer.read_cursor <= stuffer.write_cursor);
    assert(stuffer.write_cursor <= stuffer.blob.size);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
    if (stuffer.blob.data != NULL) {
        assert(stuffer.blob.allocated >= stuffer.blob.size);
    }
}
