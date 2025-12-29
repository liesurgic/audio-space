// Bassline.sc
// Bassline atom implementation
// AudioSpace (cp.potter@proton.me)

Bassline : UGen {
	*ar { |x = 0.0, y = 0.0, z = 0.0, vx = 0.0, vy = 0.0, vz = 0.0, radius = 0.6, freq = 110.0, amp = 0.2, bpm = 120.0, modDepth = 0.1, distortion = 0.3|
		/* Bassline atom in 3D space with frequency modulation synchronized to BPM
		 * x, y, z: 3D position coordinates (control rate, default 0.0)
		 * vx, vy, vz: 3D velocity components (control rate, default 0.0)
		 * radius: Collision radius (control rate, default 0.6)
		 * freq: Base frequency in Hz (control rate, default 110.0)
		 * amp: Audio amplitude (control rate, default 0.2)
		 * bpm: Beats per minute - synchronizes modulation rate (control rate, default 120.0)
		 * modDepth: Frequency modulation depth (0-1, control rate, default 0.1)
		 * distortion: Distortion amount at peaks (0-1, control rate, default 0.3)
		 * 
		 * Output: Audio signal from the bassline atom with frequency modulation
		 */
		^this.multiNew('audio', x, y, z, vx, vy, vz, radius, freq, amp, bpm, modDepth, distortion);
	}
	
	checkInputs {
		^this.checkValidInputs;
	}
}
