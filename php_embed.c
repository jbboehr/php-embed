
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "Zend/zend_API.h"
#include "Zend/zend_constants.h"
#include "Zend/zend_ini.h"
#include "Zend/zend_modules.h"
#include "Zend/zend_operators.h"
#include "main/php.h"
#include "main/php_ini.h"
#include "main/SAPI.h"
#include "ext/standard/info.h"

#include "php_embed.h"

ZEND_DECLARE_MODULE_GLOBALS(embed);

// static void (*original_ast_process)(zend_ast *ast) = NULL;
// static zend_op_array *(*original_compile_file)(zend_file_handle *file_handle, int type) = NULL;
// static zend_op_array *(*original_compile_string)(zend_string *source_string, const char *filename) = NULL;

// void embed_ast_process_file(zend_ast *ast);

PHP_INI_BEGIN()
PHP_INI_END()

// EMBED_PUBLIC zend_never_inline void embed_ast_process(zend_ast *ast)
// {
//     if (NULL != original_ast_process) {
//         original_ast_process(ast);
//     }

//     embed_ast_process_file(ast);
// }

// EMBED_PUBLIC zend_never_inline zend_op_array *embed_compile_file(zend_file_handle *file_handle, int type)
// {
//     zend_op_array *opline = original_compile_file(file_handle, type);
//     return opline;
// }

// EMBED_PUBLIC zend_never_inline zend_op_array *embed_compile_string(zend_string *source_string, const char *filename)
// {
//     zend_op_array *opline = original_compile_string(source_string, filename);
//     return opline;
// }

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

    // if (NULL == original_ast_process) {
    //     original_ast_process = zend_ast_process;
    //     zend_ast_process = embed_ast_process;
    // }

    // if (NULL == original_compile_file) {
    //     original_compile_file = zend_compile_file;
    //     zend_compile_file = embed_compile_file;
    // }

    // if (NULL == original_compile_string) {
    //     original_compile_string = zend_compile_string;
    //     zend_compile_string = embed_compile_string;
    // }

    return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(embed)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

static PHP_MINFO_FUNCTION(embed)
{
    char buf[64];

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

const zend_function_entry embed_functions[] = {
    PHP_FE_END,
};

static const zend_module_dep embed_deps[] = {
    {"ast", NULL, NULL, MODULE_DEP_OPTIONAL},
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
