#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    vec4 cor = texture(texture0, fragTexCoord) * fragColor;

    // Converte a imagem para tons de cinza.
    float intensidade = dot(
        cor.rgb,
        vec3(0.299, 0.587, 0.114)
    );

    finalColor = vec4(
        vec3(intensidade),
        cor.a
    );
}s