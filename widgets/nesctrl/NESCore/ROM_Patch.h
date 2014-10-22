//
// ROMヘッダの修正(NESToyでの間違い修正など)及びROMパッチ
//

// Mapper 000
if( pme->crc == 0x57970078 ) {	// F-1 Race(J) 無理矢理パッチ(^^;
	pme->lpPRG[0x078C] = 0x6C;
	pme->lpPRG[0x3FE1] = 0xFF;
	pme->lpPRG[0x3FE6] = 0x00;
}
if( pme->crc == 0xaf2bbcbc		// Mach Rider(JU)
 || pme->crc == 0x3acd4bf1		// Mach Rider(Alt)(JU) 無理矢理パッチ(^^;
 || pme->crc == 0x8bbe9bec ) {
	pme->lpPRG[0x090D] = 0x6E;
	pme->lpPRG[0x7FDF] = 0xFF;
	pme->lpPRG[0x7FE4] = 0x00;

	pme->header.control1 = ROM_VMIRROR;
}

if( pme->crc == 0xe16bb5fe ) {	// Zippy Race(J)
	pme->header.control1 &= 0xf6;
}
if( pme->crc == 0x85534474 ) {	// Lode Runner(J)
	pme->lpPRG[0x29E9] = 0xEA;	// セーブメニューを出すパッチ
	pme->lpPRG[0x29EA] = 0xEA;
	pme->lpPRG[0x29F8] = 0xEA;
	pme->lpPRG[0x29F9] = 0xEA;
}

// Mapper 001
if( pme->crc == 0x7831b2ff		// America Daitouryou Senkyo(J)
 || pme->crc == 0x190a3e11		// Be-Bop-Highschool - Koukousei Gokuraku Densetsu(J)
 || pme->crc == 0x52449508		// Home Run Nighter - Pennant League!!(J)
 || pme->crc == 0x0973f714		// Jangou(J)
 || pme->crc == 0x7172f3d4		// Kabushiki Doujou(J)
 || pme->crc == 0xa5781280		// Kujaku Ou 2(J)
 || pme->crc == 0x8ce9c87b		// Money Game, The(J)
 || pme->crc == 0xec47296d		// Morita Kazuo no Shougi(J)
 || pme->crc == 0xcee5857b		// Ninjara Hoi!(J)
 || pme->crc == 0xe63d9193		// Tanigawa Kouji no Shougi Shinan 3(J)
 || pme->crc == 0xd54f5da9		// Tsuppari Wars(J)
 || pme->crc == 0x1e0c7ea3 ) {	// AD&D Dragons of Flame(J)
	pme->header.control1 |= ROM_SAVERAM;
}
if( pme->crc == 0x1995ac4e ) {	// Ferrari Grand Prix Challenge(J) 無理矢理パッチ(^^;
	pme->lpPRG[0x1F7AD] = 0xFF;
	pme->lpPRG[0x1F7BC] = 0x00;
}

if( pme->crc == 0x20d22251 ) {	// Top rider(J) 無理矢理パッチ(^^;
	pme->lpPRG[0x1F17E] = 0xEA;
	pme->lpPRG[0x1F17F] = 0xEA;
}

if( pme->crc == 0x11469ce3 ) {	// Viva! Las Vegas(J) 無理矢理パッチ(^^;
	pme->lpCHR[0x0000] = 0x01;
}

if( pme->crc == 0x3fccdc7b ) {	// Baseball Star - Mezase Sankanou!!(J) 無理矢理パッチ(^^;
	pme->lpPRG[0x0F666] = 0x9D;
}

if( pme->crc == 0xdb564628 ) {	// Mario Open Golf(J)
	pme->lpPRG[0x30195] = 0xC0;
}

// Mapper 002
if( pme->crc == 0x63af202f ) {	// JJ - Tobidase Daisakusen Part 2(J)
	pme->header.control1 &= 0xf6;
	pme->header.control1 |= ROM_VMIRROR;
}

if( pme->crc == 0x99a62e47 ) {	// Black Bass 2, The(J)
	pme->header.control1 &= 0xf6;
	pme->header.control1 |= ROM_VMIRROR;
}

if( pme->crc == 0x0eaa7515		// Rod Land(J)
 || pme->crc == 0x22ab9694 ) {	// Rod Land(E)
	pme->header.control1 &= 0xf6;
	pme->header.control1 |= ROM_VMIRROR;
}

