--- if_axe.c.orig	2011-05-11 12:32:05.870559769 -0400
+++ if_axe.c	2011-05-11 12:30:42.000000000 -0400
@@ -31,7 +31,7 @@
  */
 
 #include <sys/cdefs.h>
-__FBSDID("$FreeBSD: src/sys/dev/usb/net/if_axe.c,v 1.12.2.14.2.1 2010/12/21 17:09:25 kensmith Exp $");
+__FBSDID("$FreeBSD: src/sys/dev/usb/net/if_axe.c,v 1.12.2.18 2011/01/14 22:36:33 yongari Exp $");
 
 /*
  * ASIX Electronics AX88172/AX88178/AX88778 USB 2.0 ethernet driver.
@@ -142,11 +142,11 @@
 	AXE_DEV(ASIX, AX88172, 0),
 	AXE_DEV(ASIX, AX88178, AXE_FLAG_178),
 	AXE_DEV(ASIX, AX88772, AXE_FLAG_772),
-	AXE_DEV(ASIX, AX88772A, AXE_FLAG_772),
+	AXE_DEV(ASIX, AX88772A, AXE_FLAG_772A),
 	AXE_DEV(ATEN, UC210T, 0),
 	AXE_DEV(BELKIN, F5D5055, AXE_FLAG_178),
 	AXE_DEV(BILLIONTON, USB2AR, 0),
-	AXE_DEV(CISCOLINKSYS, USB200MV2, AXE_FLAG_772),
+	AXE_DEV(CISCOLINKSYS, USB200MV2, AXE_FLAG_772A),
 	AXE_DEV(COREGA, FETHER_USB2_TX, 0),
 	AXE_DEV(DLINK, DUBE100, 0),
 	AXE_DEV(DLINK, DUBE100B1, AXE_FLAG_772),
@@ -191,6 +191,7 @@
 static int	axe_cmd(struct axe_softc *, int, int, int, void *);
 static void	axe_ax88178_init(struct axe_softc *);
 static void	axe_ax88772_init(struct axe_softc *);
+static void	axe_ax88772a_init(struct axe_softc *);
 static int	axe_get_phyno(struct axe_softc *, int);
 
 static const struct usb_config axe_config[AXE_N_TRANSFER] = {
@@ -199,7 +200,8 @@
 		.type = UE_BULK,
 		.endpoint = UE_ADDR_ANY,
 		.direction = UE_DIR_OUT,
-		.bufsize = AXE_BULK_BUF_SIZE,
+		.frames = 16,
+		.bufsize = 16 * MCLBYTES,
 		.flags = {.pipe_bof = 1,.force_short_xfer = 1,},
 		.callback = axe_bulk_write_callback,
 		.timeout = 10000,	/* 10 seconds */
@@ -302,7 +304,7 @@
 	axe_cmd(sc, AXE_CMD_MII_OPMODE_HW, 0, 0, NULL);
 
 	val = le16toh(val);
-	if ((sc->sc_flags & AXE_FLAG_772) != 0 && reg == MII_BMSR) {
+	if (AXE_IS_772(sc) && reg == MII_BMSR) {
 		/*
 		 * BMSR of AX88772 indicates that it supports extended
 		 * capability but the extended status register is
@@ -384,7 +386,7 @@
 	val = 0;
 	if ((IFM_OPTIONS(mii->mii_media_active) & IFM_FDX) != 0)
 		val |= AXE_MEDIA_FULL_DUPLEX;
-	if (sc->sc_flags & (AXE_FLAG_178 | AXE_FLAG_772)) {
+	if (AXE_IS_178_FAMILY(sc)) {
 		val |= AXE_178_MEDIA_RX_EN | AXE_178_MEDIA_MAGIC;
 		if ((sc->sc_flags & AXE_FLAG_178) != 0)
 			val |= AXE_178_MEDIA_ENCK;
@@ -536,8 +538,9 @@
 	}
 
 	if (bootverbose)
-		device_printf(sc->sc_ue.ue_dev, "EEPROM data : 0x%04x\n",
-		    eeprom);
+		device_printf(sc->sc_ue.ue_dev,
+		    "EEPROM data : 0x%04x, phymode : 0x%02x\n", eeprom,
+		    phymode);
 	/* Program GPIOs depending on PHY hardware. */
 	switch (phymode) {
 	case AXE_PHY_MODE_MARVELL:
@@ -554,6 +557,8 @@
 			    AXE_GPIO1_EN, hz / 32);
 		break;
 	case AXE_PHY_MODE_CICADA:
+	case AXE_PHY_MODE_CICADA_V2:
+	case AXE_PHY_MODE_CICADA_V2_ASIX:
 		if (gpio0 == 1)
 			AXE_GPIO_WRITE(AXE_GPIO_RELOAD_EEPROM | AXE_GPIO0 |
 			    AXE_GPIO0_EN, hz / 32);
@@ -610,7 +615,6 @@
 
 	axe_cmd(sc, AXE_CMD_RXCTL_WRITE, 0, 0, NULL);
 }
-#undef	AXE_GPIO_WRITE
 
 static void
 axe_ax88772_init(struct axe_softc *sc)
@@ -654,6 +658,47 @@
 }
 
 static void
