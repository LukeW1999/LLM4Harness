#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

/* Definitions from s2n headers (simplified for the harness) */
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

/* Function under test (declared elsewhere) */
int s2n_stuffer_wipe(struct s2n_stuffer *stuffer);

/* Constants used by the implementation */
#define S2N_WIPE_PATTERN ((uint8_t)0xFF)
#define MAX_BUFFER_SIZE   1024U

void s2n_stuffer_wipe_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;

    /* nondet size for the blob (bounded) */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);

    /* allocate the blob's data buffer */
    stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.size);
    __CPROVER_assume(stuffer.blob.data != NULL);

    /* allocated must be >= size (or equal for this harness) */
    stuffer.blob.allocated = stuffer.blob.size;
    stuffer.blob.growable = nondet_bool();

    /* cursors and flags */
    stuffer.read_cursor   = nondet_uint32_t();
    stuffer.write_cursor  = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced        = nondet_bool();
    stuffer.growable       = nondet_bool();
    stuffer.tainted        = nondet_bool();

    /* basic consistency assumptions */
    __CPROVER_assume(stuffer.read_cursor   <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor  <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* save a copy of the original blob data */
    uint8_t *old_blob_data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.size);
    __CPROVER_assume(old_blob_data != NULL);
    for (uint32_t i = 0; i < stuffer.blob.size; ++i) {
        old_blob_data[i] = stuffer.blob.data[i];
    }

    /* 3. Call the function under test */
    int result = s2n_stuffer_wipe(&stuffer);

    /* 4. Post‑condition checks */
    if (result == 0) { /* success */
        /* fields that must be reset */
        assert(stuffer.read_cursor   == 0);
        assert(stuffer.write_cursor  == 0);
        assert(stuffer.high_water_mark == 0);

        /* blob fields must stay the same */
        assert(stuffer.blob.data      == old.blob.data);
        assert(stuffer.blob.size      == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable  == old.blob.growable);

        /* stuffer flags must stay the same */
        assert(stuffer.alloced   == old.alloced);
        assert(stuffer.growable  == old.growable);
        assert(stuffer.tainted   == old.tainted);

        /* bytes beyond the previous write_cursor are wiped */
        for (uint32_t i = old.write_cursor; i < stuffer.blob.size; ++i) {
            assert(stuffer.blob.data[i] == S2N_WIPE_PATTERN);
        }
        /* bytes that were part of the previous content remain unchanged */
        for (uint32_t i = 0; i < old.write_cursor; ++i) {
            assert(stuffer.blob.data[i] == old_blob_data[i]);
        }
    } else { /* failure */
        /* the whole structure must be unchanged */
        assert(stuffer.read_cursor   == old.read_cursor);
        assert(stuffer.write_cursor  == old.write_cursor);
        assert(stuffer.high_water_mark == old.high_water_mark);
        assert(stuffer.blob.data      == old.blob.data);
        assert(stuffer.blob.size      == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable  == old.blob.growable);
        assert(stuffer.alloced       == old.alloced);
        assert(stuffer.growable      == old.growable);
        assert(stuffer.tainted       == old.tainted);

        /* all blob bytes must be unchanged */
        for (uint32_t i = 0; i < stuffer.blob.size; ++i) {
            assert(stuffer.blob.data[i] == old_blob_data[i]);
        }
    }

    /* 5. Invariant that must always hold */
    assert(stuffer.read_cursor   <= stuffer.write_cursor);
    assert(stuffer.write_cursor  <= stuffer.blob.size);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
}
