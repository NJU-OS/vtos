/*
 * Copyright (c) 2015, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef ELF_LOAD_H
#define ELF_LOAD_H

#include <types_ext.h>
#include <tee_api_types.h>

struct elf_load_state {
    bool is_32bit;

    uint8_t *nwdata;
    size_t nwdata_len;

    void *hash_ctx;
    uint32_t hash_algo;

    size_t next_offs;

    void *ta_head;
    size_t ta_head_size;

    void *ehdr;
    void *phdr;

    size_t vasize;
    void *shdr;
};
TEE_Result sn_elf_load_head(struct elf_load_state *state, size_t head_size,
            void **head, size_t *vasize);

TEE_Result elf_load_init(void *hash_ctx, uint32_t hash_algo, uint8_t *nwdata,
			size_t nwdata_len, struct elf_load_state **state);
TEE_Result elf_load_head(struct elf_load_state *state, size_t head_size,
			void **head, size_t *vasize, bool *is_32bit);
TEE_Result elf_load_body(struct elf_load_state *state, vaddr_t vabase);
// SNOWFLY
TEE_Result sn_elf_load_body(struct elf_load_state *state, vaddr_t vabase);
TEE_Result elf_load_get_next_segment(struct elf_load_state *state, size_t *idx,
			vaddr_t *vaddr, size_t *size, uint32_t *flags);
void elf_load_final(struct elf_load_state *state);
// SNOWFLY
void sn_elf_load_final(struct elf_load_state *state);

#endif /*ELF_LOAD_H*/