+axe_ax88772a_init(struct axe_softc *sc)
+{
+	struct usb_ether *ue;
+	uint16_t eeprom;
+
+	ue = &sc->sc_ue;
+	axe_cmd(sc, AXE_CMD_SROM_READ, 0, 0x0017, &eeprom);
+	eeprom = le16toh(eeprom);
+	/* Reload EEPROM. */
+	AXE_GPIO_WRITE(AXE_GPIO_RELOAD_EEPROM, hz / 32);
+	if (sc->sc_phyno == AXE_772_PHY_NO_EPHY) {
+		/* Manually select internal(embedded) PHY - MAC mode. */
+		axe_cmd(sc, AXE_CMD_SW_PHY_SELECT, 0, AXE_SW_PHY_SELECT_SS_ENB |
+		    AXE_SW_PHY_SELECT_EMBEDDED | AXE_SW_PHY_SELECT_SS_MII,
+		    NULL);
+		uether_pause(&sc->sc_ue, hz / 32);
+	} else {
+		/*
+		 * Manually select external PHY - MAC mode.
+		 * Reverse MII/RMII is for AX88772A PHY mode.
+		 */
+		axe_cmd(sc, AXE_CMD_SW_PHY_SELECT, 0, AXE_SW_PHY_SELECT_SS_ENB |
+		    AXE_SW_PHY_SELECT_EXT | AXE_SW_PHY_SELECT_SS_MII, NULL);
+		uether_pause(&sc->sc_ue, hz / 32);
+	}
+	/* Take PHY out of power down. */
+	axe_cmd(sc, AXE_CMD_SW_RESET_REG, 0, AXE_SW_RESET_IPPD |
+	    AXE_SW_RESET_IPRL, NULL);
+	uether_pause(&sc->sc_ue, hz / 4);
+	axe_cmd(sc, AXE_CMD_SW_RESET_REG, 0, AXE_SW_RESET_IPRL, NULL);
+	uether_pause(&sc->sc_ue, hz);
+	axe_cmd(sc, AXE_CMD_SW_RESET_REG, 0, AXE_SW_RESET_CLEAR, NULL);
+	uether_pause(&sc->sc_ue, hz / 32);
+	axe_cmd(sc, AXE_CMD_SW_RESET_REG, 0, AXE_SW_RESET_IPRL, NULL);
+	uether_pause(&sc->sc_ue, hz / 32);
+	axe_cmd(sc, AXE_CMD_RXCTL_WRITE, 0, 0, NULL);
+}
+
+#undef	AXE_GPIO_WRITE
+
+static void
 axe_reset(struct axe_softc *sc)
 {
 	struct usb_config_descriptor *cd;
@@ -668,6 +713,14 @@
 
 	/* Wait a little while for the chip to get its brains in order. */
 	uether_pause(&sc->sc_ue, hz / 100);
+
+	/* Reinitialize controller to achieve full reset. */
+	if (sc->sc_flags & AXE_FLAG_178)
+		axe_ax88178_init(sc);
+	else if (sc->sc_flags & AXE_FLAG_772)
+		axe_ax88772_init(sc);
+	else if (sc->sc_flags & AXE_FLAG_772A)
+		axe_ax88772a_init(sc);
 }
 
 static void
@@ -691,15 +744,21 @@
 		sc->sc_phyno = 0;
 	}
 
