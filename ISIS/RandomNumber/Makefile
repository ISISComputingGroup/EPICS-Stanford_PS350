#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), configure)
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *App))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocBoot))

define DIR_template
 $(1)_DEPEND_DIRS = configure
endef
$(foreach dir, $(filter-out configure,$(DIRS)),$(eval $(call DIR_template,$(dir))))

iocBoot_DEPEND_DIRS += $(filter %App,$(DIRS))

include $(TOP)/configure/RULES_TOP

kit : RandomIOC.msi RandomIOC-v2.msi
	copy RandomIOC.msi "\\ndhNUC\C$$\Users\Administrator\Desktop\Test Kits"
	copy RandomIOC-v2.msi "\\ndhNUC\C$$\Users\Administrator\Desktop\Test Kits"

RandomIOC.msi : RandomIOC.Wixobj
	"c:\Program Files (x86)\WiX Toolset v3.7\bin\light.exe" RandomIOC.Wixobj

RandomIOC.wixobj : RandomIOC.Wxs
	"c:\Program Files (x86)\WiX Toolset v3.7\bin\candle.exe" RandomIOC.Wxs

RandomIOC-v2.msi : RandomIOC-v2.Wixobj
	"c:\Program Files (x86)\WiX Toolset v3.7\bin\light.exe" -ext "c:\Program Files (x86)\WiX Toolset v3.7\bin\WixFirewallExtension.dll" RandomIOC-v2.Wixobj

RandomIOC-v2.wixobj : RandomIOC-v2.Wxs
	"c:\Program Files (x86)\WiX Toolset v3.7\bin\candle.exe" -ext "c:\Program Files (x86)\WiX Toolset v3.7\bin\WixFirewallExtension.dll" RandomIOC-v2.Wxs

	