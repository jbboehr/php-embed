# Copyright (c) anno Domini nostri Jesu Christi MMXVI-MMXXIV John Boehr & contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