-	if (sc->sc_flags & AXE_FLAG_178)
+	if (sc->sc_flags & AXE_FLAG_178) {
 		axe_ax88178_init(sc);
-	else if (sc->sc_flags & AXE_FLAG_772)
+		sc->sc_tx_bufsz = 16 * 1024;
+	} else if (sc->sc_flags & AXE_FLAG_772) {
 		axe_ax88772_init(sc);
+		sc->sc_tx_bufsz = 8 * 1024;
+	} else if (sc->sc_flags & AXE_FLAG_772A) {
+		axe_ax88772a_init(sc);
+		sc->sc_tx_bufsz = 8 * 1024;
+	}
 
 	/*
 	 * Get station address.
 	 */
-	if (sc->sc_flags & (AXE_FLAG_178 | AXE_FLAG_772))
+	if (AXE_IS_178_FAMILY(sc))
 		axe_cmd(sc, AXE_178_CMD_READ_NODEID, 0, 0, ue->ue_eaddr);
 	else
 		axe_cmd(sc, AXE_172_CMD_READ_NODEID, 0, 0, ue->ue_eaddr);
@@ -707,7 +766,13 @@
 	/*
 	 * Fetch IPG values.
 	 */
-	axe_cmd(sc, AXE_CMD_READ_IPG012, 0, 0, sc->sc_ipgs);
+	if (sc->sc_flags & AXE_FLAG_772A) {
+		/* Set IPG values. */
+		sc->sc_ipgs[0] = 0x15;
+		sc->sc_ipgs[1] = 0x16;
+		sc->sc_ipgs[2] = 0x1A;
+	} else
+		axe_cmd(sc, AXE_CMD_READ_IPG012, 0, 0, sc->sc_ipgs);
 }
 
 /*
@@ -810,7 +875,7 @@
 		err = 0;
 
 		pc = usbd_xfer_get_frame(xfer, 0);
-		if (sc->sc_flags & (AXE_FLAG_772 | AXE_FLAG_178)) {
+		if (AXE_IS_178_FAMILY(sc)) {
 			while (pos < actlen) {
 				if ((pos + sizeof(hdr)) > actlen) {
 					/* too little data */
@@ -875,7 +940,7 @@
 	struct ifnet *ifp = uether_getifp(&sc->sc_ue);
 	struct usb_page_cache *pc;
 	struct mbuf *m;
-	int pos;
+	int nframes, pos;
 
 	switch (USB_GET_STATE(xfer)) {
 	case USB_ST_TRANSFERRED:
@@ -892,40 +957,34 @@
 			 */
 			return;
 		}
-		pos = 0;
-		pc = usbd_xfer_get_frame(xfer, 0);
-
-		while (1) {
 
+		for (nframes = 0; nframes < 16 &&
+		    !IFQ_DRV_IS_EMPTY(&ifp->if_snd); nframes++) {
 			IFQ_DRV_DEQUEUE(&ifp->if_snd, m);
-
-			if (m == NULL) {
-				if (pos > 0)
-					break;	/* send out data */
-				return;
-			}
-			if (m->m_pkthdr.len > MCLBYTES) {
-				m->m_pkthdr.len = MCLBYTES;
-			}
-			if (sc->sc_flags & (AXE_FLAG_772 | AXE_FLAG_178)) {
-
+			if (m == NULL)
+				break;
+			usbd_xfer_set_frame_offset(xfer, nframes * MCLBYTES,
+			    nframes);
+			pos = 0;
+			pc = usbd_xfer_get_frame(xfer, nframes);
+			if (AXE_IS_178_FAMILY(sc)) {
 				hdr.len = htole16(m->m_pkthdr.len);
 				hdr.ilen = ~hdr.len;
-
 				usbd_copy_in(pc, pos, &hdr, sizeof(hdr));
-
 				pos += sizeof(hdr);
-
-				/*
-				 * NOTE: Some drivers force a short packet
-				 * by appending a dummy header with zero
-				 * length at then end of the USB transfer.
-				 * This driver uses the
-				 * USB_FORCE_SHORT_XFER flag instead.
-				 */
+				usbd_m_copy_in(pc, pos, m, 0, m->m_pkthdr.len);
+				pos += m->m_pkthdr.len;
+				if ((pos % 512) == 0) {
+					hdr.len = 0;
+					hdr.ilen = 0xffff;
+					usbd_copy_in(pc, pos, &hdr,
+					    sizeof(hdr));
+					pos += sizeof(hdr);
+				}
+			} else {
+				usbd_m_copy_in(pc, pos, m, 0, m->m_pkthdr.len);
+				pos += m->m_pkthdr.len;
 			}
-			usbd_m_copy_in(pc, pos, m, 0, m->m_pkthdr.len);
-			pos += m->m_pkthdr.len;
 
 			/*
 			 * XXX
@@ -946,22 +1005,16 @@
 
 			m_freem(m);
 
-			if (sc->sc_flags & (AXE_FLAG_772 | AXE_FLAG_178)) {
-				if (pos > (AXE_BULK_BUF_SIZE - MCLBYTES - sizeof(hdr))) {
-					/* send out frame(s) */
-					break;
-				}
-			} else {
-				/* send out frame */
-				break;
-			}
+			/* Set frame length. */
+			usbd_xfer_set_frame_len(xfer, nframes, pos);
+		}
+		if (nframes != 0) {
+			usbd_xfer_set_frames(xfer, nframes);
+			usbd_transfer_submit(xfer);
+			ifp->if_drv_flags |= IFF_DRV_OACTIVE;
 		}
