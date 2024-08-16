include $(dvr_rdk_PATH)/makerules/config.mk

package:
	gcc $(dvr_rdk_PATH)/scripts/package.c -o $(dvr_rdk_PATH)/scripts/package
	$(dvr_rdk_PATH)/scripts/package $(dvr_rdk_PATH)/ $(PLATFORM)



