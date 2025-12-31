// Womp.sc
// Womp atom implementation
// AudioSpace (cp.potter@proton.me)

Womp : UGen {
	*ar { |x = 0.0, y = 0.0, z = 0.0, vx = 0.0, vy = 0.0, vz = 0.0, radius = 0.8, kickFreq = 60.0, bassFreq = 110.0, amp = 0.3, bpm = 120.0, kickDecay = 0.15, modDepth = 0.1, distortion = 0.3, compAttack = 0.01, compRelease = 0.1, sidechainAmount = 0.8|
		/* Womp atom - combines Kick and Bassline with synchronized peaks and sidechain compression
		 * x, y, z: 3D position coordinates (control rate, default 0.0)
		 * vx, vy, vz: 3D velocity components (control rate, default 0.0)
		 * radius: Collision radius (control rate, default 0.8)
		 * kickFreq: Kick pitch in Hz (control rate, default 60.0)
		 * bassFreq: Bassline base frequency in Hz (control rate, default 110.0)
		 * amp: Audio amplitude (control rate, default 0.3)
		 * bpm: Beats per minute - synchronizes kick and bassline peaks (control rate, default 120.0)
		 * kickDecay: Kick envelope decay time in seconds (control rate, default 0.15)
		 * modDepth: Bassline frequency modulation depth (0-1, control rate, default 0.1)
		 * distortion: Bassline distortion amount at peaks (0-1, control rate, default 0.3)
		 * compAttack: Sidechain compressor attack time in seconds (control rate, default 0.01)
		 * compRelease: Sidechain compressor release time in seconds (control rate, default 0.1)
		 * sidechainAmount: Amount of sidechain compression/ducking (0-1, control rate, default 0.8)
		 * 
		 * Output: Audio signal combining kick and bassline, with bassline sidechained to kick
		 */
		^this.multiNew('audio', x, y, z, vx, vy, vz, radius, kickFreq, bassFreq, amp, bpm, kickDecay, modDepth, distortion, compAttack, compRelease, sidechainAmount);
	}
	
	checkInputs {
		^this.checkValidInputs;
	}
}