-
-		usbd_xfer_set_frame_len(xfer, 0, pos);
-		usbd_transfer_submit(xfer);
-		ifp->if_drv_flags |= IFF_DRV_OACTIVE;
 		return;
-
+		/* NOTREACHED */
 	default:			/* Error */
 		DPRINTFN(11, "transfer error, %s\n",
 		    usbd_errstr(error));
@@ -1016,20 +1069,25 @@
 
 	AXE_LOCK_ASSERT(sc, MA_OWNED);
 
+	if ((ifp->if_drv_flags & IFF_DRV_RUNNING) != 0)
+		return;
+
 	/* Cancel pending I/O */
 	axe_stop(ue);
 
+	axe_reset(sc);
+
 	/* Set MAC address. */
-	if (sc->sc_flags & (AXE_FLAG_178 | AXE_FLAG_772))
+	if (AXE_IS_178_FAMILY(sc))
 		axe_cmd(sc, AXE_178_CMD_WRITE_NODEID, 0, 0, IF_LLADDR(ifp));
 	else
 		axe_cmd(sc, AXE_172_CMD_WRITE_NODEID, 0, 0, IF_LLADDR(ifp));
 
 	/* Set transmitter IPG values */
-	if (sc->sc_flags & (AXE_FLAG_178 | AXE_FLAG_772)) {
+	if (AXE_IS_178_FAMILY(sc))
 		axe_cmd(sc, AXE_178_CMD_WRITE_IPG012, sc->sc_ipgs[2],
 		    (sc->sc_ipgs[1] << 8) | (sc->sc_ipgs[0]), NULL);
-	} else {
+	else {
 		axe_cmd(sc, AXE_172_CMD_WRITE_IPG0, 0, sc->sc_ipgs[0], NULL);
 		axe_cmd(sc, AXE_172_CMD_WRITE_IPG1, 0, sc->sc_ipgs[1], NULL);
 		axe_cmd(sc, AXE_172_CMD_WRITE_IPG2, 0, sc->sc_ipgs[2], NULL);
@@ -1037,7 +1095,7 @@
 
 	/* Enable receiver, set RX mode */
 	rxmode = (AXE_RXCMD_MULTICAST | AXE_RXCMD_ENABLE);
-	if (sc->sc_flags & (AXE_FLAG_178 | AXE_FLAG_772)) {
+	if (AXE_IS_178_FAMILY(sc)) {
 #if 0
 		rxmode |= AXE_178_RXCMD_MFB_2048;	/* chip default */
 #else
@@ -1066,6 +1124,8 @@
 	usbd_xfer_set_stall(sc->sc_xfer[AXE_BULK_DT_WR]);
 
 	ifp->if_drv_flags |= IFF_DRV_RUNNING;
+	/* Switch to selected media. */
+	axe_ifmedia_upd(ifp);
 	axe_start(ue);
 }
 
@@ -1107,6 +1167,4 @@
 	 */
 	usbd_transfer_stop(sc->sc_xfer[AXE_BULK_DT_WR]);
 	usbd_transfer_stop(sc->sc_xfer[AXE_BULK_DT_RD]);
-
-	axe_reset(sc);
 }