if( pme->crc == 0x2061772a ) {	// Tantei Jinguji Taburou Tokino Sugiyukumamani (J)
	pme->header.control1 &= 0xf6;
	pme->header.control1 |= ROM_VMIRROR;
}

// Mapper 003
if( pme->crc == 0x29401686 ) {	// Minna no Taabou no Nakayoshi Dai Sakusen(J)
//	pme->lpPRG[0x2B3E] = 0x60;
}
if( pme->crc == 0x932a077a ) {	// TwinBee(J)
	pme->mapper = 87;
}
if( pme->crc == 0x8218c637 ) {	// Space Hunter(J)
//	pme->header.control1 &= 0xf6;
//	pme->header.control1 |= ROM_4SCREEN;
	pme->header.control1 = ROM_VMIRROR;
}
if( pme->crc == 0x2bb6a0f8		// Sherlock Holmes - Hakushaku Reijou Yuukai Jiken(J)
 || pme->crc == 0x28c11d24		// Sukeban Deka 3(J)
 || pme->crc == 0x02863604 ) {	// Sukeban Deka 3(J)(Alt)
	pme->header.control1 &= 0xf6;
	pme->header.control1 |= ROM_VMIRROR;
}

// Mapper 004
if( pme->crc == 0x58581770) {	// Rasaaru Ishii no Childs Quest(J)
	pme->header.control1 &= 0xf6;
	pme->header.control1 |= ROM_VMIRROR;
}
if( pme->crc == 0xf3feb3ab		// Kunio Kun no Jidaigeki Dayo Zenin Shuugou! (J)
 || pme->crc == 0xa524ae9b		// Otaku no Seiza - An Adventure in the Otaku Galaxy (J)
 || pme->crc == 0x46dc6e57		// SD Gundam - Gachapon Senshi 2 - Capsule Senki (J)
 || pme->crc == 0x66b2dec7		// SD Gundam - Gachapon Senshi 3 - Eiyuu Senki (J)
 || pme->crc == 0x92b07fd9		// SD Gundam - Gachapon Senshi 4 - New Type Story (J)
 || pme->crc == 0x8ee6463a		// SD Gundam - Gachapon Senshi 5 - Battle of Universal Century (J)
 || pme->crc == 0xaf754426		// Ultraman Club 3 (J)
 || pme->crc == 0xfe4e5b11		// Ushio to Tora - Shinen no Daiyou (J)
 || pme->crc == 0x57c12c17 ) {	// Yamamura Misa Suspense - Kyouto Zaiteku Satsujin Jiken (J)
	pme->header.control1 |= ROM_SAVERAM;
}
if( pme->crc == 0x42e03e4a ) {	// RPG Jinsei Game (J)
	pme->mapper = 118;
	pme->header.control1 |= ROM_SAVERAM;
}
if( pme->crc == 0xfd0299c3 ) {	// METAL MAX(J)
	pme->lpPRG[0x3D522] = 0xA9;
	pme->lpPRG[0x3D523] = 0x19;
}
if( pme->crc == 0x1d2e5018		// Rockman 3(J)
 || pme->crc == 0x6b999aaf ) {	// Mega Man 3(U)
//	pme->lpPRG[0x3C179] = 0xBA;//
//	pme->lpPRG[0x3C9CC] = 0x9E;
}

// Mapper 005
if( pme->crc == 0xe91548d8 ) {	// Shin 4 Nin Uchi Mahjong - Yakuman Tengoku (J)
	pme->header.control1 |= ROM_SAVERAM;
}

if( pme->crc == 0x255b129c ) {	// Gun Sight (J) / Gun Sight (J)[a1]
	pme->lpPRG[0x02D0B] = 0x01;
	pme->lpPRG[0x0BEC0] = 0x01;
}


