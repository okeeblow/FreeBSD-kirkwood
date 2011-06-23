--- /usr/src/contrib/bind9/lib/isc/arm/include/isc/atomic.h.orig	2010-08-04 02:02:01.194401084 -0400
+++ /usr/src/contrib/bind9/lib/isc/arm/include/isc/atomic.h	2010-08-04 02:04:53.462379414 -0400
@@ -49,26 +49,22 @@
 static inline isc_int32_t
 isc_atomic_cmpxchg(isc_int32_t *p, isc_int32_t cmpval, isc_int32_t val)
 {
-	register int done, ras_start;
+	register int done, ras_start  = 0xffff1004;

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

