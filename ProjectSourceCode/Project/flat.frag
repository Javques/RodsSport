//totally based on https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp
#version 330 core
out vec4 FragColor;
in vec3 VertColor;


void main()
{
	FragColor = vec4(VertColor, 1.0f);
}

