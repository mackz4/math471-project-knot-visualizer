#version 330

// Fragment shader


// These get passed in from the vertex shader and are interpolated (varying) properties,
// so they change for each pixel across the triangle:
in vec4 interpSurfPosition; // position on the surface in world coordinates
in vec3 interpSurfNormal; // normal in world coordinates

//TODO: add a uniform variable for the light position
uniform vec4 lightPosition; // in world coordinates

//Textures
uniform samplerCube environmentMap; // The cube map containing the environment to reflect


//Material Properties
uniform float r0; // The Fresnel reflectivity when the incident angle is 0
uniform float m; // The roughness of the material

// TODO: add other material properties variables
uniform vec3 ambientReflectionCoeff;
uniform vec3 diffuseReflectionCoeff;
uniform vec3 specularReflectionCoeff;

// TODO: Light Properties
uniform vec3 ambientLightIntensity;
uniform vec3 diffuseLightIntensity;
uniform vec3 specularLightIntensity;


// The camera location in world coordinates
uniform vec3 eye_world;

// This is an out variable for the final color we want to render this fragment.
out vec4 fragColor;

void main() {

    // Start with black and then add lighting to the final color as we calculate it
	vec3 materialColor = vec3(0.0, 0.0, 0.0);

	// TODO: Fill this in using to cook-torrance model
    vec3 ambient = ambientReflectionCoeff*ambientLightIntensity;
    
    vec3 N = normalize(interpSurfNormal);
    
    vec3 lightDirection = normalize(lightPosition - interpSurfPosition).xyz;
    vec3 eyeDirection = normalize(eye_world - (interpSurfPosition.xyz));
    vec3 diffuse = diffuseReflectionCoeff*diffuseLightIntensity*(dot(N, lightDirection));
    vec3 halfway = normalize(lightDirection + eyeDirection);
    
    float alpha = acos(dot(N, halfway));
    float d = (exp((-tan(alpha)*tan(alpha))/(m*m)) / (4*m*m*cos(alpha)*cos(alpha)*cos(alpha)*cos(alpha)));
    
    float theta = acos(clamp(dot(eyeDirection, halfway), 0.0, 1.0));
    float f = r0 + (1 - r0)*(1-cos(theta))*(1-cos(theta))*(1-cos(theta))*(1-cos(theta))*(1-cos(theta));
    
    float g1 = min(1.0f, (2*dot(halfway, N)*dot(eyeDirection, N))/(dot(eyeDirection, halfway)));
    float g = min(g1, (2*dot(halfway, N)*clamp(dot(lightDirection, N), 0.0, 1.0))/(dot(eyeDirection, halfway)));
    
    float cookTorrance = (d*f*g)/dot(eyeDirection, N);
    vec3 specular = cookTorrance * specularReflectionCoeff * specularLightIntensity;
    
    materialColor += ambient + diffuse + specular;
    
    vec4 reflectedColor = texture(environmentMap, normalize(reflect(-eyeDirection, N)));

	// TODO: Tell OpenGL to use the r,g,b compenents of materialColor mixed with the reflected color for the color of this fragment (pixel).
    fragColor.rgb = materialColor * (1 - f) + f*reflectedColor.rgb; // Change me

	// And, set the alpha component to 1.0 (completely opaque, no transparency).
	fragColor.a = 1.0;
}
