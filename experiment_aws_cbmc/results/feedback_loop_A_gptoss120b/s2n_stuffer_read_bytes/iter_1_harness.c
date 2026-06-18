#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

/*--- s2n data structures (as given) ---------------------------------------*/
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

/*--- constants ------------------------------------------------------------*/
#define S2N_SUCCESS 0
#define MAX_BUF_SIZE 256

/*--- function prototypes (implemented elsewhere) --------------------------*/
int s2n_stuffer_read_bytes(struct s2n_stuffer *stuffer,
                           uint8_t *data,
                           uint32_t size);

/*--- helper to model a simple validity predicate ---------------------------*/
static inline bool s2n_stuffer_is_valid(const struct s2n_stuffer *s) {
    if (s == NULL) return false;
    if (s->blob.data == NULL && s->blob.size != 0) return false;
    if (s->blob.size < s->write_cursor) return false;
    if (s->write_cursor < s->read_cursor) return false;
    if (s->high_water_mark < s->write_cursor) return false;
    return true;
}

/*--- harness --------------------------------------------------------------*/
void s2n_stuffer_read_bytes_harness(void) {
    /* 1. Allocate and nondeterministically initialise the stuffer */
    struct s2n_stuffer stuffer;
    /* nondet blob fields */
    stuffer.blob.size = nondet_uint32_t();
    __CPROVER_assume(stuffer.blob.size <= MAX_BUF_SIZE);
    stuffer.blob.allocated = stuffer.blob.size;               /* simple model */
    stuffer.blob.growable = nondet_bool();

    /* allocate blob data (may be NULL if size == 0) */
    if (stuffer.blob.size > 0) {
        stuffer.blob.data = malloc(stuffer.blob.size);
        __CPROVER_assume(stuffer.blob.data != NULL);
    } else {
        stuffer.blob.data = NULL;
    }

    /* initialise cursors respecting invariants */
    stuffer.write_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.write_cursor <= stuffer.blob.size);
    stuffer.read_cursor = nondet_uint32_t();
    __CPROVER_assume(stuffer.read_cursor <= stuffer.write_cursor);
    stuffer.high_water_mark = nondet_uint32_t();
    __CPROVER_assume(stuffer.high_water_mark >= stuffer.write_cursor);
    stuffer.alloced   = nondet_bool();
    stuffer.growable  = nondet_bool();
    stuffer.tainted   = nondet_bool();

    /* 2. Allocate the output buffer (may be NULL) */
    uint8_t *out_data;
    uint32_t out_size = nondet_uint32_t();
    __CPROVER_assume(out_size <= MAX_BUF_SIZE);
    if (nondet_bool()) {
        out_data = NULL;               /* allow NULL to trigger failure path */
    } else {
        out_data = malloc(out_size);
        __CPROVER_assume(out_data != NULL);
    }

    /* 3. Save old state */
    struct s2n_stuffer old = stuffer;
    struct s2n_blob old_blob = stuffer.blob;
    struct store_byte_from_buffer old_bytes;
    if (out_size > 0 && stuffer.blob.data != NULL) {
        /* bytes that will be copied on success */
        save_byte_from_array(stuffer.blob.data + old.read_cursor,
                             out_size,
                             &old_bytes);
    }

    /* 4. Call the function under test */
    int result = s2n_stuffer_read_bytes(&stuffer, out_data, out_size);

    /* 5. Post‑condition checks */
    if (result == S2N_SUCCESS) {
        /* success: read_cursor advanced by out_size */
        assert(stuffer.read_cursor == old.read_cursor + out_size);
        /* other fields unchanged */
        assert(stuffer.write_cursor   == old.write_cursor);
        assert(stuffer.high_water_mark== old.high_water_mark);
        assert(stuffer.alloced        == old.alloced);
        assert(stuffer.growable       == old.growable);
        assert(stuffer.tainted        == old.tainted);
        assert(stuffer.blob.data      == old_blob.data);
        assert(stuffer.blob.size      == old_blob.size);
        assert(stuffer.blob.allocated == old_blob.allocated);
        assert(stuffer.blob.growable  == old_blob.growable);

        /* output buffer contains the bytes that were in the stuffer */
        if (out_size > 0 && out_data != NULL) {
            assert_byte_from_buffer_matches(out_data, &old_bytes);
        }
    } else {
        /* failure: stuffer unchanged */
        assert(stuffer.read_cursor   == old.read_cursor);
        assert(stuffer.write_cursor  == old.write_cursor);
        assert(stuffer.high_water_mark== old.high_water_mark);
        assert(stuffer.alloced       == old.alloced);
        assert(stuffer.growable      == old.growable);
        assert(stuffer.tainted       == old.tainted);
        assert(stuffer.blob.data     == old_blob.data);
        assert(stuffer.blob.size     == old_blob.size);
        assert(stuffer.blob.allocated== old_blob.allocated);
        assert(stuffer.blob.growable == old_blob.growable);
    }

    /* 6. Invariant that the stuffer remains valid */
    assert(s2n_stuffer_is_valid(&stuffer));

    /* free allocated memory */
    free(stuffer.blob.data);
    free(out_data);
}
