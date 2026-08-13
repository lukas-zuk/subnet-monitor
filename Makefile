include $(TOPDIR)/rules.mk

PKG_NAME:=subnet_monitor
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/subnet_monitor
	CATEGORY:=Base system
	TITLE:=subnet_monitor
	DEPENDS=+libubox +libubus +libblobmsg-json

endef

define Package/subnet_monitor/description
	Package that monitors for duplicate interface subnets and reports to ubus
endef

define Package/subnet_monitor/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/subnet_monitor $(1)/usr/bin
	$(INSTALL_BIN) ./files/subnet_monitor.init $(1)/etc/init.d/subnet_monitor
endef

$(eval $(call BuildPackage,subnet_monitor))

