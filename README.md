arduino-led-dimmer
==================

A 16-channel Arduino compatible LED dimmer using the TLC 5940 PWM driver.

This is an exercise in designing a PCB and taking it through to manufacture.

This project is inadvertantly on hold for now, but I'd say it went pretty well for a first go. :D

Successes:

* I really enjoyed the experience - layout out PCBs is very therapeutic.
* It works like a charm under very specific conditions.
* I learnt a lot.

Faults identified:

* There is a bug in the layout where the ISET resistor is connected to +5vdc and not GND. Oopsies!
* The holes for the 7805 regulator IC are too small. I can't fit the regulator and have been using an off-board one.
* There is some other mystery-bug lurking, somewhere in power distribution I suspect. I think I'm lacking knowlege of a fundemental somewhere.
	* It runs fine when powered by the off-board regulator.
	* It does not run when powered by USB.
	* It _does_ run fine when powered by USB and the off-board regulator is connected but not powered.
	* Power appears to leak from USB even when the "Serial Power" jumper is removed. Wtf?

Things to try:

* Bodge in the voltage regulator circuit on-board. - This wouldn't tell me what's wrong per-se, but I'll be interested to see the result.
