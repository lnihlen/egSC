DuffingOsc : UGen {
	*ar { |freq = 440, amp = 1.0, damping = 0.1, stiffness = 0.5, nonLinearity = 0.5|
		^this.multiNew('audio', freq, amp, damping, stiffness, nonLinearity);
	}
}

DuffingExt : UGen {
	*ar { |in, damping = 0.1, stiffness = 0.5, nonLinearity = 0.5|
		^this.multiNew('audio', in, damping, stiffness, nonLinearity);
	}
}