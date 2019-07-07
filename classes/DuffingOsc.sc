DuffingOsc : UGen {
	*ar { |freq = 440, damping = 0.1, stiffness = 0.5, nonLinearity = 0.5, reset = 0, useAdvancedIntegrator = 0|
		^this.multiNew('audio', freq, damping, stiffness, nonLinearity, reset, useAdvancedIntegrator);
	}
}
