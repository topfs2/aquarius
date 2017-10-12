//
uniform sampler2D Tex0;

void main()
{
  gl_FragColor = texture2D(Tex0, gl_TexCoord[0].st) * gl_Color;
}
