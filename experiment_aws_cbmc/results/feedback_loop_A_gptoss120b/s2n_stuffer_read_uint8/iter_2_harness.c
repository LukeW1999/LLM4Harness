#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BLOB_SIZE 256

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

/* Function prototypes (implementation provided elsewhere) */
int s2n_stuffer_read_uint8(struct s2n_stuffer *stuffer, uint8_t *u);
int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer, void *buf, size_t len);

void s2n_stuffer_read_uint8_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare and nondeterministically initialize the stuffer */
    struct s2n_stuffer stuffer;

    /* Allocate blob data */
    uint32_t blob_data_len = nondet_uint32_t();
    __CPROVER_assume(blob_data_len <= MAX_BLOB_SIZE);
    stuffer.blob.data = allocator->malloc(allocator, blob_data_len);
    __CPROVER_assume(stuffer.blob.data != NULL);
    stuffer.blob.size = blob_data_len;

    /* Allocate field may be equal to size or larger */
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= stuffer.blob.size);
    stuffer.blob.growable = nondet_bool();

    /* Cursors */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();

    /* Ensure cursor invariants before the call */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);

    /* Flags */
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    /* 2. Allocate output variable on stack */
    uint8_t u_val;
    uint8_t *u = &u_val;

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;

    /* 4. Call function under test */
    int result = s2n_stuffer_read_uint8(&stuffer, u);

    /* 5. Postcondition checks */
    if (result == 0) { /* S2N_SUCCESS */
        /* read_cursor must advance by exactly one byte */
        assert(stuffer.read_cursor == old.read_cursor + 1);
        /* The byte read must match the original buffer content */
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
        /* On failure, the stuffer must remain unchanged */
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

    /* 6. Validity invariants that must always hold */
    assert(stuffer.read_cursor <= stuffer.write_cursor);
    assert(stuffer.write_cursor <= stuffer.blob.size);
    assert(stuffer.high_water_mark <= stuffer.blob.size);
}
