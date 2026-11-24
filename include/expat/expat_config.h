#ifndef EXPAT_CONFIG_H
#define EXPAT_CONFIG_H

/* Build configuration of expat 2.0.0 (src/expat), written for this project in place
 * of the configure script's output. XML_DTD and XML_NS stay undefined: with either,
 * xmlrole.c and xmltok.c hold functions the original does not have. */

/* little-endian Allegrex */
#define BYTEORDER 1234
#define HAVE_MEMMOVE 1

#endif
