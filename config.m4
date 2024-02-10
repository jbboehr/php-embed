
PHP_ARG_ENABLE(embed, whether to enable embed support,
[  --enable-embed     Enable embed support])

AC_DEFUN([PHP_EMBED_ADD_SOURCES], [
  PHP_EMBED_SOURCES="$PHP_EMBED_SOURCES $1"
])

if test "$PHP_EMBED" != "no"; then
    PHP_EMBED_ADD_SOURCES([
        src/extension.c
        src/process.c
    ])

    PHP_ADD_BUILD_DIR(src)
    PHP_INSTALL_HEADERS([ext/embed], [php_embed.h])
    PHP_NEW_EXTENSION(embed, $PHP_EMBED_SOURCES, $ext_shared, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
    PHP_ADD_EXTENSION_DEP(embed, json, false)
    PHP_ADD_EXTENSION_DEP(embed, ast, true)
    PHP_ADD_EXTENSION_DEP(embed, opcache, true)
    PHP_ADD_EXTENSION_DEP(embed, vyrtue, true)
    PHP_SUBST(EMBED_SHARED_LIBADD)
fi
