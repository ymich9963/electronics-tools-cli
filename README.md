# Trace Width Calculator (TWC)
![badge](https://badgen.net/badge/Coverage/100/blue) 
<a href="https://scan.coverity.com/projects/ymich9963-electronics-tools-cli">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/31007/badge.svg"/>
</a>

A calculator tool used to assist designing rigid PCB traces. Implements both the IPC2221 and IPC2152 standards and has the ability to choose between the two, with the latter having two methods available for the outputs. Each method used for the standards was derived from one of the various websites that claim to implement a trace width calculator.

## Methods
One method is implemented for IPC2221 and two methods for IPC2152. The tool defaults to the IPC2152 standard, Method A.

### IPC2221
- Method A: Derived from [The CircuitCalculator.com Blog](https://circuitcalculator.com/wordpress/2006/01/31/pcb-trace-width-calculator/) and [Omni Calculator](https://www.omnicalculator.com/other/pcb-trace-width#how-to-calculate-the-width-of-a-pcb-trace). 

### IPC2152
- Method A: Derived from [NinjaCalc](https://ninjacalc.mbedded.ninja/calculators/electronics/pcb-design/track-current-ipc2152).
- Method B: Derived from [SMPS.us](https://www.smps.us/pcb-calculator.html) which was also posted on the [Altium Resources](https://resources.altium.com/p/using-ipc-2152-calculator-designing-standards)


## Installing
To install and use the `twc` command. Make sure to have the latest version, place it in a folder, and add that folder path to your Enviroment Variables.

## Building
Clone the repository and got to this directory. Using `make` simply run in a command line,
```
make
```
And `twc` should be build in a new `build/` directory.

## Tests and Coverage
Running the tests or coverage can be done by running,

```
make test
```
```
make coverage
```
Testing suite used is [Unity](https://github.com/ThrowTheSwitch/Unity) and LLVM-COV for coverage.

## Sources
Some sources for creating the tool are listed below, 
- Trace Width Calculators,
	- [Advanced PCB](https://www.advancedpcb.com/en-us/tools/trace-width-calculator/)
	- [Circuit Calculator](https://circuitcalculator.com/wordpress/2006/01/31/pcb-trace-width-calculator/)
	- [Omnicalculator](https://www.omnicalculator.com/other/pcb-trace-width#how-to-calculate-the-width-of-a-pcb-trace)
	- [SMP.us](https://www.smps.us/pcb-calculator.html)
	- [NinjaCalc](https://ninjacalc.mbedded.ninja/calculators/electronics/pcb-design/track-current-ipc2152)
- Conversion calculators,
	- [Convert-me](https://www.convert-me.com/en/convert/area/?u=mm2&v=0.0111) for area conversion
	- [UnitConverters](https://www.unitconverters.net/length/mil-to-millimeter.htm)
	- [Convert-measurement-units](https://www.convert-measurement-units.com/conversion-calculator-quick.php)
	- [Metric Conversions](https://www.metric-conversions.org/temperature/fahrenheit-to-celsius.htm) C to F conversion
- Other Resources
	- [Resistivity/Temperature Coefficients](https://www.engineeringtoolbox.com/resistivity-conductivity-d_418.html)
	- [Standard PCB Thickness – An Important Role in PCB Function](https://www.nextpcb.com/blog/standard-pcb-thickness)
	- [What is the Thermal Conductivity of Different PCBs](https://www.pcbonline.com/blog/pcb-thermal-conductivity.html)
	- [How Thick is 1 oz Copper](https://pcbprime.com/pcb-tips/how-thick-is-1oz-copper/)
	- [PCB Universe](https://www.pcbuniverse.com/pcbu-tech-tips.php?a=4)
	- [Resistivity and Temp coefficient source](http://hyperphysics.phy-astr.gsu.edu/hbase/Tables/rstiv.html)
