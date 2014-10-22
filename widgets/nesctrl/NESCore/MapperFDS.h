//////////////////////////////////////////////////////////////////////////
// Mapper020  Nintendo Disk System(FDS)                                 //
//////////////////////////////////////////////////////////////////////////
enum	{
	BLOCK_READY = 0,
	BLOCK_VOLUME_LABEL,
	BLOCK_FILE_AMOUNT,
	BLOCK_FILE_HEADER,
	BLOCK_FILE_DATA,
};
enum	{
	SIZE_VOLUME_LABEL = 56,
	SIZE_FILE_AMOUNT = 2,
	SIZE_FILE_HEADER = 16,
};
enum	{
	OFFSET_VOLUME_LABEL = 0,
	OFFSET_FILE_AMOUNT = 56,
	OFFSET_FILE_HEADER = 58,
	OFFSET_FILE_DATA = 74,
};

enum	{
	MECHANICAL_SOUND_BOOT = 0,
	MECHANICAL_SOUND_SEEKEND,
	MECHANICAL_SOUND_MOTOR_ON,
	MECHANICAL_SOUND_MOTOR_OFF,
	MECHANICAL_SOUND_ALLSTOP,
};


typedef struct {
	INHERIT_MAPPER;
	
	BOOL	bDiskThrottle;
	INT DiskThrottleTime;
	
	LPBYTE	disk;
	LPBYTE	disk_w;
	
	INT irq_counter, irq_latch; // $4020-$4021
	BYTE	irq_enable, irq_repeat; // $4022
	BYTE	irq_occur;		// IRQ発生時に0以外になる
	BYTE	irq_transfer;		// 割り込み転送フラグ
	
	BYTE	disk_enable;		// Disk I/O enable
	BYTE	sound_enable;		// Sound I/O enable
	BYTE	RW_start;		// 読み書き可能になったらIRQ発生
	BYTE	RW_mode;		// 読み書きモード
	BYTE	disk_motor_mode;	// ディスクモーター
	BYTE	disk_eject; 	// ディスクカードの挿入/非挿入
	BYTE	drive_ready;		// 読み書き中かどうか
	BYTE	drive_reset;		// ドライブリセット状態
	
	INT block_point;
	INT block_mode;
	INT size_file_data;
	INT file_amount;
	INT point;
	BYTE	first_access;
	
	BYTE	disk_side;
	BYTE	disk_mount_count;
	
	BYTE	irq_type;
	
	// For mechanical sound
	BYTE	sound_startup_flag;
	INT sound_startup_timer;
	INT sound_seekend_timer;
}Mapper020;

void	Mapper020_MechanicalSound(Mapper *mapper, INT type );

