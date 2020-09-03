#version 330

in vec3 Color;

uniform bool Complimentary;

uniform vec3 SolidColor = vec3(-1,-1,-1);

out vec4 Fragment;

void main()
{
	vec3 newColor;
		
	if(Complimentary) {
	
			newColor.r = 1 - Color.r;
			newColor.g = 1 - Color.g;
			newColor.b = 1 - Color.b;

		
		Fragment = vec4(newColor, 1);
	}
	else 
		Fragment = vec4(Color,1);
		

	if(SolidColor.r != -1.0 && SolidColor.g != -1.0 && SolidColor.b != -1.0){
		if(Complimentary) {
		
			
			newColor.r = 1 - SolidColor.r;
			newColor.g = 1 - SolidColor.g;
			newColor.b = 1 - SolidColor.b;
			
			Fragment = vec4(newColor, 1);
		}
		else
			Fragment = vec4(SolidColor, 1);
	}

	return;
}