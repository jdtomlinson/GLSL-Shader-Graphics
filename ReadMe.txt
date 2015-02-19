About:
GLSL Shader Graphics Project is an example of different
GLSL based shaders and lighting techniques including
normal mapping, specular lighting, point lights, spot lights,
ambient lights, global lights, frame buffer objects (FBOs),
and Gaussian blur. It also includes newly developed engine
tools such as debug graphics tools and a developer console.
GLSL Shader Graphics Project was written in C++ with GLSL
shaders and was completed over a 2.5 month period.


Controls:
* 'W' Key: Move forward
* 'S' Key: Move backward
* 'A' Key: Strafe left
* 'D' Key: Strafe right
* 'E' Key: Move up
* 'Q' Key: Move down
* '1' Key: Set all lights scene
* '2' Key: Set point lights scene
* '3' Key: Set spotlights scene
* '4' Key: Set ambient lights scene
* '5' Key: Set global directional light scene
* '6' Key: Set simple single light
* Tilde Key: Open/close developer console
* Esc Key: Close developer console/program
* Mouse: Look up/down/left/right


Console Commands:
* Change shader type: "changeShader " +
	* "simple": simple shader with untooned texture
	* "grayscale": grayscale of cobblestone
	* "normalMap": complex multilight shader
	* "toon": Obama hope poster style shader
* Change FBO shader type: "changeFBO " +
	* "simple": nothing is changed
	* "blur": constant blur from any distance
* Change FBO blur degree: "changeBlurScale " + number value