void	Mapper020_Reset(Mapper *mapper)
{
	((Mapper020 *)mapper)->irq_type = 0;

	((Mapper020 *)mapper)->irq_enable = ((Mapper020 *)mapper)->irq_repeat = 0;
	((Mapper020 *)mapper)->irq_counter = ((Mapper020 *)mapper)->irq_latch = 0;
	((Mapper020 *)mapper)->irq_occur = 0;
	((Mapper020 *)mapper)->irq_transfer = 0;

	((Mapper020 *)mapper)->disk_enable = 0xFF;
	((Mapper020 *)mapper)->sound_enable = 0xFF;

	((Mapper020 *)mapper)->block_point = 0;
	((Mapper020 *)mapper)->block_mode = 0;
	((Mapper020 *)mapper)->RW_start = 0xFF;
	((Mapper020 *)mapper)->size_file_data = 0;
	((Mapper020 *)mapper)->file_amount = 0;
	((Mapper020 *)mapper)->point = 0;
	((Mapper020 *)mapper)->first_access = 0;

	((Mapper020 *)mapper)->disk_eject = 0xFF;
	((Mapper020 *)mapper)->drive_ready = 0;

	((Mapper020 *)mapper)->disk_side = 0xFF;	// Eject
	((Mapper020 *)mapper)->disk = ((Mapper020 *)mapper)->disk_w = NULL;

	// Mechanical sound
	((Mapper020 *)mapper)->sound_startup_flag  = 0xFF;
	((Mapper020 *)mapper)->sound_startup_timer = -1;	// stop
	((Mapper020 *)mapper)->sound_seekend_timer = -1;	// stop
	Mapper020_MechanicalSound(mapper, MECHANICAL_SOUND_ALLSTOP );

//	memset( DRAM, 0xFF, sizeof(DRAM) );
	SetPROM_Bank( 3, DRAM+0x0000, BANKTYPE_DRAM );
	SetPROM_Bank( 4, DRAM+0x2000, BANKTYPE_DRAM );
	SetPROM_Bank( 5, DRAM+0x4000, BANKTYPE_DRAM );
	SetPROM_Bank( 6, DRAM+0x6000, BANKTYPE_DRAM );
	SetPROM_Bank( 7, ROM_GetDISKBIOS(mapper->nes->rom), BANKTYPE_ROM );
	SetCRAM_8K_Bank( 0 );

	// デフォルト
//	mapper->nes->SetIrqType( IRQ_HSYNC );

	// 帰ってきたマリオブラザーズ
	if( ROM_GetMakerID(mapper->nes->rom) == 0x01 && ROM_GetGameID(mapper->nes->rom) == 0x4b4d4152 ) {
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}

	// ガルフォース
	if( ROM_GetMakerID(mapper->nes->rom) == 0xB6 && ROM_GetGameID(mapper->nes->rom) == 0x47414C20 ) {
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
	// ファイアーバム
	if( ROM_GetMakerID(mapper->nes->rom) == 0xB6 && ROM_GetGameID(mapper->nes->rom) == 0x46424D20 ) {
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
	// ３Ｄホットラリー
	if( ROM_GetMakerID(mapper->nes->rom) == 0x01 && ROM_GetGameID(mapper->nes->rom) == 0x54445245 ) {
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
		((Mapper020 *)mapper)->irq_type = 1;	// インチキ
	}
	// タイムツイスト
	if( ROM_GetMakerID(mapper->nes->rom) == 0x01 && ROM_GetGameID(mapper->nes->rom) == 0x54540120 ) {
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
	// バイオミラクルぼくってウパ
	if( ROM_GetMakerID(mapper->nes->rom) == 0xA4 && ROM_GetGameID(mapper->nes->rom) == 0x424F4B20 ) {
		NES_SetRenderMethod( mapper->nes, PRE_ALL_RENDER );
	}
	// パットパットゴルフ
	if( ROM_GetMakerID(mapper->nes->rom) == 0x99 && ROM_GetGameID(mapper->nes->rom) == 0x50504720 ) {
		((Mapper020 *)mapper)->irq_type = 99;
	}

	// パチもん
	if( ROM_GetMakerID(mapper->nes->rom) == 0x00 && ROM_GetGameID(mapper->nes->rom) == 0x00000000 ) {
		memset( RAM+0x100, 0xFF, 0x100 );
	}

DebugOut( "MAKER ID=%02X\n", ROM_GetMakerID(mapper->nes->rom) );
DebugOut( "GAME  ID=%08X\n", ROM_GetGameID(mapper->nes->rom) );

	APU_SelectExSound( mapper->nes->apu, 4 );

//	ExCmdWrite( EXCMDWR_DISKINSERT, 0 );
	// Disk 0, Side Aをセット
	((Mapper020 *)mapper)->disk = ROM_GetPROM(mapper->nes->rom)+16+65500*0;
	((Mapper020 *)mapper)->disk_w = ROM_GetDISK(mapper->nes->rom)+16+65500*0;

	((Mapper020 *)mapper)->disk_side = 0;
	((Mapper020 *)mapper)->disk_eject = 0xFF;
	((Mapper020 *)mapper)->drive_ready = 0;
	((Mapper020 *)mapper)->disk_mount_count = 119;

	NES_Command( mapper->nes, NESCMD_DISK_THROTTLE_OFF, NULL );

	((Mapper020 *)mapper)->bDiskThrottle = FALSE;
	((Mapper020 *)mapper)->DiskThrottleTime = 0;
}

BYTE	Mapper020_ExRead( Mapper *mapper,WORD addr )
{
BYTE	data = addr>>8;

	switch( addr ) {
		case	0x4030:	// Disk I/O status
			data = 0x80;
			data |= (((Mapper020 *)mapper)->irq_occur && !((Mapper020 *)mapper)->irq_transfer)?0x01:0x00;
			data |= (((Mapper020 *)mapper)->irq_occur && ((Mapper020 *)mapper)->irq_transfer)?0x02:0x00;
			((Mapper020 *)mapper)->irq_occur = 0;

			// Clear Timer IRQ
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			// Clear Disk IRQ
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER2 );
			break;
		case	0x4031:	// Disk data read
			if( !((Mapper020 *)mapper)->RW_mode )
				return	0xFF;

			((Mapper020 *)mapper)->first_access = 0;

			if( ((Mapper020 *)mapper)->disk ) {
				switch( ((Mapper020 *)mapper)->block_mode ) {
					case BLOCK_VOLUME_LABEL:
						data = ((Mapper020 *)mapper)->disk[((Mapper020 *)mapper)->block_point];
						if( ((Mapper020 *)mapper)->block_point < SIZE_VOLUME_LABEL ) {
							((Mapper020 *)mapper)->block_point++;
						} else {
							data = 0;
						}
						return	data;
					case BLOCK_FILE_AMOUNT:
						data = ((Mapper020 *)mapper)->disk[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ];
						if( ((Mapper020 *)mapper)->block_point < SIZE_FILE_AMOUNT ) {
							((Mapper020 *)mapper)->block_point++;
							((Mapper020 *)mapper)->file_amount = data;
						} else {
							data = 0;
						}
						return	data;
					case BLOCK_FILE_HEADER:
						data = ((Mapper020 *)mapper)->disk[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ];
						if( ((Mapper020 *)mapper)->block_point == 13 )
							((Mapper020 *)mapper)->size_file_data = data;
						else if( ((Mapper020 *)mapper)->block_point == 14 )
							((Mapper020 *)mapper)->size_file_data += ((INT)data<<8);

						if( ((Mapper020 *)mapper)->block_point < SIZE_FILE_HEADER ) {
							((Mapper020 *)mapper)->block_point++;
						} else {
							data = 0;
						}
						return	data;
					case BLOCK_FILE_DATA:
						data = ((Mapper020 *)mapper)->disk[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ];
						if( ((Mapper020 *)mapper)->block_point < ((Mapper020 *)mapper)->size_file_data + 1 ) {
							((Mapper020 *)mapper)->block_point++;
						} else {
							data = 0;
						}
						return	data;
				}
			} else {
				return	0xFF;
			}
			break;
		case	0x4032:	// Disk status
			data = 0x40;
			data |= ((Mapper020 *)mapper)->disk_eject?0x01:0x00;
			data |= ((Mapper020 *)mapper)->disk_eject?0x04:0x00;
			data |= (!((Mapper020 *)mapper)->disk_eject && ((Mapper020 *)mapper)->disk_motor_mode && !((Mapper020 *)mapper)->drive_reset)?0x00:0x02;
			break;
		case	0x4033:	// External connector data/Battery sense
			data = 0x80;
			break;
		default:
			if( addr >= 0x4040 )
				data = APU_ExRead( mapper->nes->apu, addr );
			break;
	}

//if( addr >= 0x4030 && addr <= 0x4033 ){
//DebugOut( "R %04X %02X\n", addr, data );
//}
	return	data;
}

void	Mapper020_ExWrite( Mapper *mapper,WORD addr, BYTE data )
{
//DebugOut( "W %04X %02X C:%d\n", addr, data, mapper->nes->cpu->GetTotalCycles() );
	switch( addr ) {
		case	0x4020:	// IRQ latch low
			((Mapper020 *)mapper)->irq_latch = (((Mapper020 *)mapper)->irq_latch&0xFF00)|data;
			break;
		case	0x4021:	// IRQ latch high
			((Mapper020 *)mapper)->irq_latch = (((Mapper020 *)mapper)->irq_latch&0x00FF)|((WORD)data<<8);
			break;
		case	0x4022:	// IRQ control
			((Mapper020 *)mapper)->irq_repeat = data & 0x01;
			((Mapper020 *)mapper)->irq_enable = (data & 0x02) && (((Mapper020 *)mapper)->disk_enable);
			((Mapper020 *)mapper)->irq_occur  = 0;
			if( ((Mapper020 *)mapper)->irq_enable ) {
				((Mapper020 *)mapper)->irq_counter = ((Mapper020 *)mapper)->irq_latch;
			} else {
				// Clear Timer IRQ
				CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
			}
			break;

		case	0x4023: // 2C33 control
			((Mapper020 *)mapper)->disk_enable = data & 0x01;
			if( !((Mapper020 *)mapper)->disk_enable ) {
				((Mapper020 *)mapper)->irq_enable = 0;
				((Mapper020 *)mapper)->irq_occur  = 0;
				// Clear Timer IRQ
				CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER );
				// Clear Disk IRQ
				CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER2 );
			}
			break;

		case	0x4024:	// Data write
			// Clear Disk IRQ
			CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER2 );

			if( ((Mapper020 *)mapper)->RW_mode )
				break;

			if( ((Mapper020 *)mapper)->first_access ) {
				((Mapper020 *)mapper)->first_access = 0;
				break;
			}

			if( ((Mapper020 *)mapper)->disk ) {
				switch( ((Mapper020 *)mapper)->block_mode ) {
					case BLOCK_VOLUME_LABEL:
						if( ((Mapper020 *)mapper)->block_point < SIZE_VOLUME_LABEL-1 ) {
							((Mapper020 *)mapper)->disk[ ((Mapper020 *)mapper)->block_point ] = data;
							((Mapper020 *)mapper)->disk_w[ ((Mapper020 *)mapper)->block_point ] = 0xFF;
							((Mapper020 *)mapper)->block_point++;
						}
						break;
					case BLOCK_FILE_AMOUNT:
						if( ((Mapper020 *)mapper)->block_point < SIZE_FILE_AMOUNT ) {
							((Mapper020 *)mapper)->disk[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ] = data;
							((Mapper020 *)mapper)->disk_w[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ] = 0xFF;
							((Mapper020 *)mapper)->block_point++;
						}
						break;
					case BLOCK_FILE_HEADER:
						if( ((Mapper020 *)mapper)->block_point < SIZE_FILE_HEADER ) {
								((Mapper020 *)mapper)->disk[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ] = data;
								((Mapper020 *)mapper)->disk_w[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ] = 0xFF;
								if( ((Mapper020 *)mapper)->block_point == 13 )
									((Mapper020 *)mapper)->size_file_data = data;
								else if( ((Mapper020 *)mapper)->block_point == 14 )
									((Mapper020 *)mapper)->size_file_data |= data << 8;
								((Mapper020 *)mapper)->block_point++;
						}
						break;
					case BLOCK_FILE_DATA:
						if( ((Mapper020 *)mapper)->block_point < ((Mapper020 *)mapper)->size_file_data+1 ) {
							((Mapper020 *)mapper)->disk[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ] = data;
							((Mapper020 *)mapper)->disk_w[ ((Mapper020 *)mapper)->block_point + ((Mapper020 *)mapper)->point ] = 0xFF;
							((Mapper020 *)mapper)->block_point++;
						}
						break;
				}
			}
			break;

		case	0x4025:	// Disk I/O control
			// 割り込み転送
			((Mapper020 *)mapper)->irq_transfer = data & 0x80;
			if( !((Mapper020 *)mapper)->irq_transfer ) {
				CPU_ClrIRQ( mapper->nes->cpu, IRQ_MAPPER2 );
			}

			if( !((Mapper020 *)mapper)->RW_start && (data & 0x40) ) {
				((Mapper020 *)mapper)->block_point = 0;
				switch( ((Mapper020 *)mapper)->block_mode ) {
					case	BLOCK_READY:
						((Mapper020 *)mapper)->block_mode = BLOCK_VOLUME_LABEL;
						((Mapper020 *)mapper)->point = 0;
						break;
					case	BLOCK_VOLUME_LABEL:
						((Mapper020 *)mapper)->block_mode = BLOCK_FILE_AMOUNT;
						((Mapper020 *)mapper)->point += SIZE_VOLUME_LABEL;
						break;
					case	BLOCK_FILE_AMOUNT:
						((Mapper020 *)mapper)->block_mode = BLOCK_FILE_HEADER;
						((Mapper020 *)mapper)->point += SIZE_FILE_AMOUNT;
						break;
					case	BLOCK_FILE_HEADER:
						((Mapper020 *)mapper)->block_mode = BLOCK_FILE_DATA;
						((Mapper020 *)mapper)->point += SIZE_FILE_HEADER;
						break;
					case	BLOCK_FILE_DATA:
						((Mapper020 *)mapper)->block_mode = BLOCK_FILE_HEADER;
						((Mapper020 *)mapper)->point += ((Mapper020 *)mapper)->size_file_data+1;
						break;
				}

				// 最初の１回目の書き込みを無視するため
				((Mapper020 *)mapper)->first_access = 0xFF;
			}

			// 読み書きスタート
			((Mapper020 *)mapper)->RW_start = data & 0x40;

			// 読み書きモード
			((Mapper020 *)mapper)->RW_mode = data & 0x04;

			// 読み書きのリセット
			if( data&0x02 ) {
				((Mapper020 *)mapper)->point = 0;
				((Mapper020 *)mapper)->block_point = 0;
				((Mapper020 *)mapper)->block_mode = BLOCK_READY;
				((Mapper020 *)mapper)->RW_start = 0xFF;
				((Mapper020 *)mapper)->drive_reset = 0xFF;

				((Mapper020 *)mapper)->sound_startup_flag  = 0;
				((Mapper020 *)mapper)->sound_startup_timer = -1;	// stop
			} else {
				((Mapper020 *)mapper)->drive_reset = 0;

				if( !((Mapper020 *)mapper)->sound_startup_flag ) {
					Mapper020_MechanicalSound(mapper, MECHANICAL_SOUND_MOTOR_ON );
					((Mapper020 *)mapper)->sound_startup_flag  = 0xFF;
					((Mapper020 *)mapper)->sound_startup_timer = 40;
					((Mapper020 *)mapper)->sound_seekend_timer = 60*7;
				}
			}

			// ディスクモーターのコントロール
			((Mapper020 *)mapper)->disk_motor_mode = data & 0x01;
			if( !(data & 0x01) ) {
				if( ((Mapper020 *)mapper)->sound_seekend_timer >= 0 ) {
					((Mapper020 *)mapper)->sound_seekend_timer = -1;
					Mapper020_MechanicalSound(mapper, MECHANICAL_SOUND_MOTOR_OFF );
				}
			}

			// Mirror
			if( data&0x08 ) SetVRAM_Mirror_cont( VRAM_HMIRROR );
			else		SetVRAM_Mirror_cont( VRAM_VMIRROR );
			break;

		case	0x4026:	// External connector output/Battery sense
			break;
		default:
			if( addr >= 0x4040 )
				APU_ExWrite( mapper->nes->apu, addr, data );
			break;
	}
}

