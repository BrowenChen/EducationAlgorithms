/* OSCATS: Open-Source Computerized Adaptive Testing System
 * PHP Bindings
 * Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
 *
 *  OSCATS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OSCATS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OSCATS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <php.h>

PHP_RINIT_FUNCTION(oscats);

//ZEND_DECLARE_MODULE_GLOBALS(oscats);

zend_module_entry oscats_module_entry = {
    STANDARD_MODULE_HEADER,
    "php-oscats",
    NULL,		// functions
    NULL,		// minit
    NULL,		// mshutdown
    PHP_RINIT(oscats),		// rinit
    NULL,		// rshutdown
    NULL,		// minfo
    "$Revision",
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_OSCATS
ZEND_GET_MODULE(oscats)
#endif

PHP_RINIT_FUNCTION(oscats)
{
  phpg_oscats_register_classes();
  phpg_oscats_register_constants("OSCATS_");
  return SUCCESS;
}
