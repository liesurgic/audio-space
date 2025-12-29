// Kick.sc
// Kick drum atom implementation
// AudioSpace (cp.potter@proton.me)

Kick : UGen {
	*ar { |x = 0.0, y = 0.0, z = 0.0, vx = 0.0, vy = 0.0, vz = 0.0, radius = 0.8, freq = 60.0, amp = 0.3, bpm = 120.0, beatsPerBar = 4.0, trig = 0.0|
		/* Kick drum atom in 3D space synchronized to BPM
		 * x, y, z: 3D position coordinates (control rate, default 0.0)
		 * vx, vy, vz: 3D velocity components (control rate, default 0.0)
		 * radius: Collision radius (control rate, default 0.8)
		 * freq: Kick pitch in Hz (control rate, default 60.0)
		 * amp: Audio amplitude (control rate, default 0.3)
		 * bpm: Beats per minute - synchronizes kick timing (control rate, default 120.0)
		 * beatsPerBar: Number of beats per bar (control rate, default 4.0)
		 * trig: Manual trigger input - rising edge triggers kick (control rate, default 0.0)
		 * 
		 * Output: Audio signal from the kick drum atom
		 */
		^this.multiNew('audio', x, y, z, vx, vy, vz, radius, freq, amp, bpm, beatsPerBar, trig);
	}
	
	checkInputs {
		^this.checkValidInputs;
	}
}