void	Mapper020_WriteLow( Mapper *mapper,WORD addr, BYTE data )
{
	DRAM[addr-0x6000] = data;
}

void	Mapper020_Write( Mapper *mapper,WORD addr, BYTE data )
{
	if( addr < 0xE000 ) {
		DRAM[addr-0x6000] = data;
	}
}

void	Mapper020_Clock( Mapper *mapper,INT cycles )
{
	// Timer IRQ
	if( ((Mapper020 *)mapper)->irq_enable ) {
		if( !((Mapper020 *)mapper)->irq_type ) {
			((Mapper020 *)mapper)->irq_counter -= cycles;
		}
		if( ((Mapper020 *)mapper)->irq_counter <= 0 ) {
////			((Mapper020 *)mapper)->irq_counter &= 0xFFFF;
			((Mapper020 *)mapper)->irq_counter += ((Mapper020 *)mapper)->irq_latch;

			if( !((Mapper020 *)mapper)->irq_occur ) {
				((Mapper020 *)mapper)->irq_occur = 0xFF;
				if( !((Mapper020 *)mapper)->irq_repeat ) {
					((Mapper020 *)mapper)->irq_enable = 0;
				}
				if( ((Mapper020 *)mapper)->irq_type != 99 ) {
					CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER );
				} else {
					CPU_SetIRQ( mapper->nes->cpu, IRQ_TRIGGER2 );
				}
			}
		}
		if( ((Mapper020 *)mapper)->irq_type ) {
			((Mapper020 *)mapper)->irq_counter -= cycles;
		}
	}
}

