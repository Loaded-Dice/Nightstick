# Nightstick
 Artistic LED toy - POV display style animated LEDs Arduino based code and DIY tutorial
 
 
Materials & parts needed to build the Nightstick:

  Acrylic Tube clear, length 50cm, 10/6 mm (outer/inner diameter)
  Silicone tube, length 50cm, inner diameter 10mm, thickness 1mm
  Transparent glue with white pigments / translucent white glue to fill the inner part of the acrylic tube after PCB insertion
  Pay attention to what glue or resin you use. 
	Most 2 component resins are very aggressive and begin dissolving the acrylic tube when put in the tube.
  	The resin should also not expand or contract when curing, 
  	should be UV resistant,
  	should not become yellow over time, 
  	should not react too exothermic when curing
  	Should be flexible enough to not break when the stick bends
  	But also robust and hard enough to support the structural integrity of the acrylic tube
	use a viscous glue to avoid air bubbles inside the stick
	
	
  I used UV special curing resin which worked good
  TPU-Sheet clear/transparent thickness >= 0.2mm  for the flowers
   (or Elastic clear/transparent rubber or silicone band)
  
   
  PCBs to order:
    1x Main PCB for XIAO nRF52 & SD card (not assembled by PCB manufacturer - further electronic parts needed, See list below) (PCB files are not panelized)
    8x Short LED strip PCB (assembled by PCB manufacturer) (PCB files are panelized - 8 short LED strips on one PCB)
    2x Main LED Strip PCB (assembled by PCB manufacturer)  (PCB files are panelized - 2 Main LED  strips on one PCB)
    4x Led Ring PCB (assembled by PCB manufacturer) 	(PCB files are not panelized - one PCB = one LED ring)
  
  Parts needed for factory side assembly: 
    Theoretically:
    364 pcs. SK6805-EC15 (footprint 1515 ) RGB LEDs  
    and almost same amount of 100nF capacitors (see BOM)

    But in reality, the minimum order amount of a PCB is 5 pcs (at JLCPCB)
    also when using JLCPBC you need to buy the LEDs before ordering the PCBs 

  Further electronic parts for one Nightstick:
    1x Seeed Studio XIAO nRF52840 BLE Sense
    1x Micro SD card
    1x Micro SD Card Socket (See PDF datasheet)
    1x Lithium Battery Charging Board with Battery Protection Dual Functions 5V 1A Type-C TP4056 HW-373 V1.0 TP4056
    1x LiPo Battery type 18350  (I used: Efest 18350 Gold, 900mAh, unprotected, flat anode) 
    2x 100nF Capacitor C0603
    1x 0.1µF Capacitor C0603
    1x 1µF electrolytic capacitor CAP-D5.0×F2.0
    1x 3-pin switch
    3x Push button (long cone)
    1x BSS138N or similar to raise LED data voltage level
    6x 10k SMD Resistor R0603
    Male & Female 2 Pin connector of your choice to connect the battery
    Some cables to wire everything up
    
  3D Files to print the housing
   1x  Socket Side A     	PLA Transparent/Clear
   1x  Socket Side B     	PLA Transparent/Clear
   1x  Core Side A       	PLA Transparent/Clear
   1x  Core Side B       	PLA Transparent/Clear
   2x  Flex Cover        	Recreus Filaflex 82A Clear (or similar flexible filament : Shore 82A)
   2x  Flower Anchor	 	PLA Transparent/Clear
   2x  Inner Ring        	PLA Transparent/Clear
  Missing:
   1x  Cover Plate Side A 	PLA Transparent/Clear
   1x  Cover Plate Side B 	PLA Transparent/Clear

  Other stuff needed
    copy SD Card Files (copy Nightstick folder and subfolders to your micro SD) 
