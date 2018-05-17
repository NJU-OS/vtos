/*
 * Copyright (c) 2016, Linaro Limited
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

#define STR_TRACE_USER_TA "HELLO_WORLD"

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include "hello_world_ta.h"
// SNOWFLY
#include <user_ta_header.h>
#include <utee_syscalls.h>

// SNOWFLY
struct mproc {
    uint32_t mp_num;
    int mp_endpoint;
    int mp_father;
};

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");
	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	/*
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	//unsigned num = 0;
	
	if (param_types != exp_param_types)
		param_types = 0;
	else
		param_types = 1;
	*/
		//return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&param_types;
	(void)&params;
	(void)&sess_ctx;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	DMSG("Hello World         A\n");
	/*
	while(param_types != 2) {
		if(num % (16*1024*1024) == 0)
		DMSG("Hello World         %d\n", num/(16*1024*1024));
		num += 1;
	}
	*/

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	DMSG("Goodbye!\n");
}

static TEE_Result inc_value(uint32_t param_types,
	TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	DMSG("Got value: %u from NW", params[0].value.a);
	params[0].value.a++;
	DMSG("Increase value to: %u", params[0].value.a);
	return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id) {
	case TA_HELLO_WORLD_CMD_INC_VALUE:
		return inc_value(param_types, params);
#if 0
	case TA_HELLO_WORLD_CMD_XXX:
		return ...
		break;
	case TA_HELLO_WORLD_CMD_YYY:
		return ...
		break;
	case TA_HELLO_WORLD_CMD_ZZZ:
		return ...
		break;
	...
#endif
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}

/*
struct mproc mprocs[16];
void __noreturn ta_main(void)
{

	struct message msg;
    int res;

    trace_ext_puts("==================================================================================\n");
    trace_ext_puts("        This is PM, I am waiting for message\n");
    mprocs[0].mp_endpoint = 0;
    mprocs[1].mp_endpoint = 1;
    while(1) {
        res = sn_receive(-1, &msg);
        if(res != 0) {
            trace_ext_puts("PM receive error!\n");
            continue;
        }
        if(msg.type == M_TYPE_FORK) {
            printf("        PM:I got a FORK msg form %d\n", msg.from);
            res = pm_fork(msg.from);
            if(res < 0)
                msg.u.mp_pid = -1;
            else
                msg.u.mp_pid = 0;
            sn_send(msg.from, &msg);
            if(res >= 0) {
                msg.u.mp_pid = res;
                sn_send(res, &msg);
            }
        }
    }
}
*/

void __noreturn ta_main(void)
{
    unsigned num = 0;
    int temp = 0;
    int res = fork();
    char parent = 'a';
    char child = '0';
    if(res<0)
        trace_ext_puts("fork error!  \n");
    else if(res == 0)
        trace_ext_puts("This is process 1:I am father\n");
    else
        printf("This is process %d:I am child\n", res);
    if(res == 0) { // parent process
        while(1) {
            if(num % (32*1024*1024) == 0 && temp<14) {
                printf("process 1: %c\n", parent);
                parent++;
                if(parent == 'i')
                    sleep(4);
                if(parent > 'z')
                    parent = 'a';
                temp++;
            }
            num += 1;
        }
    }else{
        while(1) {
            if(num % (32*1024*1024) == 0 && temp<22) {
                printf("process %d: %c\n", res, child);
                child++;
                if(child > '9')
                    child = '0';
                temp++;
            }
            num += 1;
        }
    }
}
