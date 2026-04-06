PROJECTS := \
	projects/smart-bms-firmware \
	projects/ota-bootloader-simulator \
	projects/can-telemetry-node \
	projects/motor-condition-monitor \
	projects/resilient-flash-journal \
	projects/energy-harvesting-node

.PHONY: all test clean run-bms run-ota run-can run-motor run-journal run-power

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

run-motor:
	@$(MAKE) -C projects/motor-condition-monitor run

run-journal:
	@$(MAKE) -C projects/resilient-flash-journal run

run-power:
	@$(MAKE) -C projects/energy-harvesting-node run
