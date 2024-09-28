/**
 * Copyright (c) anno Domini nostri Jesu Christi MMXVI-MMXXIV John Boehr & contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "Zend/zend_API.h"
#include "Zend/zend_exceptions.h"
#include "main/php.h"
#include "main/php_streams.h"
#include "ext/standard/php_string.h"
#include "ext/json/php_json.h"
#include "php_embed.h"

struct vyrtue_preprocess_context;

EMBED_LOCAL
zend_string *embed_file2string(zend_string *dir, zend_string *file);

static zend_string *embed_extract_constant_string_argument(zend_ast *ast)
{
    ZEND_ASSERT(ast->child[1]->kind == ZEND_AST_ARG_LIST);

    zend_ast_list *args = zend_ast_get_list(ast->child[1]);

    if (args->children != 1) {
        // @todo change to local exception class?
        zend_throw_exception_ex(zend_ce_compile_error, 0, "embed(): only accepts one argument");
        return NULL;
    }

    zend_ast *arg = args->child[0];

    if (arg->kind != ZEND_AST_ZVAL) {
        // @todo change to local exception class?
        zend_throw_exception_ex(zend_ce_compile_error, 0, "embed(): only accepts a constant string as an argument");
        return NULL;
    }

    zval *val = zend_ast_get_zval(arg);
    if (Z_TYPE_P(val) != IS_STRING) {
        // @todo change to local exception class?
        zend_throw_exception_ex(zend_ce_compile_error, 0, "embed(): only accepts a constant string as an argument");
        return NULL;
    }

    return Z_STR_P(val);
}

static inline zend_string *get_compiled_dir(void)
{
    zend_string *dir = zend_string_dup(CG(compiled_filename), 0);
    size_t dir_len = php_dirname(dir->val, dir->len);
    return zend_string_truncate(dir, dir_len, 0);
}

EMBED_LOCAL
zend_ast *embed_call_embed_visitor_leave(zend_ast *ast, struct vyrtue_preprocess_context *ctx)
{
    zend_string *arg = embed_extract_constant_string_argument(ast);

    if (!arg) {
        return NULL;
    }

    zend_string *dir = get_compiled_dir();
    zend_string *contents = embed_file2string(dir, arg);
    zend_string_release(dir);

    if (contents != NULL) {
        zend_ast *rv = zend_ast_create_zval_from_str(contents);
        // zend_string_release(contents);
        return rv;
    }

    return NULL;
}

EMBED_LOCAL
zend_ast *embed_call_embed_json_visitor_leave(zend_ast *ast, struct vyrtue_preprocess_context *ctx)
{
    zend_string *arg = embed_extract_constant_string_argument(ast);

    if (!arg) {
        return NULL;
    }

    zend_string *dir = get_compiled_dir();
    zend_string *contents = embed_file2string(dir, arg);
    zend_string_release(dir);

    if (contents != NULL) {
        zval tmp = {0};
        int result = php_json_decode_ex(
            &tmp,
            ZSTR_VAL(contents),
            ZSTR_LEN(contents),
            PHP_JSON_THROW_ON_ERROR | PHP_JSON_OBJECT_AS_ARRAY,
            PHP_JSON_PARSER_DEFAULT_DEPTH
        );

        zend_string_release(contents);

        if (result == SUCCESS) {
            return zend_ast_create_zval(&tmp);
        }
    }

    return NULL;
}
