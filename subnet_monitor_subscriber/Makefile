include $(TOPDIR)/rules.mk

PKG_NAME:=subnet_monitor_sub
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/subnet_monitor_sub
	CATEGORY:=Base system
	TITLE:=subnet_monitor_sub
	DEPENDS=+libubox +libubus +libblobmsg-json

endef

define Package/subnet_monitor_sub/description
	Subscriber for subnet monitor
endef

define Package/subnet_monitor_sub/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/subnet_monitor_sub $(1)/usr/bin
	$(INSTALL_BIN) ./files/subnet_monitor_sub.init $(1)/etc/init.d/subnet_monitor_sub
endef

$(eval $(call BuildPackage,subnet_monitor_sub))

