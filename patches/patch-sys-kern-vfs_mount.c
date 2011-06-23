--- /usr/src/sys/kern/vfs_mount.c.orig	2010-07-04 22:50:00.613726077 -0400
+++ /usr/src/sys/kern/vfs_mount.c	2010-07-05 12:11:09.986561693 -0400
@@ -1651,6 +1651,9 @@
 	int error, i, asked = 0;

 	options = NULL;
+
+	/* NASTY HACK: wait for USB sticks to appear */
+	pause("usbhack", hz * 10);

 	root_mount_prepare();

