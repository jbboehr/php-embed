
# vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab

# args
PHP_ARG_ENABLE(embed, whether to enable embed support,
[  --enable-embed     Enable embed support])

AC_DEFUN([PHP_EMBED_ADD_SOURCES], [
  PHP_EMBED_SOURCES="$PHP_EMBED_SOURCES $1"
])

# main
if test "$PHP_EMBED" != "no"; then
    # compilers
    AC_PROG_CC_STDC
    AC_PROG_CC_C99

    AC_MSG_CHECKING([if compiling with gcc])
    AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([], [[
    #ifndef __GNUC__
        not gcc
    #endif
    ]])],
    [GCC=yes], [GCC=no])
    AC_MSG_RESULT([$GCC])

    AC_MSG_CHECKING([if compiling with clang])
    AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([], [[
    #ifndef __clang__
        not clang
    #endif
    ]])],
    [CLANG=yes], [CLANG=no])
    AC_MSG_RESULT([$CLANG])

    # programs
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

    PHP_EMBED_ADD_SOURCES([
        php_embed.c
    ])

    #PHP_ADD_BUILD_DIR(src)
    PHP_INSTALL_HEADERS([ext/embed], [php_embed.h])
    PHP_NEW_EXTENSION(embed, $PHP_EMBED_SOURCES, $ext_shared, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
    #PHP_ADD_EXTENSION_DEP(embed, ast, true)
    PHP_SUBST(EMBED_SHARED_LIBADD)
fi
