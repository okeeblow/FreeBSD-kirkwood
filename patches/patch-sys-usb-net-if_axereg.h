--- if_axereg.h.orig	2011-05-11 12:32:16.905422511 -0400
+++ if_axereg.h	2011-05-11 12:30:54.000000000 -0400
@@ -29,7 +29,7 @@
  * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  * THE POSSIBILITY OF SUCH DAMAGE.
  *
- * $FreeBSD: src/sys/dev/usb/net/if_axereg.h,v 1.3.2.3.2.1 2010/12/21 17:09:25 kensmith Exp $
+ * $FreeBSD: src/sys/dev/usb/net/if_axereg.h,v 1.3.2.5 2011/01/14 22:33:12 yongari Exp $
  */
 
 /*
@@ -92,6 +92,10 @@
 #define	AXE_CMD_SW_PHY_STATUS			0x0021
 #define	AXE_CMD_SW_PHY_SELECT			0x0122
 
+/* AX88772A and AX88772B only. */
+#define	AXE_CMD_READ_VLAN_CTRL			0x4027
+#define	AXE_CMD_WRITE_VLAN_CTRL			0x4028
+
 #define	AXE_SW_RESET_CLEAR			0x00
 #define	AXE_SW_RESET_RR				0x01
 #define	AXE_SW_RESET_RT				0x02
@@ -172,6 +176,21 @@
 #define	AXE_PHY_MODE_REALTEK_8251CL	0x0E
 #define	AXE_PHY_MODE_ATTANSIC		0x40
 
+/* AX88772A only. */
+#define	AXE_SW_PHY_SELECT_EXT		0x0000
+#define	AXE_SW_PHY_SELECT_EMBEDDED	0x0001
+#define	AXE_SW_PHY_SELECT_AUTO		0x0002
+#define	AXE_SW_PHY_SELECT_SS_MII	0x0004
+#define	AXE_SW_PHY_SELECT_SS_RVRS_MII	0x0008
+#define	AXE_SW_PHY_SELECT_SS_RVRS_RMII	0x000C
+#define	AXE_SW_PHY_SELECT_SS_ENB	0x0010
+
+/* AX88772A/AX88772B VLAN control. */
+#define	AXE_VLAN_CTRL_ENB		0x00001000
+#define	AXE_VLAN_CTRL_STRIP		0x00002000
+#define	AXE_VLAN_CTRL_VID1_MASK		0x00000FFF
+#define	AXE_VLAN_CTRL_VID2_MASK		0x0FFF0000
+
 #define	AXE_BULK_BUF_SIZE	16384	/* bytes */
 
 #define	AXE_CTL_READ		0x01
@@ -203,12 +222,22 @@
 	int			sc_flags;
 #define	AXE_FLAG_LINK		0x0001
 #define	AXE_FLAG_772		0x1000	/* AX88772 */
-#define	AXE_FLAG_178		0x2000	/* AX88178 */
+#define	AXE_FLAG_772A		0x2000	/* AX88772A */
+#define	AXE_FLAG_772B		0x4000	/* AX88772B */
+#define	AXE_FLAG_178		0x8000	/* AX88178 */
 
 	uint8_t			sc_ipgs[3];
 	uint8_t			sc_phyaddrs[2];
+	int			sc_tx_bufsz;
 };
 
+#define	AXE_IS_178_FAMILY(sc)						  \
+	((sc)->sc_flags & (AXE_FLAG_772 | AXE_FLAG_772A | AXE_FLAG_772B | \
+	AXE_FLAG_178))
+
+#define	AXE_IS_772(sc)							  \
+	((sc)->sc_flags & (AXE_FLAG_772 | AXE_FLAG_772A | AXE_FLAG_772B))
+
 #define	AXE_LOCK(_sc)		mtx_lock(&(_sc)->sc_mtx)
 #define	AXE_UNLOCK(_sc)		mtx_unlock(&(_sc)->sc_mtx)
 #define	AXE_LOCK_ASSERT(_sc, t)	mtx_assert(&(_sc)->sc_mtx, t)