void	Mapper020_HSync( Mapper *mapper,INT scanline )
{
	// Disk IRQ
	if( ((Mapper020 *)mapper)->irq_transfer ) {
		CPU_SetIRQ( mapper->nes->cpu, IRQ_MAPPER2 );
	}
}

void	Mapper020_VSync(Mapper *mapper)
{
	if( ((Mapper020 *)mapper)->disk && ((Mapper020 *)mapper)->disk_eject ) {
		if( ((Mapper020 *)mapper)->disk_mount_count > 120 ) {
			((Mapper020 *)mapper)->disk_eject = 0;
		} else {
			((Mapper020 *)mapper)->disk_mount_count++;
		}
	}

//	if( ((Mapper020 *)mapper)->disk && (((Mapper020 *)mapper)->disk_mount_count > 120) ) {
	if( ((Mapper020 *)mapper)->disk ) {
		if( ((Mapper020 *)mapper)->sound_startup_timer > 0 ) {
			((Mapper020 *)mapper)->sound_startup_timer--;
		} else if( ((Mapper020 *)mapper)->sound_startup_timer == 0 ) {
			((Mapper020 *)mapper)->sound_startup_timer--;
			Mapper020_MechanicalSound(mapper, MECHANICAL_SOUND_BOOT );
		}

		if( ((Mapper020 *)mapper)->sound_seekend_timer > 0 ) {
			((Mapper020 *)mapper)->sound_seekend_timer--;
		} else if( ((Mapper020 *)mapper)->sound_seekend_timer == 0 ) {
			((Mapper020 *)mapper)->sound_seekend_timer--;
			Mapper020_MechanicalSound(mapper, MECHANICAL_SOUND_MOTOR_OFF );
			Mapper020_MechanicalSound(mapper, MECHANICAL_SOUND_SEEKEND );
			((Mapper020 *)mapper)->sound_startup_flag  = 0;
		}
	}

	if( ((Mapper020 *)mapper)->irq_transfer || (((Mapper020 *)mapper)->disk && ((Mapper020 *)mapper)->disk_mount_count < 120) ) {
		if( ((Mapper020 *)mapper)->DiskThrottleTime > 2 ) {
			((Mapper020 *)mapper)->bDiskThrottle = TRUE;
		} else {
			((Mapper020 *)mapper)->bDiskThrottle = FALSE;
			((Mapper020 *)mapper)->DiskThrottleTime++;
		}
	} else {
		((Mapper020 *)mapper)->DiskThrottleTime = 0;
		((Mapper020 *)mapper)->bDiskThrottle = FALSE;
	}
	if( !((Mapper020 *)mapper)->bDiskThrottle ) {
		NES_Command( mapper->nes, NESCMD_DISK_THROTTLE_OFF, NULL );
	} else {
		NES_Command( mapper->nes, NESCMD_DISK_THROTTLE_ON, NULL );
	}
}

