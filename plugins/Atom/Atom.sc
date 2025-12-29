// Atom.sc
// Music Universe with Atomic 3D Audio Space
// SwampMonster (liesurgic@site.com)

// AtomSpace: Simple container space for atoms
AtomSpace : UGen {
	*ar { |spaceSize = 10.0|
		/* Container for atoms in 3D space
		 * spaceSize: Size of the space cube (default 10.0)
		 * 
		 * Output: Control rate signal indicating number of active atoms
		 */
		^this.multiNew('audio', spaceSize);
	}
	
	checkInputs {
		^this.checkValidInputs;
	}
}

// Note: Base Atom class is now minimal and meant to be extended
// Concrete atom implementations should be derived classes with their own physics and behavior