// Mapper 010
if( pme->crc == 0xc9cce8f2 ) {	// Famicom Wars (J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 016
if( pme->crc == 0x983d8175		// Datach - Battle Rush - Build Up Robot Tournament (J)
 || pme->crc == 0x894efdbc		// Datach - Crayon Shin Chan - Ora to Poi Poi (J)
 || pme->crc == 0x19e81461		// Datach - Dragon Ball Z - Gekitou Tenkaichi Budou Kai (J)
 || pme->crc == 0xbe06853f		// Datach - J League Super Top Players (J)
 || pme->crc == 0x0be0a328		// Datach - SD Gundam - Gundam Wars (J)
 || pme->crc == 0x5b457641		// Datach - Ultraman Club - Supokon Fight! (J)
 || pme->crc == 0xf51a7f46		// Datach - Yuu Yuu Hakusho - Bakutou Ankoku Bujutsu Kai (J)
 || pme->crc == 0x31cd9903		// Dragon Ball Z - Kyoushuu! Saiya Jin (J)
 || pme->crc == 0xe49fc53e		// Dragon Ball Z 2 - Gekishin Freeza!! (J)
 || pme->crc == 0x09499f4d		// Dragon Ball Z 3 - Ressen Jinzou Ningen (J)
 || pme->crc == 0x2e991109		// Dragon Ball Z Gaiden - Saiya Jin Zetsumetsu Keikaku (J)
 || pme->crc == 0x170250de ) {	// Rokudenashi Blues(J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 019
if( pme->crc == 0x3296ff7a		// Battle Fleet (J)
 || pme->crc == 0x429fd177		// Famista '90 (J)
 || pme->crc == 0xdd454208		// Hydlide 3 - Yami Kara no Houmonsha (J)
 || pme->crc == 0xb1b9e187		// Kaijuu Monogatari (J)
 || pme->crc == 0xaf15338f ) {	// Mindseeker (J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 026
if( pme->crc == 0x836cc1ab ) {	// Mouryou Senki Madara (J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 033
if( pme->crc == 0x547e6cc1 ) {	// Flintstones - The Rescue of Dino & Hoppy, The(J)
	pme->mapper = 48;
}

// Mapper 065
if( pme->crc == 0xfd3fc292 ) {	// Ai Sensei no Oshiete - Watashi no Hoshi (J)
	pme->mapper = 32;
}

// Mapper 068
if( pme->crc == 0xfde79681 ) {	// Maharaja (J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 069
if( pme->crc == 0xfeac6916		// Honoo no Toukyuuji - Dodge Danpei 2(J)
 || pme->crc == 0x67898319 ) {	// Barcode World(J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 080
if( pme->crc == 0x95aaed34		// Mirai Shinwa Jarvas (J)
 || pme->crc == 0x17627d4b ) {	// Taito Grand Prix - Eikou heno License (J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 082
if( pme->crc == 0x4819a595 ) {	// Kyuukyoku Harikiri Stadium - Heisei Gannen Ban (J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 086
if( pme->crc == 0xe63f7d0b ) {	// Urusei Yatsura - Lum no Wedding Bell(J)
	pme->mapper = 101;
}

// Mapper 118
if( pme->crc == 0x3b0fb600 ) {	// Ys 3 - Wonderers From Ys (J)
	pme->header.control1 |= ROM_SAVERAM;
}

// Mapper 180
if( pme->crc == 0xc68363f6 ) {	// Crazy Climber(J)
	pme->header.control1 &= 0xf6;
}

// VS-Unisystem
if( pme->crc == 0x70901b25 ) {	// VS Slalom
	pme->mapper = 99;
}

if( pme->crc == 0xd5d7eac4 ) {	// VS Dr. Mario
	pme->mapper = 1;
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->crc == 0xffbef374		// VS Castlevania
 || pme->crc == 0x8c0c2df5 ) {	// VS Top Gun
	pme->mapper = 2;
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->crc == 0xeb2dba63		// VS TKO Boxing
 || pme->crc == 0x98cfe016		// VS TKO Boxing (Alt)
 || pme->crc == 0x9818f656 ) {	// VS TKO Boxing (f1)
	pme->mapper = 4;
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->crc == 0x135adf7c ) {	// VS Atari RBI Baseball
	pme->mapper = 4;
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->crc == 0xf9d3b0a3		// VS Super Xevious
 || pme->crc == 0x9924980a		// VS Super Xevious (b1)
 || pme->crc == 0x66bb838f ) {	// VS Super Xevious (b2)
	pme->mapper = 4;
	pme->header.control1 &= 0xF6;
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->crc == 0x17ae56be ) {	// VS Freedom Force
	pme->mapper = 4;
	pme->header.control1 &= 0xF6;
	pme->header.control1 |= ROM_4SCREEN;
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->crc == 0xe2c0a2be ) {	// VS Platoon
	pme->mapper = 68;
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->crc == 0xcbe85490		// VS Excitebike
 || pme->crc == 0x29155e0c		// VS Excitebike (Alt)
 || pme->crc == 0xff5135a3 ) {	// VS Hogan's Alley
	pme->header.control1 &= 0xF6;
	pme->header.control1 |= ROM_4SCREEN;
}

if( pme->crc == 0x0b65a917 ) {	// VS Mach Rider(Endurance Course)
	pme->lpPRG[0x7FDF] = 0xFF;
	pme->lpPRG[0x7FE4] = 0x00;
}

if( pme->crc == 0x8a6a9848		// VS Mach Rider(Endurance Course)(Alt)
 || pme->crc == 0xae8063ef ) {	// VS Mach Rider(Japan, Fighting Course)
	pme->lpPRG[0x7FDD] = 0xFF;
	pme->lpPRG[0x7FE2] = 0x00;
}

if( pme->crc == 0x16d3f469 ) {	// VS Ninja Jajamaru Kun (J)
	pme->header.control1 &= 0xf6;
	pme->header.control1 |= ROM_VMIRROR;
}

if( pme->crc == 0xc99ec059 ) {	// VS Raid on Bungeling Bay(J)
	pme->mapper = 99;
	pme->header.control1 &= 0xF6;
	pme->header.control1 |= ROM_4SCREEN;
}
if( pme->crc == 0xca85e56d ) {	// VS Mighty Bomb Jack(J)
	pme->mapper = 99;
	pme->header.control1 &= 0xF6;
	pme->header.control1 |= ROM_4SCREEN;
}


if( pme->crc == 0xeb2dba63		// VS TKO Boxing
 || pme->crc == 0x9818f656		// VS TKO Boxing
 || pme->crc == 0xed588f00		// VS Duck Hunt
 || pme->crc == 0x8c0c2df5		// VS Top Gun
 || pme->crc == 0x16d3f469		// VS Ninja Jajamaru Kun
 || pme->crc == 0x8850924b		// VS Tetris
 || pme->crc == 0xcf36261e		// VS Sky Kid
 || pme->crc == 0xe1aa8214		// VS Star Luster
 || pme->crc == 0xec461db9		// VS Pinball
 || pme->crc == 0xe528f651		// VS Pinball (alt)
 || pme->crc == 0x17ae56be		// VS Freedom Force
 || pme->crc == 0xe2c0a2be		// VS Platoon
 || pme->crc == 0xff5135a3		// VS Hogan's Alley
 || pme->crc == 0x70901b25		// VS Slalom
 || pme->crc == 0x0b65a917		// VS Mach Rider(Endurance Course)
 || pme->crc == 0x8a6a9848		// VS Mach Rider(Endurance Course)(Alt)
 || pme->crc == 0xae8063ef		// VS Mach Rider(Japan, Fighting Course)
 || pme->crc == 0xcc2c4b5d		// VS Golf
 || pme->crc == 0xa93a5aee		// VS Stroke and Match Golf
 || pme->crc == 0x86167220		// VS Lady Golf
 || pme->crc == 0xffbef374		// VS Castlevania
 || pme->crc == 0x135adf7c		// VS Atari RBI Baseball
 || pme->crc == 0xd5d7eac4		// VS Dr. Mario
 || pme->crc == 0x46914e3e		// VS Soccer
 || pme->crc == 0x70433f2c		// VS Battle City
 || pme->crc == 0x8d15a6e6		// VS bad .nes
 || pme->crc == 0x1e438d52		// VS Goonies
 || pme->crc == 0xcbe85490		// VS Excitebike
 || pme->crc == 0x29155e0c		// VS Excitebike (alt)
 || pme->crc == 0x07138c06		// VS Clu Clu Land
 || pme->crc == 0x43a357ef		// VS Ice Climber
 || pme->crc == 0x737dd1bf		// VS Super Mario Bros
 || pme->crc == 0x4bf3972d		// VS Super Mario Bros
 || pme->crc == 0x8b60cc58		// VS Super Mario Bros
 || pme->crc == 0x8192c804		// VS Super Mario Bros
 || pme->crc == 0xd99a2087		// VS Gradius
 || pme->crc == 0xf9d3b0a3		// VS Super Xevious
 || pme->crc == 0x9924980a		// VS Super Xevious
 || pme->crc == 0x66bb838f		// VS Super Xevious
 || pme->crc == 0xc99ec059		// VS Raid on Bungeling Bay(J)
 || pme->crc == 0xca85e56d ) {	// VS Mighty Bomb Jack(J)
	pme->header.control2 |= ROM_VSUNISYSTEM;
}

if( pme->mapper == 99 || pme->mapper == 151 ) {
	pme->header.control2 |= ROM_VSUNISYSTEM;
}
