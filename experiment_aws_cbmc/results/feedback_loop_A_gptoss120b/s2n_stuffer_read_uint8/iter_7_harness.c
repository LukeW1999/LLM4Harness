#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Nondeterministic helpers */
extern uint32_t nondet_uint32_t(void);
extern uint8_t  nondet_uint8_t(void);
extern bool     nondet_bool(void);

/* Allocator helpers */
struct aws_allocator;
extern struct aws_allocator *aws_default_allocator(void);
extern void *aws_default_allocator_malloc(struct aws_allocator *allocator, size_t size);

/* s2n structures */
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

/* Function prototypes */
int s2n_stuffer_read_uint8(struct s2n_stuffer *stuffer, uint8_t *u);
int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer, void *buf, size_t len);

void s2n_stuffer_read_uint8_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and nondeterministically initialize the stuffer */
    struct s2n_stuffer stuffer;

    /* Allocate blob data */
    uint32_t blob_data_len = nondet_uint32_t();
    __CPROVER_assume(blob_data_len <= 256);
    stuffer.blob.data = (uint8_t *)aws_default_allocator_malloc(allocator, blob_data_len);
    __CPROVER_assume(blob_data_len == 0 || stuffer.blob.data != NULL);
    stuffer.blob.size = blob_data_len;
    stuffer.blob.allocated = blob_data_len;               /* allocated matches actual allocation */
    stuffer.blob.growable = nondet_bool();

    /* Initialise blob contents with nondet values */
    for (uint32_t i = 0; i < blob_data_len; ++i) {
        stuffer.blob.data[i] = nondet_uint8_t();
    }

    /* Cursors */
    stuffer.read_cursor  = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    /* Invariants before the call */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);
    __CPROVER_assume(stuffer.read_cursor < stuffer.write_cursor); /* at least one byte to read */

    /* Additional precondition for the internal memcpy */
    __CPROVER_assume(stuffer.read_cursor + 1 <= stuffer.blob.allocated);

    /* Flags – alloced must be true for a successful read */
    stuffer.alloced = true;
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    /* 2. Output variable */
    uint8_t u_val;
    uint8_t *u = &u_val;

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 4. Call function under test */
    int result = s2n_stuffer_read_uint8(&stuffer, u);

    /* 5. Post‑condition checks */
    if (result == 0) { /* S2N_SUCCESS */
        assert(stuffer.read_cursor == old.read_cursor + 1);
        assert(*u == old.blob.data[old.read_cursor]);

        /* Unchanged fields */
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.blob.data == old.blob.data);
    } else {
        /* On failure the stuffer must remain unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.write_cursor == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.blob.data == old.blob.data);
    }

    /* 6. Invariants that must always hold */
    assert(stuffer.read_cursor <= stuffer.write_cursor);
    assert(stuffer.write_cursor <= stuffer.blob.size);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
}
