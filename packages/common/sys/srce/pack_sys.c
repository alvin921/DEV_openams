
#include "pack_sys.hi"

#if defined(TARGET_WIN)
#if (TARGET_ENDIAN == ENDIAN_LITTLE)
#include "sys_brd_le.h"
#else
#include "sys_brd_be.h"
#endif
#endif


typedef struct {
	gu32	plmn;
	resid_t	resid;
}t_PlmnResid;

#define	PLMN_ADD(num)		{num, RESID_STR_PLMN_##num}

static const t_PlmnResid gaPlmnResidTab[] = {
	PLMN_ADD(46000),
	PLMN_ADD(46001),
	PLMN_ADD(46601),
	PLMN_ADD(46688),
	PLMN_ADD(46689),
	PLMN_ADD(46692),
	PLMN_ADD(46693),
	PLMN_ADD(46697),
	PLMN_ADD(46699),
	PLMN_ADD(45400),
	PLMN_ADD(45402),
	PLMN_ADD(45403),
	PLMN_ADD(45404),
	PLMN_ADD(45406),
	PLMN_ADD(45410),
	PLMN_ADD(45412),
	PLMN_ADD(45415),
	PLMN_ADD(45416),
	PLMN_ADD(45418),
	PLMN_ADD(45419),
	PLMN_ADD(45500),
	PLMN_ADD(45501),
	PLMN_ADD(45503),
	PLMN_ADD(45504),
	PLMN_ADD(45505),
	PLMN_ADD(40401),
	PLMN_ADD(40402),
	PLMN_ADD(40403),
	PLMN_ADD(40404),
	PLMN_ADD(40405),
	PLMN_ADD(40407),
	PLMN_ADD(40410),
	PLMN_ADD(40411),
	PLMN_ADD(40412),
	PLMN_ADD(40413),
	PLMN_ADD(40414),
	PLMN_ADD(40415),
	PLMN_ADD(40417),
	PLMN_ADD(40419),
	PLMN_ADD(40420),
	PLMN_ADD(40421),
	PLMN_ADD(40422),
	PLMN_ADD(40424),
	PLMN_ADD(40425),
	PLMN_ADD(40427),
	PLMN_ADD(40428),
	PLMN_ADD(40429),
	PLMN_ADD(40431),
	PLMN_ADD(40433),
	PLMN_ADD(40434),
	PLMN_ADD(40435),
	PLMN_ADD(40437),
	PLMN_ADD(40438),
	PLMN_ADD(40440),
	PLMN_ADD(40441),
	PLMN_ADD(40442),
	PLMN_ADD(40443),
	PLMN_ADD(40444),
	PLMN_ADD(40445),
	PLMN_ADD(40446),
	PLMN_ADD(40449),
	PLMN_ADD(40451),
	PLMN_ADD(40453),
	PLMN_ADD(40454),
	PLMN_ADD(40455),
	PLMN_ADD(40456),
	PLMN_ADD(40457),
	PLMN_ADD(40458),
	PLMN_ADD(40459),
	PLMN_ADD(40460),
	PLMN_ADD(40462),
	PLMN_ADD(40464),
	PLMN_ADD(40466),
	PLMN_ADD(40468),
	PLMN_ADD(40469),
	PLMN_ADD(40470),
	PLMN_ADD(40471),
	PLMN_ADD(40472),
	PLMN_ADD(40473),
	PLMN_ADD(40474),
	PLMN_ADD(40475),
	PLMN_ADD(40476),
	PLMN_ADD(40477),
	PLMN_ADD(40478),
	PLMN_ADD(40479),
	PLMN_ADD(40480),
	PLMN_ADD(40481),
	PLMN_ADD(40482),
	PLMN_ADD(40484),
	PLMN_ADD(40486),
	PLMN_ADD(40487),
	PLMN_ADD(40488),
	PLMN_ADD(40489),
	PLMN_ADD(40490),
	PLMN_ADD(40491),
	PLMN_ADD(40492),
	PLMN_ADD(40493),
	PLMN_ADD(40494),
	PLMN_ADD(40495),
	PLMN_ADD(40496),
	PLMN_ADD(40497),
	PLMN_ADD(40498),
	PLMN_ADD(404030),
	PLMN_ADD(40501),
	PLMN_ADD(40505),
	PLMN_ADD(40506),
	PLMN_ADD(40507),
	PLMN_ADD(40509),
	PLMN_ADD(40510),
	PLMN_ADD(40511),
	PLMN_ADD(40513),
	PLMN_ADD(40515),
	PLMN_ADD(40518),
	PLMN_ADD(40519),
	PLMN_ADD(40520),
	PLMN_ADD(40521),
	PLMN_ADD(40522),
	PLMN_ADD(40550),
	PLMN_ADD(40551),
	PLMN_ADD(40552),
	PLMN_ADD(40553),
	PLMN_ADD(40554),
	PLMN_ADD(40555),
	PLMN_ADD(40556),
	PLMN_ADD(40566),
	PLMN_ADD(40567),
	PLMN_ADD(40570),
	PLMN_ADD(405025),
	PLMN_ADD(405027),
	PLMN_ADD(405029),
	PLMN_ADD(405030),
	PLMN_ADD(405031),
	PLMN_ADD(405032),
	PLMN_ADD(405034),
	PLMN_ADD(405035),
	PLMN_ADD(405036),
	PLMN_ADD(405037),
	PLMN_ADD(405038),
	PLMN_ADD(405039),
	PLMN_ADD(405041),
	PLMN_ADD(405042),
	PLMN_ADD(405043),
	PLMN_ADD(405044),
	PLMN_ADD(405045),
	PLMN_ADD(405046),
	PLMN_ADD(405047),
	PLMN_ADD(405750),
	PLMN_ADD(405751),
	PLMN_ADD(405752),
	PLMN_ADD(405753),
	PLMN_ADD(405754),
	PLMN_ADD(405755),
	PLMN_ADD(405756),
	PLMN_ADD(405799),
	PLMN_ADD(405800),
	PLMN_ADD(405801),
	PLMN_ADD(405803),
	PLMN_ADD(405805),
	PLMN_ADD(405809),
	PLMN_ADD(405810),
	PLMN_ADD(405811),
	PLMN_ADD(405852),
	PLMN_ADD(41201),
	PLMN_ADD(41220),
	PLMN_ADD(41240),
	PLMN_ADD(41250),
	PLMN_ADD(47001),
	PLMN_ADD(47002),
	PLMN_ADD(47003),
	PLMN_ADD(47004),
	PLMN_ADD(47007),
	PLMN_ADD(42601),
	PLMN_ADD(42602),
	PLMN_ADD(40211),
	PLMN_ADD(40277),
	PLMN_ADD(52802),
	PLMN_ADD(52811),
	PLMN_ADD(45601),
	PLMN_ADD(45602),
	PLMN_ADD(45604),
	PLMN_ADD(45605),
	PLMN_ADD(45618),
	PLMN_ADD(51402),
	PLMN_ADD(51001),
	PLMN_ADD(51008),
	PLMN_ADD(51010),
	PLMN_ADD(51011),
	PLMN_ADD(51021),
	PLMN_ADD(51089),
	PLMN_ADD(43211),
	PLMN_ADD(43214),
	PLMN_ADD(43219),
	PLMN_ADD(43235),
	PLMN_ADD(41805),
	PLMN_ADD(41820),
	PLMN_ADD(42501),
	PLMN_ADD(42502),
	PLMN_ADD(42503),
	PLMN_ADD(44010),
	PLMN_ADD(44020),
	PLMN_ADD(41601),
	PLMN_ADD(41603),
	PLMN_ADD(41677),
	PLMN_ADD(45002),
	PLMN_ADD(45005),
	PLMN_ADD(41902),
	PLMN_ADD(41903),
	PLMN_ADD(43701),
	PLMN_ADD(43705),
	PLMN_ADD(45701),
	PLMN_ADD(45702),
	PLMN_ADD(45703),
	PLMN_ADD(45708),
	PLMN_ADD(41501),
	PLMN_ADD(50212),
	PLMN_ADD(50213),
	PLMN_ADD(50216),
	PLMN_ADD(50217),
	PLMN_ADD(50219),
	PLMN_ADD(47201),
	PLMN_ADD(47202),
	PLMN_ADD(42899),
	PLMN_ADD(41401),
	PLMN_ADD(42901),
	PLMN_ADD(42902),
	PLMN_ADD(42202),
	PLMN_ADD(42203),
	PLMN_ADD(41001),
	PLMN_ADD(41003),
	PLMN_ADD(41007),
	PLMN_ADD(51501),
	PLMN_ADD(51502),
	PLMN_ADD(51503),
	PLMN_ADD(51505),
	PLMN_ADD(42701),
	PLMN_ADD(42001),
	PLMN_ADD(42003),
	PLMN_ADD(52501),
	PLMN_ADD(52502),
	PLMN_ADD(52503),
	PLMN_ADD(52505),
	PLMN_ADD(41302),
	PLMN_ADD(41303),
	PLMN_ADD(41701),
	PLMN_ADD(41702),
	PLMN_ADD(43601),
	PLMN_ADD(43602),
	PLMN_ADD(43603),
	PLMN_ADD(43604),
	PLMN_ADD(43605),
	PLMN_ADD(52001),
	PLMN_ADD(52010),
	PLMN_ADD(52015),
	PLMN_ADD(52018),
	PLMN_ADD(52023),
	PLMN_ADD(28601),
	PLMN_ADD(28602),
	PLMN_ADD(28603),
	PLMN_ADD(28604),
	PLMN_ADD(42402),
	PLMN_ADD(42403),
	PLMN_ADD(45201),
	PLMN_ADD(45202),
	PLMN_ADD(45204),
	PLMN_ADD(42101),
	PLMN_ADD(42102),
	PLMN_ADD(27601),
	PLMN_ADD(27602),
	PLMN_ADD(21303),
	PLMN_ADD(28301),
	PLMN_ADD(23201),
	PLMN_ADD(23203),
	PLMN_ADD(23205),
	PLMN_ADD(23207),
	PLMN_ADD(23210),
	PLMN_ADD(40001),
	PLMN_ADD(40002),
	PLMN_ADD(40004),
	PLMN_ADD(25701),
	PLMN_ADD(25702),
	PLMN_ADD(20601),
	PLMN_ADD(20610),
	PLMN_ADD(20620),
	PLMN_ADD(21803),
	PLMN_ADD(21805),
	PLMN_ADD(21890),
	PLMN_ADD(28401),
	PLMN_ADD(28405),
	PLMN_ADD(21901),
	PLMN_ADD(21910),
	PLMN_ADD(28001),
	PLMN_ADD(23001),
	PLMN_ADD(23002),
	PLMN_ADD(23003),
	PLMN_ADD(23801),
	PLMN_ADD(23802),
	PLMN_ADD(23806),
	PLMN_ADD(23820),
	PLMN_ADD(23830),
	PLMN_ADD(24801),
	PLMN_ADD(24802),
	PLMN_ADD(24803),
	PLMN_ADD(28801),
	PLMN_ADD(28802),
	PLMN_ADD(24405),
	PLMN_ADD(24409),
	PLMN_ADD(24412),
	PLMN_ADD(24414),
	PLMN_ADD(24491),
	PLMN_ADD(20801),
	PLMN_ADD(20810),
	PLMN_ADD(20820),
	PLMN_ADD(28201),
	PLMN_ADD(28202),
	PLMN_ADD(26201),
	PLMN_ADD(26202),
	PLMN_ADD(26203),
	PLMN_ADD(26207),
	PLMN_ADD(26213),
	PLMN_ADD(26214),
	PLMN_ADD(26601),
	PLMN_ADD(20201),
	PLMN_ADD(20205),
	PLMN_ADD(20209),
	PLMN_ADD(20210),
	PLMN_ADD(29001),
	PLMN_ADD(21601),
	PLMN_ADD(21630),
	PLMN_ADD(21670),
	PLMN_ADD(27401),
	PLMN_ADD(27402),
	PLMN_ADD(27403),
	PLMN_ADD(27404),
	PLMN_ADD(27201),
	PLMN_ADD(27202),
	PLMN_ADD(27203),
	PLMN_ADD(22201),
	PLMN_ADD(22210),
	PLMN_ADD(22288),
	PLMN_ADD(22299),
	PLMN_ADD(40101),
	PLMN_ADD(40102),
	PLMN_ADD(24701),
	PLMN_ADD(24702),
	PLMN_ADD(29501),
	PLMN_ADD(29502),
	PLMN_ADD(29505),
	PLMN_ADD(29577),
	PLMN_ADD(24601),
	PLMN_ADD(24602),
	PLMN_ADD(24603),
	PLMN_ADD(27001),
	PLMN_ADD(27077),
	PLMN_ADD(27099),
	PLMN_ADD(29401),
	PLMN_ADD(29402),
	PLMN_ADD(27801),
	PLMN_ADD(27821),
	PLMN_ADD(25901),
	PLMN_ADD(25902),
	PLMN_ADD(22002),
	PLMN_ADD(22004),
	PLMN_ADD(20404),
	PLMN_ADD(20408),
	PLMN_ADD(20412),
	PLMN_ADD(20416),
	PLMN_ADD(20420),
	PLMN_ADD(24201),
	PLMN_ADD(24202),
	PLMN_ADD(24203),
	PLMN_ADD(26001),
	PLMN_ADD(26002),
	PLMN_ADD(26003),
	PLMN_ADD(26801),
	PLMN_ADD(26803),
	PLMN_ADD(26806),
	PLMN_ADD(22601),
	PLMN_ADD(22603),
	PLMN_ADD(22610),
	PLMN_ADD(25001),
	PLMN_ADD(25002),
	PLMN_ADD(25003),
	PLMN_ADD(25004),
	PLMN_ADD(25005),
	PLMN_ADD(25007),
	PLMN_ADD(25010),
	PLMN_ADD(25011),
	PLMN_ADD(25012),
	PLMN_ADD(25013),
	PLMN_ADD(25016),
	PLMN_ADD(25017),
	PLMN_ADD(25019),
	PLMN_ADD(25020),
	PLMN_ADD(25028),
	PLMN_ADD(25039),
	PLMN_ADD(25044),
	PLMN_ADD(25092),
	PLMN_ADD(25093),
	PLMN_ADD(25099),
	PLMN_ADD(22001),
	PLMN_ADD(22003),
	PLMN_ADD(22005),
	PLMN_ADD(23101),
	PLMN_ADD(23102),
	PLMN_ADD(29340),
	PLMN_ADD(29341),
	PLMN_ADD(29370),
	PLMN_ADD(21401),
	PLMN_ADD(21403),
	PLMN_ADD(21404),
	PLMN_ADD(21407),
	PLMN_ADD(24001),
	PLMN_ADD(24002),
	PLMN_ADD(24003),
	PLMN_ADD(24004),
	PLMN_ADD(24005),
	PLMN_ADD(24007),
	PLMN_ADD(24008),
	PLMN_ADD(24010),
	PLMN_ADD(22801),
	PLMN_ADD(22802),
	PLMN_ADD(22803),
	PLMN_ADD(22807),
	PLMN_ADD(22808),
	PLMN_ADD(25501),
	PLMN_ADD(25502),
	PLMN_ADD(25503),
	PLMN_ADD(25505),
	PLMN_ADD(25506),
	PLMN_ADD(25507),
	PLMN_ADD(23410),
	PLMN_ADD(23415),
	PLMN_ADD(23420),
	PLMN_ADD(23430),
	PLMN_ADD(23433),
	PLMN_ADD(23450),
	PLMN_ADD(23455),
	PLMN_ADD(23458),
	PLMN_ADD(43401),
	PLMN_ADD(43402),
	PLMN_ADD(43404),
	PLMN_ADD(43405),
	PLMN_ADD(43407),
	PLMN_ADD(43801),
	PLMN_ADD(344030),
	PLMN_ADD(344920),
	PLMN_ADD(344930),
	PLMN_ADD(72202),
	PLMN_ADD(72204),
	PLMN_ADD(72207),
	PLMN_ADD(72231),
	PLMN_ADD(72234),
	PLMN_ADD(72235),
	PLMN_ADD(36301),
	PLMN_ADD(70267),
	PLMN_ADD(70268),
	PLMN_ADD(35001),
	PLMN_ADD(35002),
	PLMN_ADD(73601),
	PLMN_ADD(73602),
	PLMN_ADD(73603),
	PLMN_ADD(72402),
	PLMN_ADD(72405),
	PLMN_ADD(72410),
	PLMN_ADD(72415),
	PLMN_ADD(72416),
	PLMN_ADD(72423),
	PLMN_ADD(72431),
	PLMN_ADD(72432),
	PLMN_ADD(302370),
	PLMN_ADD(302710),
	PLMN_ADD(302720),
	PLMN_ADD(73001),
	PLMN_ADD(73002),
	PLMN_ADD(73003),
	PLMN_ADD(732101),
	PLMN_ADD(732103),
	PLMN_ADD(732123),
	PLMN_ADD(71201),
	PLMN_ADD(36801),
	PLMN_ADD(37001),
	PLMN_ADD(37002),
	PLMN_ADD(37003),
	PLMN_ADD(37004),
	PLMN_ADD(74000),
	PLMN_ADD(74001),
	PLMN_ADD(70601),
	PLMN_ADD(70602),
	PLMN_ADD(70603),
	PLMN_ADD(708001),
	PLMN_ADD(70802),
	PLMN_ADD(338180),
	PLMN_ADD(338050),
	PLMN_ADD(34001),
	PLMN_ADD(34002),
	PLMN_ADD(34020),
	PLMN_ADD(33401),
	PLMN_ADD(334020),
	PLMN_ADD(33403),
	PLMN_ADD(352110),
	PLMN_ADD(352130),
	PLMN_ADD(70401),
	PLMN_ADD(70402),
	PLMN_ADD(70403),
	PLMN_ADD(73801),
	PLMN_ADD(37201),
	PLMN_ADD(36251),
	PLMN_ADD(36269),
	PLMN_ADD(36291),
	PLMN_ADD(71021),
	PLMN_ADD(71073),
	PLMN_ADD(71401),
	PLMN_ADD(74401),
	PLMN_ADD(74402),
	PLMN_ADD(71606),
	PLMN_ADD(71610),
	PLMN_ADD(356070),
	PLMN_ADD(356110),
	PLMN_ADD(358110),
	PLMN_ADD(30801),
	PLMN_ADD(360110),
	PLMN_ADD(74602),
	PLMN_ADD(37412),
	PLMN_ADD(310160),
	PLMN_ADD(310150),
	PLMN_ADD(310020),
	PLMN_ADD(310050),
	PLMN_ADD(310070),
	PLMN_ADD(310080),
	PLMN_ADD(310090),
	PLMN_ADD(310100),
	PLMN_ADD(310180),
	PLMN_ADD(310190),
	PLMN_ADD(310320),
	PLMN_ADD(310340),
	PLMN_ADD(310350),
	PLMN_ADD(310390),
	PLMN_ADD(310400),
	PLMN_ADD(310450),
	PLMN_ADD(310460),
	PLMN_ADD(310490),
	PLMN_ADD(310500),
	PLMN_ADD(310530),
	PLMN_ADD(310560),
	PLMN_ADD(310570),
	PLMN_ADD(310590),
	PLMN_ADD(310610),
	PLMN_ADD(310630),
	PLMN_ADD(310640),
	PLMN_ADD(310650),
	PLMN_ADD(310470),
	PLMN_ADD(74807),
	PLMN_ADD(74810),
	PLMN_ADD(73401),
	PLMN_ADD(73402),
	PLMN_ADD(50501),
	PLMN_ADD(50502),
	PLMN_ADD(50503),
	PLMN_ADD(50506),
	PLMN_ADD(50508),
	PLMN_ADD(54801),
	PLMN_ADD(54201),
	PLMN_ADD(54720),
	PLMN_ADD(54509),
	PLMN_ADD(55001),
	PLMN_ADD(54601),
	PLMN_ADD(53001),
	PLMN_ADD(53003),
	PLMN_ADD(55280),
	PLMN_ADD(53701),
	PLMN_ADD(54901),
	PLMN_ADD(54010),
	PLMN_ADD(53901),
	PLMN_ADD(54101),
	PLMN_ADD(60301),
	PLMN_ADD(60302),
	PLMN_ADD(63102),
	PLMN_ADD(61601),
	PLMN_ADD(61602),
	PLMN_ADD(61603),
	PLMN_ADD(65201),
	PLMN_ADD(65202),
	PLMN_ADD(61302),
	PLMN_ADD(64201),
	PLMN_ADD(64202),
	PLMN_ADD(64203),
	PLMN_ADD(62401),
	PLMN_ADD(62402),
	PLMN_ADD(62501),
	PLMN_ADD(62301),
	PLMN_ADD(62302),
	PLMN_ADD(62303),
	PLMN_ADD(62201),
	PLMN_ADD(62202),
	PLMN_ADD(65401),
	PLMN_ADD(62901),
	PLMN_ADD(62910),
	PLMN_ADD(63001),
	PLMN_ADD(63005),
	PLMN_ADD(63801),
	PLMN_ADD(60201),
	PLMN_ADD(60202),
	PLMN_ADD(60203),
	PLMN_ADD(63601),
	PLMN_ADD(62701),
	PLMN_ADD(62801),
	PLMN_ADD(62802),
	PLMN_ADD(62803),
	PLMN_ADD(60701),
	PLMN_ADD(60702),
	PLMN_ADD(62001),
	PLMN_ADD(62002),
	PLMN_ADD(62003),
	PLMN_ADD(61102),
	PLMN_ADD(61202),
	PLMN_ADD(61203),
	PLMN_ADD(61205),
	PLMN_ADD(63902),
	PLMN_ADD(63903),
	PLMN_ADD(65101),
	PLMN_ADD(65102),
	PLMN_ADD(60601),
	PLMN_ADD(64601),
	PLMN_ADD(64602),
	PLMN_ADD(65001),
	PLMN_ADD(65010),
	PLMN_ADD(61001),
	PLMN_ADD(61002),
	PLMN_ADD(60901),
	PLMN_ADD(60910),
	PLMN_ADD(61701),
	PLMN_ADD(61710),
	PLMN_ADD(60400),
	PLMN_ADD(60401),
	PLMN_ADD(64301),
	PLMN_ADD(64304),
	PLMN_ADD(64901),
	PLMN_ADD(61401),
	PLMN_ADD(61402),
	PLMN_ADD(61403),
	PLMN_ADD(62120),
	PLMN_ADD(62130),
	PLMN_ADD(62140),
	PLMN_ADD(64700),
	PLMN_ADD(64702),
	PLMN_ADD(64710),
	PLMN_ADD(63510),
	PLMN_ADD(62601),
	PLMN_ADD(60801),
	PLMN_ADD(60802),
	PLMN_ADD(63301),
	PLMN_ADD(63302),
	PLMN_ADD(63310),
	PLMN_ADD(61901),
	PLMN_ADD(61902),
	PLMN_ADD(63701),
	PLMN_ADD(63704),
	PLMN_ADD(63710),
	PLMN_ADD(63719),
	PLMN_ADD(63730),
	PLMN_ADD(65501),
	PLMN_ADD(65507),
	PLMN_ADD(65510),
	PLMN_ADD(63401),
	PLMN_ADD(63402),
	PLMN_ADD(65310),
	PLMN_ADD(64002),
	PLMN_ADD(64003),
	PLMN_ADD(64004),
	PLMN_ADD(64005),
	PLMN_ADD(61501),
	PLMN_ADD(60502),
	PLMN_ADD(60503),
	PLMN_ADD(64101),
	PLMN_ADD(64110),
	PLMN_ADD(64111),
	PLMN_ADD(64501),
	PLMN_ADD(64502),
	PLMN_ADD(64801),
	PLMN_ADD(64803),
	PLMN_ADD(64804),
	PLMN_ADD(45608),
	PLMN_ADD(45609),
};
static const int gvPlmnNum = ARRAY_SIZE(gaPlmnResidTab);