BYTE	Mapper020_ExCmdRead( Mapper *mapper,EXCMDRD cmd )
{
BYTE	data = 0x00;

	if( cmd == EXCMDRD_DISKACCESS ) {
		if( ((Mapper020 *)mapper)->irq_transfer )
			return	0xFF;
	}

	return	data;
}

void	Mapper020_ExCmdWrite( Mapper *mapper,EXCMDWR cmd, BYTE data )
{
	switch( cmd ) {
		case	EXCMDWR_NONE:
			break;
		case	EXCMDWR_DISKINSERT:
			((Mapper020 *)mapper)->disk = ROM_GetPROM(mapper->nes->rom)+16+65500*data;
			((Mapper020 *)mapper)->disk_w = ROM_GetDISK(mapper->nes->rom)+16+65500*data;
			((Mapper020 *)mapper)->disk_side = data;
			((Mapper020 *)mapper)->disk_eject = 0xFF;
			((Mapper020 *)mapper)->drive_ready = 0;
			((Mapper020 *)mapper)->disk_mount_count = 0;
			break;
		case	EXCMDWR_DISKEJECT:
			((Mapper020 *)mapper)->disk = NULL;	// とりあえず
			((Mapper020 *)mapper)->disk_w = NULL;
			((Mapper020 *)mapper)->disk_side = 0xFF;
			((Mapper020 *)mapper)->disk_eject = 0xFF;
			((Mapper020 *)mapper)->drive_ready = 0;
			((Mapper020 *)mapper)->disk_mount_count = 0;
			break;
	}
}

