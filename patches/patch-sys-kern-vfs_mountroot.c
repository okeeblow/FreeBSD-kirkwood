--- sys/kern/vfs_mountroot.c.orig	2012-04-06 16:29:46.584814596 -0400
+++ sys/kern/vfs_mountroot.c	2012-04-06 16:29:52.559814429 -0400
@@ -911,6 +911,8 @@
 	struct root_hold_token *h;
 	struct timeval lastfail;
 	int curfail;
+	
+	pause("usbhack", hz * 10);
 
 	curfail = 0;
 	while (1) {
