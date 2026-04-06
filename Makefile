PROJECTS := \
	projects/smart-bms-firmware \
	projects/ota-bootloader-simulator \
	projects/can-telemetry-node

.PHONY: all test clean run-bms run-ota run-can

all:
	@for project in $(PROJECTS); do $(MAKE) -C $$project all; done

test:
	@for project in $(PROJECTS); do $(MAKE) -C $$project test; done

clean:
	@for project in $(PROJECTS); do $(MAKE) -C $$project clean; done

run-bms:
	@$(MAKE) -C projects/smart-bms-firmware run

run-ota:
	@$(MAKE) -C projects/ota-bootloader-simulator run

run-can:
	@$(MAKE) -C projects/can-telemetry-node run

