--- if_mge.c.orig	2011-05-11 14:27:51.692628521 -0400
+++ if_mge.c	2011-05-11 14:28:20.950873599 -0400
@@ -1607,9 +1607,9 @@
 		}
 
 		if (etype != ETHERTYPE_IP) {
-			if_printf(sc->ifp,
+			/*if_printf(sc->ifp,
 			    "TCP/IP Offload enabled for unsupported "
-			    "protocol!\n");
+			    "protocol!\n");*/
 			return;
 		}
 