void	Mapper020_MechanicalSound(Mapper *mapper, INT type )
{
	switch( type ) {
		case	MECHANICAL_SOUND_BOOT:
			// Head start ((Mapper020 *)mapper)->point CAM sound.
			break;
		case	MECHANICAL_SOUND_SEEKEND:
			// Reset or Seekend sound.
			break;
		case	MECHANICAL_SOUND_MOTOR_ON:
			// Start Motor sound.(loop)
			break;
		case	MECHANICAL_SOUND_MOTOR_OFF:
			// Stop Motor sound.(loop)
			break;
		case	MECHANICAL_SOUND_ALLSTOP:
			// Stop sound.
			break;
		default:
			break;
	}
}

void	Mapper020_SaveState( Mapper *mapper,LPBYTE p )
{
	p[0] = ((Mapper020 *)mapper)->irq_enable;
	p[1] = ((Mapper020 *)mapper)->irq_repeat;
	p[2] = ((Mapper020 *)mapper)->irq_occur;
	p[3] = ((Mapper020 *)mapper)->irq_transfer;

	*(INT*)&p[4] = ((Mapper020 *)mapper)->irq_counter;
	*(INT*)&p[8] = ((Mapper020 *)mapper)->irq_latch;

	p[12] = ((Mapper020 *)mapper)->disk_enable;
	p[13] = ((Mapper020 *)mapper)->sound_enable;
	p[14] = ((Mapper020 *)mapper)->RW_start;
	p[15] = ((Mapper020 *)mapper)->RW_mode;
	p[16] = ((Mapper020 *)mapper)->disk_motor_mode;
	p[17] = ((Mapper020 *)mapper)->disk_eject;
	p[18] = ((Mapper020 *)mapper)->drive_ready;
	p[19] = ((Mapper020 *)mapper)->drive_reset;

	*(INT*)&p[20] = ((Mapper020 *)mapper)->block_point;
	*(INT*)&p[24] = ((Mapper020 *)mapper)->block_mode;
	*(INT*)&p[28] = ((Mapper020 *)mapper)->size_file_data;
	*(INT*)&p[32] = ((Mapper020 *)mapper)->file_amount;
	*(INT*)&p[36] = ((Mapper020 *)mapper)->point;

	p[40] = ((Mapper020 *)mapper)->first_access;
	p[41] = ((Mapper020 *)mapper)->disk_side;
	p[42] = ((Mapper020 *)mapper)->disk_mount_count;

	p[44] = ((Mapper020 *)mapper)->sound_startup_flag;
	*(INT*)&p[48] = ((Mapper020 *)mapper)->sound_startup_timer;
	*(INT*)&p[52] = ((Mapper020 *)mapper)->sound_seekend_timer;
}

