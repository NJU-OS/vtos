#include <compiler.h>
#include <stdio.h>
#include <trace.h>
#include <kernel/pseudo_ta.h>
#include <string.h>
#include <mm/core_mmu.h>
#include <io.h>
#include <keep.h>
#include <kernel/misc.h>
#include <kernel/pseudo_ta.h>
#include <kernel/tee_time.h>
#include <kernel/thread.h>
#include <platform_config.h>
#include <utee_defines.h>
#include <tee/tee_cryp_provider.h>

#define TA_NAME  "sta_filter.ta"

#define STA_SEPOL_UUID \
        { 0xff5def46, 0xe2ad, 0x42bf, \
            { 0x83, 0x40, 0x2e, 0x0f, 0xa2, 0xf8, 0xdf, 0x44} }

static uint8_t out[360];
static size_t outlen = 360;
static uint8_t sig[64] = { 0 };
static size_t sig_len = 64;
static uint8_t digest[32] = { 0 };
static struct rsa_keypair key_tp;
static uint8_t mark = 0;
static uint32_t algo = TEE_ALG_RSASSA_PKCS1_V1_5_SHA256;

static uint8_t keystr[] ="\
30820139020100024100B1012F96D1FF953C23053A80CB8973CF08D09172BAC949654CF2075FC85A0C1A0FE0590803876AB580A78B0FAF5E2CF21BB9177163125DB3FFDDD52A11AAACCF0203010001024018808185EF2BE20CA10C683CCBF00B07EDC07161E0595CBF991727CE2A7DCC8ED3CE1C5BE2C08EC0AA7CABF511A436CF0D4F575F7469750DA5ECBB8BEDB4E581022100C13F97BE9392950280A17FF4A11AA5C1DF09D8F2535E3379FF4BC6C3C897BD53022100EA7B46A6050D2C5A3EDB6BAD68C0CE6EE113584408AE0742B8F0D45CFE8DA71502200414BBB7A2962DDF67A80173C11336FCE920E41BF9F6BB4F903CA16DF10683510220596E934F96F5A348F85870053FC2E9F958572C7F266059DC94F525CA223C9CD1022016016C52B990330A5A32747C77D599B287F016426EEE3D77F5DE3DD402EBB2FA";
/*uint8_t kpstr[] = "\
3048024100B1012F96D1FF953C23053A80CB8973CF08D09172BAC949654CF2075FC85A0C1A0FE0590803876AB580A78B0FAF5E2CF21BB9177163125DB3FFDDD52A11AAACCF0203010001";
*/
/*
 * Trusted Application Entry Points
 */

static TEE_Result create_ta(void)
{
    //EMSG("now sta_filter running!");
    return TEE_SUCCESS;
}

static void destroy_ta(void)
{
}

static TEE_Result open_session(uint32_t ptype __unused,
                   TEE_Param params[4] __unused,
                   void **ppsess __unused)
{
	uint32_t res = TEE_SUCCESS;

	if(mark == 0) {
		unsigned int i = 0;
		sn_key key_in;
		outlen = sizeof(keystr)/2;
		for(i=0; i<outlen; i++) {
			uint8_t tp = keystr[i*2];
			uint8_t sum = (tp>='A' ? tp-'A'+10 : tp-'0');
			sum <<= 4;
			tp = keystr[i*2+1];
			sum += (tp>='A' ? tp-'A'+10 : tp-'0');
			out[i] = sum;	
		}
    	res = crypto_ops.rkeyin.sn_import((const uint8_t*)out, outlen, &key_in);
		if(res != TEE_SUCCESS) {
       		EMSG("key import error!\n");
			return res;
		}

    	key_tp.e = key_in.e;
    	key_tp.d = key_in.d;
    	key_tp.n = key_in.N;
    	key_tp.p = key_in.p;
    	key_tp.q = key_in.q;
    	key_tp.qp = key_in.qP;
    	key_tp.dp = key_in.dP;
    	key_tp.dq = key_in.dQ;
		mark = 1;
	}
	return res;
}

static void close_session(void *psess __unused)
{
}

static TEE_Result invoke_command(void *psess __unused,
                 uint32_t cmd __unused, uint32_t ptypes __unused,
                 TEE_Param params[4])
{

	size_t hash_ctx_size;
	void* hash_ctx;
	uint32_t hash_algo = TEE_ALG_SHA256;
	const uint8_t* buf = params[0].memref.buffer;
	int buf_size = params[0].memref.size;
	int res;

	res = crypto_ops.hash.get_ctx_size(hash_algo, &hash_ctx_size);
	if(res != TEE_SUCCESS)
		EMSG("get_ctx_size ERROR!\n");
	hash_ctx = malloc(hash_ctx_size);
	if (!hash_ctx)
		EMSG("malloc ERROR!\n");
	res = crypto_ops.hash.init(hash_ctx, hash_algo);
	if (res != TEE_SUCCESS)
		EMSG("hash init ERROR!\n");
	res = crypto_ops.hash.update(hash_ctx, hash_algo, buf, buf_size);
	if (res != TEE_SUCCESS)
		EMSG("hash update ERROR!\n");
	res = crypto_ops.hash.final(hash_ctx, hash_algo, digest, 32);
	if (res != TEE_SUCCESS)
		EMSG("hash final ERROR!\n");

	res = crypto_ops.acipher.rsassa_sign(algo, &key_tp, -1, 
							(const uint8_t*)digest, 32, sig, &sig_len);
	memcpy(params[1].memref.buffer, sig, sig_len);
	params[2].value.a = sig_len;
	return res;
}

pseudo_ta_register(.uuid = STA_SEPOL_UUID, .name = TA_NAME,
           .flags = PTA_DEFAULT_FLAGS,
           .create_entry_point = create_ta,
           .destroy_entry_point = destroy_ta,
           .open_session_entry_point = open_session,
           .close_session_entry_point = close_session,
           .invoke_command_entry_point = invoke_command);
