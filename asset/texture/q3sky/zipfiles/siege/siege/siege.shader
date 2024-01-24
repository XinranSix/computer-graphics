//****************************************************//
//									//
//	siege.shader for GTKRadiant			//
//	by Amethyst7 - 06.12.2001				//
//									//

// Direction & elevation checked and adjusted - Speaker
//****************************************************//

textures/skies/siege
{
	qer_editorimage env/siege/siege.jpg
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_sun 1.00 1.00 0.965 75 320 30
	q3map_surfacelight 50
	skyparms env/siege/siege - -
	{
		map env/siege/clouds.jpg
		blendFunc filter
		tcMod scroll 0.005 0.005
		tcMod scale 2 2
	}
}