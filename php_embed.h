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

#ifndef PHP_EMBED_H
#define PHP_EMBED_H

#include "main/php.h"

#define PHP_EMBED_NAME "embed"
#define PHP_EMBED_VERSION "0.1.0"
#define PHP_EMBED_RELEASE "2024-01-27"
#define PHP_EMBED_AUTHORS "John Boehr <jbboehr@gmail.com> (lead)"

#if (__GNUC__ >= 4) || defined(__clang__) || defined(HAVE_FUNC_ATTRIBUTE_VISIBILITY)
#define EMBED_PUBLIC __attribute__((visibility("default")))
#define EMBED_LOCAL __attribute__((visibility("hidden")))
#elif defined(PHP_WIN32) && defined(EMBED_EXPORTS)
#define EMBED_PUBLIC __declspec(dllexport)
#define EMBED_LOCAL
#else
#define EMBED_PUBLIC
#define EMBED_LOCAL
#endif

extern zend_module_entry embed_module_entry;
#define phpext_embed_ptr &embed_module_entry

#if defined(ZTS) && ZTS
#include "TSRM.h"
#endif

#if defined(ZTS) && ZTS
#define EMBED_G(v) TSRMG(embed_globals_id, zend_embed_globals *, v)
#else
#define EMBED_G(v) (embed_globals.v)
#endif

#if defined(ZTS) && defined(COMPILE_DL_EMBED)
ZEND_TSRMLS_CACHE_EXTERN();
#endif

ZEND_BEGIN_MODULE_GLOBALS(embed)
ZEND_END_MODULE_GLOBALS(embed)

ZEND_EXTERN_MODULE_GLOBALS(embed);

#endif /* PHP_EMBED_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: et sw=4 ts=4
 */