static error_t 	sys_plmn_query(t_sparam sparam, t_lparam lparam)
{
	plmn_msarg_t *arg = (plmn_msarg_t *)lparam;
	if(!arg)
		return EBADPARM;
	arg->resid = RESID_UNDEF;
	switch(arg->plmn){
		case NW_STATUS_CLOSED:
			arg->resid = RESID_STR_NET_CLOSED;
			break;
		case NW_STATUS_SEARCHING:
			arg->resid = RESID_STR_NET_SEARCHING;
			break;
		case NW_STATUS_NO_CELL:
			arg->resid = RESID_STR_NET_NO_CELL;
			break;
		case NW_STATUS_LIMITED_SERVICE:
			arg->resid = RESID_STR_NET_LIMITED_SERVICE;
			break;
		case NW_STATUS_ERROR:
			arg->resid = RESID_STR_NET_ERROR;
			break;
		default:
			{
				int i;
				for(i = 0; i < gvPlmnNum; i++){
					if(gaPlmnResidTab[i].plmn == arg->plmn){
						arg->resid = gaPlmnResidTab[i].resid;
						break;
					}
				}
				if(i == gvPlmnNum)
					arg->resid = RESID_STR_PLMN_UNKNOWN;
			}
			break;
	}

	return SUCCESS;
}

