
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_ini.h"
#include "Zend/zend_modules.h"
#include "Zend/zend_operators.h"
#include "main/php.h"
#include "main/php_ini.h"
#include "main/SAPI.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "ext/json/php_json.h"

#include "php_embed.h"

ZEND_DECLARE_MODULE_GLOBALS(embed);

static void (*original_ast_process)(zend_ast *ast) = NULL;

void embed_ast_process_file(zend_ast *ast);

PHP_INI_BEGIN()
PHP_INI_END()

EMBED_PUBLIC zend_never_inline void embed_ast_process(zend_ast *ast)
{
    if (NULL != original_ast_process) {
        original_ast_process(ast);
    }

    embed_ast_process_file(ast);
}

zend_string *embed_file2string(zend_string *dir, zend_string *file)
{
    zend_string *filepath =
        zend_string_concat3(ZSTR_VAL(dir), ZSTR_LEN(dir), ZEND_STRL("/"), ZSTR_VAL(file), ZSTR_LEN(file));

    php_stream *stream = php_stream_open_wrapper(ZSTR_VAL(filepath), "rb", REPORT_ERRORS, NULL);

    if (!stream) {
        zend_throw_exception_ex(
            zend_ce_compile_error, 0, "embed(): failed to read file %.*s", (int) ZSTR_LEN(filepath), ZSTR_VAL(filepath)
        );
        zend_string_release(filepath);
        return NULL;
    }

    zend_string *contents = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);
    php_stream_close(stream);

    if (contents == NULL) {
        zend_throw_exception_ex(
            zend_ce_compile_error, 0, "embed(): failed to read file %.*s", (int) ZSTR_LEN(filepath), ZSTR_VAL(filepath)
        );
        zend_string_release(filepath);
        return NULL;
    }

    zend_string_release(filepath);

    return contents;
}

ZEND_BEGIN_ARG_INFO_EX(embed_arginfo, ZEND_SEND_BY_VAL, 0, 1)
ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_FUNCTION(embed)
{
    zend_string *path;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(path)
    ZEND_PARSE_PARAMETERS_END();

    // This function is never intended to actually be called
    php_error_docref(
        NULL,
        E_WARNING,
        "not intended to be called, the AST pass has likely failed, or the function was called dynamically"
    );

    zend_string *executed_filename = zend_get_executed_filename_ex();
    zend_string *dir = zend_string_dup(executed_filename, 0);
    size_t dir_len = php_dirname(dir->val, dir->len);
    dir = zend_string_truncate(dir, dir_len, 0);

    zend_string *contents = embed_file2string(dir, path);

    zend_string_release(dir);

    if (contents) {
        RETURN_STR(contents);
    } else {
        RETURN_NULL();
    }
}

PHP_FUNCTION(embed_json)
{
    zend_string *path;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STR(path)
    ZEND_PARSE_PARAMETERS_END();

    // This function is never intended to actually be called
    php_error_docref(
        NULL,
        E_WARNING,
        "not intended to be called, the AST pass has likely failed, or the function was called dynamically"
    );

    zend_string *executed_filename = zend_get_executed_filename_ex();
    zend_string *dir = zend_string_dup(executed_filename, 0);
    size_t dir_len = php_dirname(dir->val, dir->len);
    dir = zend_string_truncate(dir, dir_len, 0);

    zend_string *contents = embed_file2string(dir, path);

    zend_string_release(dir);

    if (contents) {
        php_json_decode_ex(
            return_value,
            ZSTR_VAL(contents),
            ZSTR_LEN(contents),
            PHP_JSON_THROW_ON_ERROR | PHP_JSON_OBJECT_AS_ARRAY,
            PHP_JSON_PARSER_DEFAULT_DEPTH
        );

        zend_string_release(contents);
    } else {
        RETURN_NULL();
    }
}

static PHP_RINIT_FUNCTION(embed)
{
#if defined(COMPILE_DL_EMBED) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}

static PHP_MINIT_FUNCTION(embed)
{
    int flags = CONST_CS | CONST_PERSISTENT;

    REGISTER_INI_ENTRIES();

    // Register constants
    REGISTER_STRING_CONSTANT("EmbedExt\\VERSION", (char *) PHP_EMBED_VERSION, flags);

    if (NULL == original_ast_process) {
        original_ast_process = zend_ast_process;
        zend_ast_process = embed_ast_process;
    }

    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(embed)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

static PHP_MINFO_FUNCTION(embed)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Version", PHP_EMBED_VERSION);
    php_info_print_table_row(2, "Released", PHP_EMBED_RELEASE);
    php_info_print_table_row(2, "Authors", PHP_EMBED_AUTHORS);

    DISPLAY_INI_ENTRIES();
}

PHP_GINIT_FUNCTION(embed)
{
#if defined(COMPILE_DL_EMBED) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    memset(embed_globals, 0, sizeof(zend_embed_globals));
}

const zend_function_entry embed_functions[] = {PHP_FE(embed, embed_arginfo) PHP_FE_END};

static const zend_module_dep embed_deps[] = {
    {"json", NULL, NULL, MODULE_DEP_REQUIRED},
    {"ast",  NULL, NULL, MODULE_DEP_OPTIONAL},
    ZEND_MOD_END,
};

zend_module_entry embed_module_entry = {
    STANDARD_MODULE_HEADER_EX,
    NULL,
    embed_deps,                /* Deps */
    PHP_EMBED_NAME,            /* Name */
    embed_functions,           /* Functions */
    PHP_MINIT(embed),          /* MINIT */
    PHP_MSHUTDOWN(embed),      /* MSHUTDOWN */
    PHP_RINIT(embed),          /* RINIT */
    NULL,                      /* RSHUTDOWN */
    PHP_MINFO(embed),          /* MINFO */
    PHP_EMBED_VERSION,         /* Version */
    PHP_MODULE_GLOBALS(embed), /* Globals */
    PHP_GINIT(embed),          /* GINIT */
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX,
};

#ifdef COMPILE_DL_EMBED
#if defined(ZTS)
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(embed) // Common for all PHP extensions which are build as shared modules
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
