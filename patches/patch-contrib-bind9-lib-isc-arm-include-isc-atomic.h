--- contrib/bind9/lib/isc/arm/include/isc/atomic.h.orig	2011-11-10 23:20:22.000000000 -0500
+++ contrib/bind9/lib/isc/arm/include/isc/atomic.h	2012-04-06 16:32:13.146816031 -0400
@@ -53,22 +53,18 @@
 
 	__asm __volatile("1:\n"
 	    "adr	%1, 1b\n"
-	    "mov	%0, #0xe0000004\n"
 	    "str	%1, [%0]\n"
-	    "mov	%0, #0xe0000008\n"
 	    "adr	%1, 2f\n"
-	    "str	%1, [%0]\n"
+	    "str	%1, [%0, #4]\n"
 	    "ldr	%1, [%2]\n"
 	    "cmp	%1, %3\n"
 	    "streq	%4, [%2]\n"
 	    "2:\n"
 	    "mov	%3, #0\n"
-	    "mov	%0, #0xe0000004\n"
 	    "str	%3, [%0]\n"
 	    "mov	%3, #0xffffffff\n"
-	    "mov	%0, #0xe0000008\n"
-	    "str	%3, [%0]\n"
-	    : "=r" (ras_start), "=r" (done)
+	    "str	%3, [%0, #4]\n"
+	    : "+r" (ras_start), "=r" (done)
 	    ,"+r" (p), "+r" (cmpval), "+r" (val) : : "memory");
 	return (done);
 