error_t	sys_key_preprocess(gu8 type, amos_key_t key)
{
	error_t ret = NOT_HANDLED;
	switch(key){
		case MVK_POWER:
			if(type == MMI_KEY_PRESS){
				if(amos_get_key_status(MVK_END) != MMI_KEY_UNDEFINED){
					//2 定义了MVK_END键，则短按用于开关背光
					if(prod_bklight_on(BACKLIGHT_DEV_LCD)){
						mime_service_exec(MIME_TYPE_SSAVER, METHOD_NAME_OPEN, NULL, NULL, 0, FALSE);
						prod_bklight_close(BACKLIGHT_DEV_LCD);
					}else{
						prod_bklight_open(BACKLIGHT_DEV_LCD);
					}
					if(prod_bklight_on(BACKLIGHT_DEV_KEYPAD))
						prod_bklight_close(BACKLIGHT_DEV_KEYPAD);
					else
						prod_bklight_open(BACKLIGHT_DEV_KEYPAD);
				}
			}else if(type == MMI_KEY_LONG_PRESS){
				#if defined(FTR_SHUTDOWN_PROMPT)
				if(mime_service_exec(MIME_TYPE_SHUTDOWN, METHOD_NAME_OPEN, NULL, NULL, 0, FALSE) != SUCCESS)
				#endif
				ams_shutdown();
			}
			ret = SUCCESS;
			break;
		case MVK_END:
			if(type == MMI_KEY_LONG_PRESS){
				ams_shutdown();
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}

error_t	sys_key_postprocess(gu8 type, amos_key_t key)
{
	error_t ret = NOT_HANDLED;
	switch(key){
		case MVK_POWER:
			if(amos_get_key_status(MVK_END) != MMI_KEY_UNDEFINED)
				break;
		case MVK_END:
			if(type == MMI_KEY_PRESS){
				app_activate(app_query(prod_get_first_app(), 0));
				ret = SUCCESS;
			}
			break;
	}
	return ret;
}


static error_t		sys_OnLoad(void)
{
	ams_register_key_preprocess(sys_key_preprocess);
	ams_register_key_postprocess(sys_key_postprocess);
	return SUCCESS;
}

static error_t		sys_OnUnload(void)
{
	return SUCCESS;
}


static t_AmsModule		pack_sys_modules[] = {
	MOD_INFO_FUNC(MOD_ID_SYS_MESSAGEBOX),
	MOD_INFO_FUNC(MOD_ID_SYS_PLMN),
	MOD_INFO_FUNC(MOD_ID_SYS_METHODDLG),
#if defined(FTR_TOUCH_SCREEN)
	{
		MOD_ID_SYS_TSCALIB, 			// id
		0,						// prior
		MOD_ATTR_SINGLE_INSTANCE,	// attr
		3*1024, 				// stack size
		0, 						// title resid
	},
#endif
	MOD_INFO_END
};

static int	sys_AppEntry(t_AmsModuleId id, t_sparam sparam, t_lparam lparam)
{
	int ret = EBADPARM;
	switch(id){
		case MOD_ID_SYS_MESSAGEBOX:
			ret = MessageBox_Handler(sparam, lparam);
			break;
		case MOD_ID_SYS_PLMN:
			ret = sys_plmn_query(sparam, lparam);
			break;
		case MOD_ID_SYS_METHODDLG:
			ret = MethodDlg_Handler(sparam, lparam);
			break;
#if defined(FTR_TOUCH_SCREEN)
		case MOD_ID_SYS_TSCALIB:
			ret = TSCalibAppMsgHandler(sparam, lparam);
			break;
#endif
	}
	return ret;
}

t_AmsPackage pack_info_sys = {
	PACK_GUID_SELF
	, sys_OnLoad
	, sys_OnUnload
	, sys_AppEntry

	, pack_sys_modules
	
#if defined(TARGET_WIN)
	, sys_brd_data
#endif
};


#if defined(TARGET_WIN)

__declspec(dllexport) const t_AmsPackage *DllGetAmsInfo(void)
{
	return &pack_info_sys;
}

#endif


