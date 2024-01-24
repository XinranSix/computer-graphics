//****************************************************//
//
//	darkgloom.shader for GTKRadiant
//	by Amethyst7 - 04.19.2001
//
// Direction & elevation checked and adjusted - Speaker	
//****************************************************//

textures/skies/darkgloom
{
	qer_editorimage env/darkgloom/darkgloom.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_sun 0.934 0.835 1.00 75 330 25
	q3map_surfacelight 50
	skyparms env/darkgloom/darkgloom - -
	{
		map env/darkgloom/clouds.jpg
		blendFunc filter
		tcMod scroll 0.005 0.005
		tcMod scale 3 3
	}
}