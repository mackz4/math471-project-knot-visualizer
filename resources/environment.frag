#version 330

// Fragment shader


// These get passed in from the vertex shader and are interpolated (varying) properties,
// so they change for each pixel across the triangle:
in vec4 interpSurfPosition; // position on the surface in world coordinates
in vec3 interpSurfNormal; // normal in world coordinates
in vec2 texCoord;

uniform vec4 lightPosition; // in world coordinates

//Textures
uniform samplerCube environmentMap;
uniform sampler2D tex;

// Material Properties
uniform vec3 ambientReflectionCoeff;
uniform vec3 diffuseReflectionCoeff;
uniform vec3 specularReflectionCoeff;
 
uniform float specularExponent;
 
uniform vec3 ambientLightIntensity;
uniform vec3 diffuseLightIntensity;
uniform vec3 specularLightIntensity;

uniform vec3 eye_world;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

void main() {

    // Start with black and then add lighting to the final color as we calculate it
    vec3 finalColor = texture(tex, texCoord).rgb;
     
    // Calculate ambient, diffuse, and specular lighting for this pixel based on its position, normal, etc.
    vec3 ambient = ambientReflectionCoeff*ambientLightIntensity;
    finalColor += ambient;
     
    vec3 N = normalize(interpSurfNormal);
    vec3 eyeDirection = normalize(eye_world - (interpSurfPosition.xyz));
    vec3 lightDirection = normalize(lightPosition - interpSurfPosition).xyz;
    vec3 halfway = normalize(lightDirection + eyeDirection);
    
    float NdotL = clamp(dot(N, lightDirection), 0, 1);
    vec3 diffuse = diffuseReflectionCoeff * diffuseLightIntensity * NdotL;
    finalColor += diffuse;
    
    vec3 specular = specularReflectionCoeff * specularLightIntensity * pow(clamp(dot(N, halfway), 0, 1), specularExponent);
    finalColor += specular;
    
    // Tell OpenGL to use the r,g,b compenents of finalColor for the color of this fragment (pixel).
    fragColor.rgb = finalColor.rgb;
     
    // And, set the alpha component to 1.0 (completely opaque, no transparency).
    fragColor.a = 1.0;
}
