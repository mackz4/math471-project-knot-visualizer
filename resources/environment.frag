#version 330

// Fragment shader

// These get passed in from the vertex shader and are interpolated (varying) properties,
// so they change for each pixel across the triangle:
in vec4 interpSurfPosition; // position on the surface in world coordinates
in vec3 interpSurfNormal; // normal in world coordinates
in vec2 texCoord;


//Textures
uniform samplerCube environmentMap;
uniform vec3 materialColor;

// Material Properties
uniform vec3 ambientReflectionCoeff;
uniform vec3 diffuseReflectionCoeff;
uniform vec3 specularReflectionCoeff;
 
uniform float specularExponent;
 
uniform vec3 ambientLightIntensity;
uniform vec3 diffuseLightIntensity;
uniform vec3 specularLightIntensity;

uniform vec3 eye_world;

uniform vec3 light_direction; // in world coordinates
uniform float component_mix;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

void main() {
    vec3 finalColor = vec3(0.0, 0.0, 0.0);

    // TODO: Calculate ambient, diffuse, and specular lighting for this pixel based on its position, normal, etc.
    vec3 n = normalize(interpSurfNormal);
    //vec3 u = -light_direction;
    vec3 u = vec3(0.0, 1.0, 0.0);
    vec3 v = normalize(eye_world - (interpSurfPosition.xyz)); 
    vec3 h = normalize(u + v);

    vec3 ambientLighting = ambientReflectionCoeff * ambientLightIntensity;
    vec3 diffuseLighting = diffuseReflectionCoeff * diffuseLightIntensity * clamp(dot(n, u), 0.0, 1.0);
    vec3 specularLighting = specularReflectionCoeff * specularLightIntensity * pow(clamp(dot(n, h), 0.0, 1.0), specularExponent);

    finalColor += ambientLighting + diffuseLighting + specularLighting;

	// Tell OpenGL to use the r,g,b compenents of finalColor for the color of this fragment (pixel).
	fragColor.rgb = mix(materialColor, finalColor, component_mix);

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}