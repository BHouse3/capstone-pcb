# capstone-pcb

All files and documentation for PCB integration.

## Timeline

01/12/2026 - Created project repo and initial hierarchical KiCad structure. Found DFRobot's rp2350 module footprint.

01/13/2026 - Ethan created symbol for rp2350 module.

01/14/2026 - Created fuel gauge schematic with LC709203FQH-01TWG. Evaluating charging and voltage conversion methodology. 

01/15/2026 - Redesign of entire power subsystem. Utilizing TP4057 charger on board RP2350 module. DW01A + FS8205A protection circuitry. TPS61023 5V boost converter. BQ27441-G1 fuel gauge. Minimizes cost and number of components in power system. Ethan created L1 schematic with labels and wires for the symbol created on 01/13/2026. The symbol was also edited to correct labeling errors.

01/17/2026 - Power schematic starting point based on datasheet implementations. Will work to further verify. Changed MCU labels to global labels.

1/20/2026 - UI schematic finished. External connector symbols added. Hierarchical label issue. Washout detection schematic finished. Motor schematic finished.