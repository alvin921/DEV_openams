/*----------------------------------------------------------------------*/
/*                                                                      */
/*      NES Mapeers Factory                                             */
/*                                                           Norix      */
/*                                               written     2003/09/04 */
/*                                               last modify ----/--/-- */
/*----------------------------------------------------------------------*/
#include "mapper.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "pad.h"
#include "rom.h"

//////////////////////////////////////////////////////////////////////////

#include "Mapper000.h"
#include "Mapper001.h"
#include "Mapper002.h"
#include "Mapper003.h"
#include "Mapper004.h"
#include "Mapper005.h"
#include "Mapper006.h"
#include "Mapper007.h"
#include "Mapper008.h"
#include "Mapper009.h"
#include "Mapper010.h"
#include "Mapper011.h"
#include "Mapper013.h"
#include "Mapper015.h"
#include "Mapper016.h"
#include "Mapper017.h"
#include "Mapper018.h"
#include "Mapper019.h"
#include "MapperFDS.h"	// 020
#include "Mapper021.h"
#include "Mapper022.h"
#include "Mapper023.h"
#include "Mapper024.h"
#include "Mapper025.h"
#include "Mapper026.h"
#include "Mapper032.h"
#include "Mapper033.h"
#include "Mapper034.h"
#include "Mapper040.h"
#include "Mapper041.h"
#include "Mapper042.h"
#include "Mapper043.h"
#include "Mapper048.h"
#include "Mapper064.h"
#include "Mapper065.h"
#include "Mapper066.h"
#include "Mapper067.h"
#include "Mapper068.h"
#include "Mapper069.h"
#include "Mapper070.h"
#include "Mapper071.h"
#include "Mapper072.h"
#include "Mapper073.h"
#include "Mapper075.h"
#include "Mapper076.h"
#include "Mapper077.h"
#include "Mapper078.h"
#include "Mapper079.h"
#include "Mapper080.h"
#include "Mapper082.h"
#include "Mapper083.h"
#include "Mapper085.h"
#include "Mapper086.h"
#include "Mapper087.h"
#include "Mapper088.h"
#include "Mapper089.h"
#include "Mapper090.h"
#include "Mapper091.h"
#include "Mapper092.h"
#include "Mapper093.h"
#include "Mapper094.h"
#include "Mapper095.h"
#include "Mapper096.h"
#include "Mapper097.h"
#include "Mapper100.h"
#include "Mapper101.h"
#include "Mapper107.h"
#include "Mapper113.h"
#include "Mapper118.h"
#include "Mapper119.h"
#include "Mapper122.h"
#include "Mapper180.h"
#include "Mapper181.h"
#include "Mapper185.h"
#include "Mapper187.h"
#include "Mapper188.h"
#include "Mapper189.h"
#include "Mapper243.h"

#include "MapperNSF.h"

#include "Mapper044.h"
#include "Mapper045.h"
#include "Mapper046.h"
#include "Mapper047.h"
#include "Mapper050.h"
#include "Mapper051.h"
#include "Mapper057.h"
#include "Mapper058.h"
#include "Mapper060.h"
#include "Mapper062.h"
#include "Mapper074.h"
#include "Mapper105.h"
#include "Mapper108.h"
#include "Mapper109.h"
#include "Mapper110.h"
#include "Mapper112.h"
#include "Mapper114.h"
#include "Mapper115.h"
#include "Mapper116.h"
#include "Mapper117.h"
#include "Mapper133.h"
#include "Mapper134.h"
#include "Mapper135.h"
#include "Mapper140.h"
#include "Mapper142.h"
#include "Mapper160.h"
#include "Mapper182.h"
#include "Mapper183.h"
#include "Mapper190.h"
#include "Mapper191.h"
#include "Mapper193.h"
#include "Mapper194.h"
#include "Mapper198.h"
#include "Mapper222.h"
#include "Mapper225.h"
#include "Mapper226.h"
#include "Mapper227.h"
#include "Mapper228.h"
#include "Mapper229.h"
#include "Mapper230.h"
#include "Mapper231.h"
#include "Mapper232.h"
#include "Mapper233.h"
#include "Mapper234.h"
#include "Mapper235.h"
#include "Mapper236.h"
#include "Mapper240.h"
#include "Mapper241.h"
#include "Mapper242.h"
#include "Mapper244.h"
#include "Mapper245.h"
#include "Mapper246.h"
#include "Mapper248.h"
#include "Mapper249.h"
#include "Mapper251.h"
#include "Mapper252.h"
#include "Mapper254.h"
#include "Mapper255.h"

