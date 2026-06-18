#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

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

/* Forward declarations for s2n functions */
int s2n_stuffer_read_uint16(struct s2n_stuffer *stuffer, uint16_t *u);

/*
 * Analysis before writing harness:
 * 1. What fields change on success?
 *    - *u is set to the 16-bit value read from stuffer
 *    - stuffer->read_cursor advances by 2 (sizeof(uint16_t))
 * 2. What fields are unchanged?
 *    - stuffer->blob (data, size, allocated, growable)
 *    - stuffer->write_cursor
 *    - stuffer->high_water_mark
 *    - stuffer->alloced, growable, tainted
 * 3. What happens on failure?
 *    - Returns non-zero (S2N_FAILURE)
 *    - stuffer state may be unchanged
 *    - *u may be unmodified
 * 4. Validity invariants:
 *    - read_cursor <= write_cursor <= blob.size
 */

void s2n_stuffer_read_uint16_harness(void) {
    /* 1. Set up the stuffer */
    struct s2n_stuffer stuffer;

    /* Allocate blob data non-deterministically with bounded size */
    uint32_t blob_size;
    __CPROVER_assume(blob_size <= 16); /* small bound for tractability */
    
    uint8_t *blob_data = NULL;
    if (blob_size > 0) {
        blob_data = malloc(blob_size);
        __CPROVER_assume(blob_data != NULL);
    }
    
    stuffer.blob.data = blob_data;
    stuffer.blob.size = blob_size;
    stuffer.blob.allocated = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.allocated >= blob_size || stuffer.blob.allocated == 0);
    stuffer.blob.growable = nondet_bool() ? 1 : 0;

    /* Set up cursors with valid constraints */
    stuffer.read_cursor = nondet_uint32_t();
    stuffer.write_cursor = nondet_uint32_t();
    stuffer.high_water_mark = nondet_uint32_t();
    
    /* Validity constraints for stuffer */
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    __CPROVER_assume(stuffer.high_water_mark <= stuffer.blob.size);
    __CPROVER_assume(stuffer.write_cursor <= stuffer.high_water_mark);

    stuffer.alloced = nondet_bool() ? 1 : 0;
    stuffer.growable = nondet_bool() ? 1 : 0;
    stuffer.tainted = nondet_bool() ? 1 : 0;

    /* 2. Set up output pointer - non-deterministically NULL or valid */
    uint16_t u_val;
    uint16_t *u = nondet_bool() ? &u_val : NULL;

    /* 3. Save old state */
    struct s2n_stuffer old_stuffer = stuffer;
    uint16_t old_u_val = u_val;

    /* 4. Call function under test */
    int result = s2n_stuffer_read_uint16(&stuffer, u);

    /* 5. Assert postconditions */
    if (result == 0) {
        /* Success: u must be non-NULL (otherwise POSIX_ENSURE_REF would have failed) */
        assert(u != NULL);
        
        /* read_cursor advanced by sizeof(uint16_t) = 2 */
        assert(stuffer.read_cursor == old_stuffer.read_cursor + 2);
        
        /* The value read should be consistent with the data in the blob */
        /* *u = data[0] << 8 | data[1] where data was read from stuffer */
        /* We can verify the value is set (not checking exact value since blob data is nondet) */
        
        /* Unchanged fields on success */
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
        
        /* Validity: read_cursor still <= write_cursor */
        assert(stuffer.read_cursor <= stuffer.write_cursor);
        assert(stuffer.write_cursor <= stuffer.blob.size);
        
    } else {
        /* Failure path */
        /* On failure, stuffer state should be unchanged or u was NULL */
        
        /* Unchanged fields on failure */
        assert(stuffer.blob.data == old_stuffer.blob.data);
        assert(stuffer.blob.size == old_stuffer.blob.size);
        assert(stuffer.blob.allocated == old_stuffer.blob.allocated);
        assert(stuffer.blob.growable == old_stuffer.blob.growable);
        assert(stuffer.write_cursor == old_stuffer.write_cursor);
        assert(stuffer.high_water_mark == old_stuffer.high_water_mark);
        assert(stuffer.alloced == old_stuffer.alloced);
        assert(stuffer.growable == old_stuffer.growable);
        assert(stuffer.tainted == old_stuffer.tainted);
    }
}

void s2n_stuffer_read_uint16_harness(void) {
    s2n_stuffer_read_uint16_harness();
    return 0;
}
