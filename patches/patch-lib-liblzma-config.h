--- /usr/src/lib/liblzma/config.h.orig	2010-09-24 13:42:10.346344850 -0400
+++ /usr/src/lib/liblzma/config.h	2010-09-24 13:44:29.568683203 -0400
@@ -66,7 +66,7 @@
 #define SIZEOF_SIZE_T 8
 #define STDC_HEADERS 1
 #define TUKLIB_CPUCORES_SYSCONF 1
-#define TUKLIB_FAST_UNALIGNED_ACCESS 1
+/* #define TUKLIB_FAST_UNALIGNED_ACCESS 1 */
 #define TUKLIB_PHYSMEM_SYSCONF 1
 #ifndef _ALL_SOURCE
 # define _ALL_SOURCE 1