#include "Mapper099.h"
#include "Mapper151.h"

#include "Mapper012.h"

#include "Mapper200.h"
#include "Mapper201.h"
#include "Mapper202.h"

#include "Mapper061.h"

#include "Mapper027.h"

#include "Mapper164.h"
#include "Mapper165.h"
#include "Mapper167.h"

//////////////////////////////////////////////////////////////////////////
// Mapper Factory
//////////////////////////////////////////////////////////////////////////
Mapper*	CreateMapper( NES* parent, INT no )
{
	switch( no ) {
		case	0:
			return Mapper000_New(parent);
		case	1:
			return Mapper001_New(parent);
		case	2:
			return Mapper002_New(parent);
		case	3:
			return Mapper003_New(parent);
		case	4:
			return Mapper004_New(parent);
		case	5:
			return Mapper005_New(parent);
		case	6:
			return Mapper006_New(parent);
		case	7:
			return Mapper007_New(parent);
		case	8:
			return Mapper008_New(parent);
		case	9:
			return Mapper009_New(parent);
		case	10:
			return Mapper010_New(parent);
		case	11:
			return Mapper011_New(parent);
		case	13:
			return Mapper013_New(parent);
		case	15:
			return Mapper015_New(parent);
		case	16:
			return Mapper016_New(parent);
		case	17:
			return Mapper017_New(parent);
		case	18:
			return Mapper018_New(parent);
		case	19:
			return Mapper019_New(parent);
		case	20:
			return Mapper020_New(parent);
		case	21:
			return Mapper021_New(parent);
		case	22:
			return Mapper022_New(parent);
		case	23:
			return Mapper023_New(parent);
		case	24:
			return Mapper024_New(parent);
		case	25:
			return Mapper025_New(parent);
		case	26:
			return Mapper026_New(parent);
		case	32:
			return Mapper032_New(parent);
		case	33:
			return Mapper033_New(parent);
		case	34:
			return Mapper034_New(parent);
		case	40:
			return Mapper040_New(parent);
		case	41:
			return Mapper041_New(parent);
		case	42:
			return Mapper042_New(parent);
		case	43:
			return Mapper043_New(parent);
		case	48:
			return Mapper048_New(parent);
		case	64:
			return Mapper064_New(parent);
		case	65:
			return Mapper065_New(parent);
		case	66:
			return Mapper066_New(parent);
		case	67:
			return Mapper067_New(parent);
		case	68:
			return Mapper068_New(parent);
		case	69:
			return Mapper069_New(parent);
		case	70:
			return Mapper070_New(parent);
		case	71:
			return Mapper071_New(parent);
		case	72:
			return Mapper072_New(parent);
		case	73:
			return Mapper073_New(parent);
		case	75:
			return Mapper075_New(parent);
		case	76:
			return Mapper076_New(parent);
		case	77:
			return Mapper077_New(parent);
		case	78:
			return Mapper078_New(parent);
		case	79:
			return Mapper079_New(parent);
		case	80:
			return Mapper080_New(parent);
		case	82:
			return Mapper082_New(parent);
		case	83:
			return Mapper083_New(parent);
		case	85:
			return Mapper085_New(parent);
		case	86:
			return Mapper086_New(parent);
		case	87:
			return Mapper087_New(parent);
		case	88:
			return Mapper088_New(parent);
		case	89:
			return Mapper089_New(parent);
		case	90:
			return Mapper090_New(parent);
		case	91:
			return Mapper091_New(parent);
		case	92:
			return Mapper092_New(parent);
		case	93:
			return Mapper093_New(parent);
		case	94:
			return Mapper094_New(parent);
		case	95:
			return Mapper095_New(parent);
		case	96:
			return Mapper096_New(parent);
		case	97:
			return Mapper097_New(parent);
		case	100:
			return Mapper100_New(parent);
		case	101:
			return Mapper101_New(parent);
		case	107:
			return Mapper107_New(parent);
		case	113:
			return Mapper113_New(parent);
		case	118:
			return Mapper118_New(parent);
		case	119:
			return Mapper119_New(parent);
		case	122:
		case	184:
			return Mapper122_New(parent);
		case	180:
			return Mapper180_New(parent);
		case	181:
			return Mapper181_New(parent);
		case	185:
			return Mapper185_New(parent);
		case	187:
			return Mapper187_New(parent);
		case	188:
			return Mapper188_New(parent);
		case	189:
			return Mapper189_New(parent);
		case	243:
			return Mapper243_New(parent);
		case	0x100:
			return MapperNSF_New(parent);

		case	44:
			return Mapper044_New(parent);
		case	45:
			return Mapper045_New(parent);
		case	46:
			return Mapper046_New(parent);
		case	47:
			return Mapper047_New(parent);
		case	50:
			return Mapper050_New(parent);
		case	51:
			return Mapper051_New(parent);
		case	57:
			return Mapper057_New(parent);
		case	58:
			return Mapper058_New(parent);
		case	60:
			return Mapper060_New(parent);
		case	62:
			return Mapper062_New(parent);
		case	74:
			return Mapper074_New(parent);
		case	105:
			return Mapper105_New(parent);
		case	108:
			return Mapper108_New(parent);
		case	109:
			return Mapper109_New(parent);
		case	110:
			return Mapper110_New(parent);
		case	112:
			return Mapper112_New(parent);
		case	114:
			return Mapper114_New(parent);
		case	115:
			return Mapper115_New(parent);
		case	116:
			return Mapper116_New(parent);
		case	117:
			return Mapper117_New(parent);
		case	133:
			return Mapper133_New(parent);
		case	134:
			return Mapper134_New(parent);
		case	135:
			return Mapper135_New(parent);
		case	140:
			return Mapper140_New(parent);
		case	142:
			return Mapper142_New(parent);
		case	160:
			return Mapper160_New(parent);
		case	182:
			return Mapper182_New(parent);
		case	183:
			return Mapper183_New(parent);
		case	190:
			return Mapper190_New(parent);
		case	191:
			return Mapper191_New(parent);
		case	193:
			return Mapper193_New(parent);
		case	194:
			return Mapper194_New(parent);
		case	198:
			return Mapper198_New(parent);
		case	222:
			return Mapper222_New(parent);
		case	225:
			return Mapper225_New(parent);
		case	226:
			return Mapper226_New(parent);
		case	227:
			return Mapper227_New(parent);
		case	228:
			return Mapper228_New(parent);
		case	229:
			return Mapper229_New(parent);
		case	230:
			return Mapper230_New(parent);
		case	231:
			return Mapper231_New(parent);
		case	232:
			return Mapper232_New(parent);
		case	233:
			return Mapper233_New(parent);
		case	234:
			return Mapper234_New(parent);
		case	235:
			return Mapper235_New(parent);
		case	236:
			return Mapper236_New(parent);
		case	240:
			return Mapper240_New(parent);
		case	241:
			return Mapper241_New(parent);
		case	242:
			return Mapper242_New(parent);
		case	244:
			return Mapper244_New(parent);
		case	245:
			return Mapper245_New(parent);
		case	246:
			return Mapper246_New(parent);
		case	248:
			return Mapper248_New(parent);
		case	249:
			return Mapper249_New(parent);
		case	251:
			return Mapper251_New(parent);
		case	252:
			return Mapper252_New(parent);
		case	254:
			return Mapper254_New(parent);
		case	255:
			return Mapper255_New(parent);

		case	99:
			return Mapper099_New(parent);
		case	151:
			return Mapper151_New(parent);

		case	12:
			return Mapper012_New(parent);

		case	200:
			return Mapper200_New(parent);
		case	201:
			return Mapper201_New(parent);
		case	202:
			return Mapper202_New(parent);

		case	61:
			return Mapper061_New(parent);

		case	27:
			return Mapper027_New(parent);

		case	164:
			return Mapper164_New(parent);
		case	165:
			return Mapper165_New(parent);
		case	167:
			return Mapper167_New(parent);

		default:
			break;
	}

	return	NULL;
}

