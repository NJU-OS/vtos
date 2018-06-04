#define STR_TRACE_USER_TA "LSH"

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include "lsh.h"

#include <user_ta_header.h>
#include <utee_syscalls.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_ext.h"

/*
 Flags Declarations for characters
 */
#define EOF '\0'

/*
  Function Declarations for builtin shell commands:
*/
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_num_builtins(void);
int lsh_launch(char **args);
int lsh_stdin(void);
int lsh_execute(char **args);
char *lsh_read_line(void);
char **lsh_split_line(char *line);
void lsh_loop(void);

/*
  List of builtin commands, followed by their corresponding functions.
*/
const char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins(void)
{
    return sizeof(builtin_str) / sizeof(char *);
}

/*
  @brief Bultin command: change directory.
  @param args List of args.  args[0] is "cd".  args[1] is the directory.
  @return Always returns 1, to continue executing.
*/
int lsh_cd(char **args)
{
    if (args[1] == NULL) {
        DMSG("usage: cd parameter");
    } else {
        DMSG("chdir %s", args[1]);
    }
    return TEE_SUCCESS;
}

/*
  @brief callback by the serial interrupt.
  @param args assume no input NOW.
  @return Always returns 1, to continue executing.
*/
int lsh_stdin(void)
{
    return TEE_SUCCESS;
}

/*
  @brief Builtin command: print help.
  @param args List of args.  Not examined.
  @return Always returns 1, to continue executing.
*/
int lsh_help(char **args)
{
    int i;
    (void)(*args);
    DMSG("vtos shell\n");
    DMSG("Type program names and arguments, and hit enter.\n");
    DMSG("The following are built in:\n");

    for (i = 0; i < lsh_num_builtins(); i++) {
        DMSG("  %s\n", builtin_str[i]);
    }

    DMSG("Use the man command for information on other programs.\n");
    return 1;
}

/*
  @brief Builtin command: exit.
  @param args List of args.  Not examined.
  @return Always returns 0, to terminate execution.
*/
int lsh_exit(char **args)
{
    (void)(*args);
    return 0;
}

/*
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
*/
int lsh_launch(char **args)
{
    (void)(*args);
    /*
      int pid;
      int status;
    */

    // create another process

    return TEE_SUCCESS;
}

/*
  brief Execute shell built-in or launch program.
  param args Null terminated list of arguments.
  return 1 if the shell should continue running, 0 if it should terminate
*/
int lsh_execute(char **args)
{
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/*
  brief Read a line of input from stdin.
  return The line from stdin.
*/
char *lsh_read_line(void)
{
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    char c;

    if (!buffer) {
        DMSG("lsh: allocation error\n");
        return NULL;
    }

    while (1) {
        // Read a character from serial
        // c = getchar();
        c = 'a';

        if (c == EOF) {
            // exit shell
            // exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        // If we have exceeded the buffer, reallocate.
        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                DMSG("lsh: allocation error\n");
                return NULL;
            }
        }
    }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/*
  brief Split a line into tokens (very naively).
  param line The line.
  return Null-terminated array of tokens.
*/
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;

    if (!tokens) {
        DMSG("lsh: allocation error\n");
        return NULL;
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                DMSG("lsh: allocation error\n");
                return NULL;
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/*
  brief Loop getting input and executing it.
*/
void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        DMSG("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

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
    /* Unused parameters */
    (void)&param_types;
    (void)&params;
    (void)&sess_ctx;

    /*
     * The DMSG() macro is non-standard, TEE Internal API doesn't
     * specify any means to logging from a TA.
     */
    DMSG("Hello World         A\n");

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
    uint32_t exp_param_types = TEE_PARAM_TYPES(
        TEE_PARAM_TYPE_VALUE_INOUT,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE);

    (void)params;

    DMSG("has been called");
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    lsh_loop();
    return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result
TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
                           uint32_t cmd_id,
                           uint32_t param_types, TEE_Param params[4])
{
    (void)&sess_ctx; /* Unused parameter */

    switch (cmd_id) {
    case TA_LSH_CMD_INC_VALUE:
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

struct mproc mprocs[16];
void __noreturn ta_main(void)
{
    struct message msg;
    int res;

    trace_ext_puts("==============================\n");
    trace_ext_puts("        This is PM, I am waiting for message\n");
    mprocs[0].mp_endpoint = 0;
    mprocs[1].mp_endpoint = 1;
    while(1) {
        res = sn_receive(-1, &msg);
        if(res != 0) {
            trace_ext_puts("PM receive error!\n");
            continue;
        }

        // process user input
        /*
          lsh_loop();
        */

        if(msg.type == M_TYPE_FORK) {
            DMSG("        PM:I got a FORK msg form %d\n", msg.from);
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

