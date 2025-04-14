#version 330

// Fragment shader


// These get passed in from the vertex shader and are interpolated (varying) properties,
// so they change for each pixel across the triangle:
in vec4 interpSurfPosition; // position on the surface in world coordinates
in vec3 interpSurfNormal; // normal in world coordinates

uniform vec3 light_direction; // in world coordinates

//Textures
uniform samplerCube environmentMap;

// Material Properties
uniform vec3 glassEta; // Contains one eta for each color channel (use eta.r, eta.g, eta.b in your code)
uniform float glassR0; // The Fresnel reflectivity when the incident angle is 0

uniform vec3 eye_world;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

void main() {

    vec3 N = normalize(interpSurfNormal);
    vec3 eyeDirection = normalize(eye_world - (interpSurfPosition.xyz));
    vec4 reflectedColor = texture(environmentMap, normalize(reflect(-1.0f*eyeDirection, N)));
    
    vec3 halfway = normalize(light_direction + eyeDirection);
    float theta = clamp(dot(eyeDirection, halfway), 0.0, 1.0);
    float f = glassR0 + (1 - glassR0)*pow(1-theta, 5.0);
    
    vec4 refractedR = texture(environmentMap, refract(-eyeDirection, N, glassEta.r));
    vec4 refractedG = texture(environmentMap, refract(-eyeDirection, N, glassEta.g));
    vec4 refractedB = texture(environmentMap, refract(-eyeDirection, N, glassEta.b));
    
    vec4 refractedColor = vec4(refractedR.r, refractedG.g, refractedB.b, 1);
    
    // Tell OpenGL to use the mix of the refracted and reflected color based on the fresnel term.
    fragColor.rgb = mix(refractedColor.rgb, reflectedColor.rgb, f); // change me

    // And, set the alpha component to 1.0 (completely opaque, no transparency).
    fragColor.a = 1.0;

}
