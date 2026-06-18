#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* Nondeterministic helpers provided by CBMC */
extern uint32_t nondet_uint32_t(void);
extern _Bool nondet_bool(void);

/* Minimal AWS allocator definitions */
struct aws_allocator { int dummy; };
struct aws_allocator *aws_default_allocator(void);
void *aws_mem_acquire(struct aws_allocator *allocator, size_t size);

/* Minimal s2n data structures */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    bool growable;
};

struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    bool alloced;
    bool growable;
    bool tainted;
};

/* Function under test */
int s2n_stuffer_erase_and_read(struct s2n_stuffer *stuffer, struct s2n_blob *out);

#define MAX_BUFFER_SIZE 1024
#define S2N_SUCCESS 0

void s2n_stuffer_erase_and_read_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct s2n_stuffer stuffer;
    struct s2n_blob out;

    /* Blob sizes */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size > 0);
    __CPROVER_assume(stuffer.blob.size <= MAX_BUFFER_SIZE);
    out.size = nondet_uint32_t();
    __CPROVER_assume(out.size > 0);
    __CPROVER_assume(out.size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(out.size <= stuffer.blob.size);

    /* Allocate buffers */
    stuffer.blob.data = (uint8_t *)aws_mem_acquire(allocator, stuffer.blob.size);
    __CPROVER_assume(stuffer.blob.data != NULL);
    out.data = (uint8_t *)aws_mem_acquire(allocator, out.size);
    __CPROVER_assume(out.data != NULL);

    /* Blob metadata */
    stuffer.blob.allocated = stuffer.blob.size;
    stuffer.blob.growable = nondet_bool();

    out.allocated = out.size;
    out.growable = nondet_bool();

    /* Stuffer cursors and flags */
    stuffer.read_cursor  = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    stuffer.alloced = nondet_bool();
    stuffer.growable = nondet_bool();
    stuffer.tainted = nondet_bool();

    /* Clamp cursors to blob size */
    if (stuffer.read_cursor > stuffer.blob.size) {
        stuffer.read_cursor = stuffer.blob.size;
    }
    if (stuffer.write_cursor > stuffer.blob.size) {
        stuffer.write_cursor = stuffer.blob.size;
    }
    if (stuffer.read_cursor > stuffer.write_cursor) {
        stuffer.read_cursor = stuffer.write_cursor;
    }

    /* Preconditions required by s2n_stuffer_erase_and_read */
    __CPROVER_assume(stuffer.alloced);                                   /* stuffer must be allocated */
    __CPROVER_assume(stuffer.read_cursor + out.size <= stuffer.write_cursor); /* enough data to read */
    __CPROVER_assume(stuffer.read_cursor + out.size <= stuffer.blob.size);    /* within blob bounds */

    /* Preserve old state for post‑condition checks */
    struct s2n_stuffer old_stuffer = stuffer;
    struct s2n_blob   old_out     = out;

    uint8_t *old_region = NULL;
    if (out.size > 0) {
        old_region = (uint8_t *)aws_mem_acquire(allocator, out.size);
        __CPROVER_assume(old_region != NULL);
        for (uint32_t i = 0; i < out.size; ++i) {
            old_region[i] = stuffer.blob.data[old_stuffer.read_cursor + i];
        }
    }

    uint8_t *old_out_data = NULL;
    if (out.size > 0) {
        old_out_data = (uint8_t *)aws_mem_acquire(allocator, out.size);
        __CPROVER_assume(old_out_data != NULL);
        for (uint32_t i = 0; i < out.size; ++i) {
            old_out_data[i] = out.data[i];
        }
    }

    int result = s2n_stuffer_erase_and_read(&stuffer, &out);

    if (result == S2N_SUCCESS) {
        /* Successful read/erase */
        assert(stuffer.read_cursor == old_stuffer.read_cursor + out.size);
        /* The erased region should be zeroed */
        for (uint32_t i = 0; i < out.size; ++i) {
            assert(stuffer.blob.data[old_stuffer.read_cursor + i] == 0);
        }
    } else {
        /* Failure – out data must be unchanged */
        for (uint32_t i = 0; i < out.size; ++i) {
            assert(out.data[i] == old_out_data[i]);
        }
        /* Stuffer state must be unchanged except possibly read_cursor */
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
        assert(stuffer.read_cursor == old_stuffer.read_cursor ||
               stuffer.read_cursor == old_stuffer.read_cursor + out.size);
        for (uint32_t i = 0; i < out.size; ++i) {
            assert(stuffer.blob.data[old_stuffer.read_cursor + i] == old_region[i]);
        }
    }

    /* Global invariants – stuffer and out structures must remain otherwise unchanged */
    assert(stuffer.blob.data == old_stuffer.blob.data);
    assert(stuffer.blob.size == old_stuffer.blob.size);
    assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
    assert(stuffer.blob.growable == old_stuffer.blob.growable);
    assert(stuffer.write_cursor == old_stuffer.write_cursor);
    assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
    assert(stuffer.alloced == old_stuffer.alloced);
    assert(stuffer.growable == old_stuffer.growable);
    assert(stuffer.tainted == old_stuffer.tainted);

    assert(out.data == old_out.data);
    assert(out.size == old_out.size);
    assert(out.allocated == old_out.allocated);
    assert(out.growable == old_out.growable);

    if (stuffer.blob.size > 0) {
        assert(stuffer.blob.data != NULL);
    }
    if (out.size > 0) {
        assert(out.data != NULL);
    }
}
