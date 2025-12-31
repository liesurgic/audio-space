// Sylenth.sc
// Sylenth1 VST wrapper for easy use in SuperCollider
// AudioSpace (cp.potter@proton.me)

// Note: This requires the VSTPlugin extension to be installed
// Install from: https://github.com/Spacechild1/vstplugin

Sylenth {
	classvar <>defaultPath = "/Library/Audio/Plug-Ins/VST3/Sylenth1.vst3";
	classvar <>vst2Path = "/Library/Audio/Plug-Ins/VST/Sylenth1.vst"; // Alternative VST2 path
	
	*ar { |in, path|
		/* Load Sylenth1 VST plugin
		 * in: Input signal (nil = no input for instrument use, or audio rate signal for effect use)
		 * path: Path to Sylenth1.vst3 file (default: /Library/Audio/Plug-Ins/VST3/Sylenth1.vst3)
		 * 
		 * Returns VSTPlugin.ar node
		 * 
		 * Usage:
		 *   // Load as instrument (no input)
		 *   var sig = Sylenth.ar; // or Sylenth.ar(nil)
		 *   
		 *   // Or use in SynthDef
		 *   SynthDef(\sylenth, {
		 *     var sig = Sylenth.ar;
		 *     Out.ar(0, sig);
		 *   }).add;
		 */
		var input = in; // nil for no input, or audio rate signal
		var channels = if(input.isNil, 2, { input.numChannels });
		
		^VSTPlugin.ar(input, channels, id: \sylenth);
	}
	
	*kr { |in, path|
		/* Control rate version (rarely used for VST instruments) */
		^this.ar(in, path);
	}
	
	*new { |server, path|
		/* Create a Synth with Sylenth1
		 * server: Server instance (default: Server.default)
		 * path: Path to Sylenth1.vst3 file
		 * 
		 * Returns Synth instance
		 * 
		 * Usage:
		 *   ~synth = Sylenth.new;
		 *   ~vst = VSTPluginController(~synth);
		 *   ~vst.open(path: Sylenth.defaultPath);
		 *   ~vst.editor; // Open GUI
		 */
		server = server ?? Server.default;
		
		^Synth(\sylenth_synth, target: server);
	}
	
	*initClass {
		// Register default SynthDef
		StartUp.add {
			SynthDef(\sylenth_synth, { |out = 0|
				var sig = Sylenth.ar;
				Out.ar(out, sig);
			}).add;
		}
	}
	
	*findPaths {
		/* Try to find available Sylenth1 installations
		 * Returns array of available paths
		 */
		var paths, vst3Path, vst2Path, auPath;
		paths = [];
		vst3Path = "/Library/Audio/Plug-Ins/VST3/Sylenth1.vst3";
		vst2Path = "/Library/Audio/Plug-Ins/VST/Sylenth1.vst";
		auPath = "/Library/Audio/Plug-Ins/Components/Sylenth1.component";
		
		if(File.exists(vst3Path), { paths = paths.add(vst3Path) });
		if(File.exists(vst2Path), { paths = paths.add(vst2Path) });
		if(File.exists(auPath), { paths = paths.add(auPath) });
		
		^paths;
	}
}

// Convenience class for managing Sylenth1 instances
SylenthController {
	var <synth, <vst, <path;
	
	*new { |synth, path|
		/* Create a controller for Sylenth1
		 * synth: Synth instance containing Sylenth
		 * path: Path to Sylenth1.vst3 (default: Sylenth.defaultPath)
		 * 
		 * Usage:
		 *   ~sylenth = SylenthController.new(Synth(\sylenth_synth));
		 *   ~sylenth.open; // Opens the plugin
		 *   ~sylenth.editor; // Opens GUI
		 *   ~sylenth.midi.noteOn(0, 60, 100); // Play note
		 */
		^super.new.init(synth, path);
	}
	
	init { |synthArg, pathArg|
		var paths;
		synth = synthArg;
		path = pathArg;
		
		// If no path specified, try to find available paths
		if(path.isNil) {
			paths = Sylenth.findPaths;
			if(paths.size > 0) {
				path = paths[0]; // Use first available path
				"Using Sylenth1 at: %".format(path).postln;
			} {
				path = Sylenth.defaultPath;
				"Warning: Using default path, may not exist: %".format(path).postln;
			}
		};
		
		vst = VSTPluginController(synth);
	}
	
	open { |action|
		/* Open Sylenth1 plugin
		 * action: Function to execute after opening
		 */
		if(synth.isPlaying, {
			vst.open(path: path, action: {
				"Sylenth1 loaded successfully".postln;
				action.value;
			});
		}, {
			"Error: Synth is not playing, cannot open plugin".postln;
			action.value; // Still call action to prevent blocking
		});
	}
	
	isLoaded {
		/* Check if the plugin is loaded */
		^vst.isLoaded;
	}
	
	editor {
		/* Open Sylenth1 GUI editor */
		if(vst.isLoaded, {
			vst.editor;
		}, {
			"Warning: VST plugin is not loaded yet, cannot open editor".postln;
		});
	}
	
	close {
		/* Close Sylenth1 plugin */
		vst.close;
	}
	
	free {
		/* Free the synth and close plugin */
		vst.close;
		synth.free;
	}
	
	// Accessors
	midi {
		/* Return the VSTPluginMIDIProxy for direct MIDI access */
		^vst.midi;
	}
	
	// MIDI convenience methods
	noteOn { |channel = 0, note = 60, velocity = 100|
		if(synth.isPlaying, {
			if(vst.isLoaded, {
				vst.midi.noteOn(channel, note, velocity);
			}, {
				"Warning: VST plugin is not loaded yet, cannot send MIDI".postln;
			});
		}, {
			"Warning: Synth is not playing, cannot send MIDI".postln;
		});
	}
	
	noteOff { |channel = 0, note = 60|
		if(synth.isPlaying, {
			if(vst.isLoaded, {
				vst.midi.noteOff(channel, note);
			}, {
				"Warning: VST plugin is not loaded yet, cannot send MIDI".postln;
			});
		}, {
			"Warning: Synth is not playing, cannot send MIDI".postln;
		});
	}
	
	set { |param, value|
		/* Set parameter by name or index
		 * param: Parameter name (Symbol) or index (Integer)
		 * value: Parameter value (0-1)
		 */
		if(vst.isLoaded, {
			vst.set(param, value);
		}, {
			"Warning: VST plugin is not loaded yet, cannot set parameter".postln;
		});
	}
	
	get { |param|
		/* Get parameter value */
		if(vst.isLoaded, {
			^vst.get(param);
		}, {
			"Warning: VST plugin is not loaded yet, cannot get parameter".postln;
			^nil;
		});
	}
}