void	Mapper020_LoadState( Mapper *mapper,LPBYTE p )
{
	((Mapper020 *)mapper)->irq_enable	= p[0];
	((Mapper020 *)mapper)->irq_repeat	= p[1];
	((Mapper020 *)mapper)->irq_occur	= p[2];
	((Mapper020 *)mapper)->irq_transfer	= p[3];

	((Mapper020 *)mapper)->irq_counter	= *(INT*)&p[4];
	((Mapper020 *)mapper)->irq_latch	= *(INT*)&p[8];

	((Mapper020 *)mapper)->disk_enable	= p[12];
	((Mapper020 *)mapper)->sound_enable	= p[13];
	((Mapper020 *)mapper)->RW_start	= p[14];
	((Mapper020 *)mapper)->RW_mode		= p[15];
	((Mapper020 *)mapper)->disk_motor_mode	= p[16];
	((Mapper020 *)mapper)->disk_eject	= p[17];
	((Mapper020 *)mapper)->drive_ready	= p[18];
	((Mapper020 *)mapper)->drive_reset	= p[19];

	((Mapper020 *)mapper)->block_point	= *(INT*)&p[20];
	((Mapper020 *)mapper)->block_mode	= *(INT*)&p[24];
	((Mapper020 *)mapper)->size_file_data	= *(INT*)&p[28];
	((Mapper020 *)mapper)->file_amount	= *(INT*)&p[32];
	((Mapper020 *)mapper)->point		= *(INT*)&p[36];

	((Mapper020 *)mapper)->first_access	= p[40];
	((Mapper020 *)mapper)->disk_side	= p[41];
	((Mapper020 *)mapper)->disk_mount_count= p[42];

	((Mapper020 *)mapper)->sound_startup_flag = p[44];
	((Mapper020 *)mapper)->sound_startup_timer = *(INT*)&p[48];
	((Mapper020 *)mapper)->sound_seekend_timer = *(INT*)&p[52];

	if( ((Mapper020 *)mapper)->disk_side != 0xFF ) {
		((Mapper020 *)mapper)->disk = ROM_GetPROM(mapper->nes->rom)+sizeof(NESHEADER)+65500*((Mapper020 *)mapper)->disk_side;
		((Mapper020 *)mapper)->disk_w = ROM_GetDISK(mapper->nes->rom)+sizeof(NESHEADER)+65500*((Mapper020 *)mapper)->disk_side;
	} else {
		((Mapper020 *)mapper)->disk = NULL;
		((Mapper020 *)mapper)->disk_w = NULL;
	}

	// DiskBios Setup(ステートで上書きされている為)
	SetPROM_Bank( 7, ROM_GetDISKBIOS(mapper->nes->rom), BANKTYPE_ROM );
}
BOOL	Mapper020_IsStateSave(Mapper *mapper) { return TRUE; }
Mapper * Mapper020_New(NES* parent)
{
	Mapper *mapper = (Mapper *)MALLOC(sizeof(Mapper020));
	
	memset(mapper, 0, sizeof(Mapper020));
	mapper->nes = parent;
	memcpy(&mapper->vtbl, &MapperBaseVtbl, sizeof(MapperBaseVtbl));
	mapper->vtbl.Reset = Mapper020_Reset;
	mapper->vtbl.Write = Mapper020_Write;
	mapper->vtbl.ExRead = Mapper020_ExRead;
	mapper->vtbl.ExWrite = Mapper020_ExWrite;
	mapper->vtbl.WriteLow = Mapper020_WriteLow;
	mapper->vtbl.Clock = Mapper020_Clock;
	mapper->vtbl.HSync = Mapper020_HSync;
	mapper->vtbl.VSync = Mapper020_VSync;
	mapper->vtbl.ExCmdRead = Mapper020_ExCmdRead;
	mapper->vtbl.ExCmdWrite = Mapper020_ExCmdWrite;
	mapper->vtbl.SaveState = Mapper020_SaveState;
	mapper->vtbl.LoadState = Mapper020_LoadState;
	mapper->vtbl.IsStateSave = Mapper020_IsStateSave;

	return (Mapper *)mapper;
}


