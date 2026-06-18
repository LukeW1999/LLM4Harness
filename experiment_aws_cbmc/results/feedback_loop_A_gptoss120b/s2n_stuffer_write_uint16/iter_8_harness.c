#include <assert.h>
#include <stdint.h>
#include <aws/common/allocator.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>

/* Re‑declare the structs as described in the prompt */
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

/* Function under test */
int s2n_stuffer_write_uint16(struct s2n_stuffer *stuffer, const uint16_t u);

/* Helper to compute the expected high water mark after a successful write */
static inline uint32_t expected_hwm(uint32_t old_hwm, uint32_t old_write, uint32_t inc) {
    uint32_t new_write = old_write + inc;
    return (new_write > old_hwm) ? new_write : old_hwm;
}

/* Maximum buffer size for the harness */
#define MAX_BUFFER_SIZE 1024

void s2n_stuffer_write_uint16_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate and initialise a stuffer */
    struct s2n_stuffer stuffer;

    /* nondet allocation size, bounded */
    uint32_t blob_cap = nondet_uint32_t();
    __CPROVER_assume(blob_cap <= MAX_BUFFER_SIZE);

    stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, blob_cap);
    __CPROVER_assume(stuffer.blob.data != NULL || blob_cap == 0);
    stuffer.blob.size = blob_cap;
    stuffer.blob.allocated = blob_cap;
    stuffer.blob.growable = nondet_bool();

    stuffer.read_cursor   = nondet_uint32_t();
    stuffer.write_cursor  = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    stuffer.alloced   = nondet_bool();
    stuffer.growable  = nondet_bool();
    stuffer.tainted   = nondet_bool();

    /* basic validity assumptions */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* 2. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 3. Nondet input value */
    uint16_t u = nondet_uint16_t();

    /* 4. Call the function under test */
    int result = s2n_stuffer_write_uint16(&stuffer, u);

    /* 5. Post‑condition checks */
    if (result == 0) { /* success */
        /* write cursor must advance by exactly 2 bytes */
        assert(stuffer.write_cursor == old.write_cursor + 2);
        /* must not exceed the blob capacity */
        assert(stuffer.write_cursor <= stuffer.blob.size);

        /* high water mark is updated if the new write cursor exceeds it */
        assert(stuffer.high_water_mark ==
               expected_hwm(old.high_water_mark, old.write_cursor, 2));

        /* the two bytes written must be the network‑order representation of u */
        uint8_t hi = (uint8_t)((u >> 8) & 0xFF);
        uint8_t lo = (uint8_t)(u & 0xFF);
        assert(stuffer.blob.data[old.write_cursor] == hi);
        assert(stuffer.blob.data[old.write_cursor + 1] == lo);

        /* fields that must stay unchanged */
        assert(stuffer.read_cursor == old.read_cursor);
        assert(stuffer.blob.data == old.blob.data);
        assert(stuffer.blob.size == old.blob.size);
        assert(stuffer.blob.allocated == old.blob.allocated);
        assert(stuffer.blob.growable == old.blob.growable);
        assert(stuffer.alloced == old.alloced);
        assert(stuffer.growable == old.growable);
        assert(stuffer.tainted == old.tainted);
    } else { /* failure */
        /* all fields must remain exactly as they were */
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

    /* 6. Global validity invariants that must always hold */
    assert(stuffer.read_cursor <= stuffer.write_cursor);
    assert(stuffer.write_cursor <= stuffer.blob.size);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
}
