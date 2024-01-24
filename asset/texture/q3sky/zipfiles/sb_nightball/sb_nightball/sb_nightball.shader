//**********************************************************************//
//									//
//	Title  : nightball.shader					//
//	Author : Sock							//
//	Date   : 25th October 2001					//
//	Email  : sock@planetquake.com					//
//	URL    : http://www.planetquake.com/simland			//
//									//

// Direction & elevation checked and adjusted - Speaker
//**********************************************************************//

textures/skies/sb_nightball
{
	qer_editorimage env/sb_nightball/nightball_ft.tga
	surfaceparm noimpact
	surfaceparm nolightmap
	q3map_globaltexture
	q3map_lightsubdivide 256
	q3map_surfacelight 25
	surfaceparm sky
	q3map_sun 0.851 0.945 1.0 100 310 45
	skyparms env/sb_nightball/nightball - -
}
