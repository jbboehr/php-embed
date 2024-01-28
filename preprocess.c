
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

struct ast_ctx
{
    zend_string *dir;
};

zend_string *embed_file2string(zend_string *dir, zend_string *file);

static zend_ast *embed_ast_walk(zend_ast *ast, struct ast_ctx *ctx);

static void embed_ast_enter_node(zend_ast *ast, struct ast_ctx *ctx)
{
    ;
}

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

static zend_ast *embed_ast_leave_node(zend_ast *ast, struct ast_ctx *ctx)
{
    if (ast->kind == ZEND_AST_CALL) {
        zval *name = zend_ast_get_zval(ast->child[0]);
        if (Z_TYPE_P(name) == IS_STRING && zend_string_equals_literal(Z_STR_P(name), "embed")) {
            zend_string *arg = embed_extract_constant_string_argument(ast);

            if (!arg) {
                return NULL;
            }

            zend_string *contents = embed_file2string(ctx->dir, arg);

            if (contents != NULL) {
                zend_ast *rv = zend_ast_create_zval_from_str(contents);
                // zend_string_release(contents);
                return rv;
            } else {
                return NULL;
            }
        }

        if (Z_TYPE_P(name) == IS_STRING && zend_string_equals_literal(Z_STR_P(name), "embed_json")) {
            zend_string *arg = embed_extract_constant_string_argument(ast);

            if (!arg) {
                return NULL;
            }

            zend_string *contents = embed_file2string(ctx->dir, arg);

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
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        }
    }

    return NULL;
}

static void embed_ast_walk_children(zend_ast *ast, struct ast_ctx *ctx)
{
    uint32_t i, children = zend_ast_get_num_children(ast);
    for (i = 0; i < children; i++) {
        if (UNEXPECTED(ast->child[i] == NULL)) {
            continue;
        }

        zend_ast *replace_child = embed_ast_walk(ast->child[i], ctx);
        if (UNEXPECTED(replace_child != NULL)) {
            zend_ast_destroy(ast->child[i]);
            ast->child[i] = replace_child;
        }
    }
}

static void embed_ast_walk_list(zend_ast *ast, zend_ast_list *list, bool is_stmt_list, struct ast_ctx *ctx)
{
    for (int i = 0; i < list->children; i++) {
        if (UNEXPECTED(NULL == list->child[i])) {
            continue;
        }

        zend_ast *replace_child = embed_ast_walk(list->child[i], ctx);

        if (UNEXPECTED(replace_child != NULL)) {
            zend_ast_destroy(list->child[i]);
            list->child[i] = replace_child;
        }
    }
}

static zend_ast *embed_ast_walk(zend_ast *ast, struct ast_ctx *ctx)
{
    zend_ast_decl *decl;

    embed_ast_enter_node(ast, ctx);

    switch (ast->kind) {
        case ZEND_AST_FUNC_DECL:
        case ZEND_AST_CLOSURE:
        case ZEND_AST_METHOD:
        case ZEND_AST_ARROW_FUNC:
        case ZEND_AST_CLASS: {
            decl = (zend_ast_decl *) ast;
            if (EXPECTED(decl->child[2])) {
                if (EXPECTED(zend_ast_is_list(decl->child[2]))) {
                    embed_ast_walk_list(ast, zend_ast_get_list(decl->child[2]), true, ctx);
                } else {
                    embed_ast_walk_children(decl->child[2], ctx);
                }
            }
            goto done;
        }

        case ZEND_AST_STMT_LIST: {
            embed_ast_walk_list(ast, zend_ast_get_list(ast), true, ctx);
            goto done;
        }

        default:
            break;
    }

    if (zend_ast_is_list(ast)) {
        embed_ast_walk_list(ast, zend_ast_get_list(ast), false, ctx);
    } else {
        embed_ast_walk_children(ast, ctx);
    }

done:
    return embed_ast_leave_node(ast, ctx);
}

void embed_ast_process_file(zend_ast *ast)
{
    zend_string *dir = zend_string_dup(CG(compiled_filename), 0);
    size_t dir_len = php_dirname(dir->val, dir->len);
    dir = zend_string_truncate(dir, dir_len, 0);

    struct ast_ctx ctx = {.dir = dir};

    embed_ast_walk(ast, &ctx);

    zend_string_release(dir);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
