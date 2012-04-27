--- sys/dev/mge/if_mge.c.orig	2012-04-06 16:40:27.158814586 -0400
+++ sys/dev/mge/if_mge.c	2012-04-06 16:40:41.582329613 -0400
@@ -1640,9 +1640,9 @@
 		}
 
 		if (etype != ETHERTYPE_IP) {
-			if_printf(sc->ifp,
+			/*if_printf(sc->ifp,
 			    "TCP/IP Offload enabled for unsupported "
-			    "protocol!\n");
+			    "protocol!\n");*/
 			return;
 		}
 
