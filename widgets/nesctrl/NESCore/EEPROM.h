enum	{
	X24C01_IDLE = 0,	// Idle
	X24C01_ADDRESS, 	// Address set
	X24C01_READ,		// Read
	X24C01_WRITE,		// Write
	X24C01_ACK, 	// Acknowledge
	X24C01_ACK_WAIT,	// Acknowledge wait
};

typedef struct {
	INT	now_state, next_state;
	INT	bitcnt;
	BYTE	addr, data;
	BYTE	sda;
	BYTE	scl_old, sda_old;

	LPBYTE	pEEPDATA;
}X24C01;

void	X24C01_Reset( X24C01 * pme, LPBYTE ptr )
{
	pme->now_state  = X24C01_IDLE;
	pme->next_state = X24C01_IDLE;
	pme->addr	   = 0;
	pme->data	   = 0;
	pme->sda 	   = 0xFF;
	pme->scl_old    = 0;
	pme->sda_old    = 0;

	pme->pEEPDATA   = ptr;
}

void	X24C01_Write( X24C01 * pme, BYTE scl_in, BYTE sda_in )
{
	// Clock line
	BYTE	scl_rise = ~pme->scl_old & scl_in;
	BYTE	scl_fall = pme->scl_old & ~scl_in;
	// Data line
	BYTE	sda_rise = ~pme->sda_old & sda_in;
	BYTE	sda_fall = pme->sda_old & ~sda_in;

	BYTE	scl_old_temp = pme->scl_old;
	BYTE	sda_old_temp = pme->sda_old;

	pme->scl_old = scl_in;
	pme->sda_old = sda_in;

	// Start condition?
	if( scl_old_temp && sda_fall ) {
		pme->now_state = X24C01_ADDRESS;
		pme->bitcnt	  = 0;
		pme->addr	  = 0;
		pme->sda 	  = 0xFF;
		return;
	}

	// Stop condition?
	if( scl_old_temp && sda_rise ) {
		pme->now_state = X24C01_IDLE;
		pme->sda 	  = 0xFF;
		return;
	}

	// SCL ____---- RISE
	if( scl_rise ) {
		switch( pme->now_state ) {
			case	X24C01_ADDRESS:
				if( pme->bitcnt < 7 ) {
					// –{—ˆ‚ÍMSB->LSB
					pme->addr &= ~(1<<pme->bitcnt);
					pme->addr |= (sda_in?1:0)<<pme->bitcnt;
				} else {
					if( sda_in ) {
						pme->next_state = X24C01_READ;
						pme->data	   = pme->pEEPDATA[pme->addr&0x7F];
					} else {
						pme->next_state = X24C01_WRITE;
					}
				}
				pme->bitcnt++;
				break;
			case	X24C01_ACK:
				pme->sda = 0;	// ACK
				break;
			case	X24C01_READ:
				if( pme->bitcnt < 8 ) {
					// –{—ˆ‚ÍMSB->LSB
					pme->sda = (pme->data&(1<<pme->bitcnt))?1:0;
				}
				pme->bitcnt++;
				break;
			case	X24C01_WRITE:
				if( pme->bitcnt < 8 ) {
					// –{—ˆ‚ÍMSB->LSB
					pme->data &= ~(1<<pme->bitcnt);
					pme->data |= (sda_in?1:0)<<pme->bitcnt;
				}
				pme->bitcnt++;
				break;

			case	X24C01_ACK_WAIT:
				if( !sda_in ) {
					pme->next_state = X24C01_IDLE;
				}
				break;
		}
	}

	// SCL ----____ FALL
	if( scl_fall ) {
		switch( pme->now_state ) {
			case	X24C01_ADDRESS:
				if( pme->bitcnt >= 8 ) {
					pme->now_state = X24C01_ACK;
					pme->sda 	  = 0xFF;
				}
				break;
			case	X24C01_ACK:
				pme->now_state = pme->next_state;
				pme->bitcnt	  = 0;
				pme->sda 	  = 0xFF;
				break;
			case	X24C01_READ:
				if( pme->bitcnt >= 8 ) {
					pme->now_state = X24C01_ACK_WAIT;
					pme->addr	  = (pme->addr+1)&0x7F;
				}
				break;
			case	X24C01_WRITE:
				if( pme->bitcnt >= 8 ) {
					pme->now_state  = X24C01_ACK;
					pme->next_state = X24C01_IDLE;
					pme->pEEPDATA[pme->addr&0x7F] = pme->data;
					pme->addr	   = (pme->addr+1)&0x7F;
				}
				break;
		}
	}
}

BYTE	X24C01_Read(X24C01 * pme) {
	return	pme->sda;
}

// For State save
void	X24C01_Load( X24C01 * pme, LPBYTE p )
{
	pme->now_state  = *((INT*)&p[0]);
	pme->next_state = *((INT*)&p[4]);
	pme->bitcnt	   = *((INT*)&p[8]);
	pme->addr	   = p[12];
	pme->data	   = p[13];
	pme->sda 	   = p[14];
	pme->scl_old    = p[15];
	pme->sda_old    = p[16];
}

void	X24C01_Save( X24C01 * pme, LPBYTE p )
{
	*((INT*)&p[0]) = pme->now_state;
	*((INT*)&p[4]) = pme->next_state;
	*((INT*)&p[8]) = pme->bitcnt;
	p[12]		   = pme->addr;
	p[13]		   = pme->data;
	p[14]		   = pme->sda;
	p[15]		   = pme->scl_old;
	p[16]		   = pme->sda_old;
}

enum	{
	X24C02_IDLE = 0,	// Idle
	X24C02_DEVADDR, 	// Device address set
	X24C02_ADDRESS, 	// Address set
	X24C02_READ,		// Read
	X24C02_WRITE,		// Write
	X24C02_ACK, 	// Acknowledge
	X24C02_NAK, 	// Not Acknowledge
	X24C02_ACK_WAIT,	// Acknowledge wait
};
typedef struct {

	INT	now_state, next_state;
	INT	bitcnt;
	BYTE	addr, data, rw;
	BYTE	sda;
	BYTE	scl_old, sda_old;

	LPBYTE	pEEPDATA;
}X24C02;


void	X24C02_Reset( X24C02 * pme, LPBYTE ptr ) {
	pme->now_state  = X24C02_IDLE;
	pme->next_state = X24C02_IDLE;
	pme->addr	   = 0;
	pme->data	   = 0;
	pme->rw		   = 0;
	pme->sda 	   = 0xFF;
	pme->scl_old    = 0;
	pme->sda_old    = 0;

	pme->pEEPDATA   = ptr;
}

void	X24C02_Write( X24C02 * pme, BYTE scl_in, BYTE sda_in ) {
	// Clock line
	BYTE	scl_rise = ~pme->scl_old & scl_in;
	BYTE	scl_fall = pme->scl_old & ~scl_in;
	// Data line
	BYTE	sda_rise = ~pme->sda_old & sda_in;
	BYTE	sda_fall = pme->sda_old & ~sda_in;

	BYTE	scl_old_temp = pme->scl_old;
	BYTE	sda_old_temp = pme->sda_old;

	pme->scl_old = scl_in;
	pme->sda_old = sda_in;

	// Start condition?
	if( scl_old_temp && sda_fall ) {
		pme->now_state = X24C02_DEVADDR;
		pme->bitcnt	  = 0;
		pme->sda 	  = 0xFF;
		return;
	}

	// Stop condition?
	if( scl_old_temp && sda_rise ) {
		pme->now_state = X24C02_IDLE;
		pme->sda 	  = 0xFF;
		return;
	}

	// SCL ____---- RISE
	if( scl_rise ) {
		switch( pme->now_state ) {
			case	X24C02_DEVADDR:
				if( pme->bitcnt < 8 ) {
					pme->data &= ~(1<<(7-pme->bitcnt));
					pme->data |= (sda_in?1:0)<<(7-pme->bitcnt);
				}
				pme->bitcnt++;
				break;
			case	X24C02_ADDRESS:
				if( pme->bitcnt < 8 ) {
					pme->addr &= ~(1<<(7-pme->bitcnt));
					pme->addr |= (sda_in?1:0)<<(7-pme->bitcnt);
				}
				pme->bitcnt++;
				break;
			case	X24C02_READ:
				if( pme->bitcnt < 8 ) {
					pme->sda = (pme->data&(1<<(7-pme->bitcnt)))?1:0;
				}
				pme->bitcnt++;
				break;
			case	X24C02_WRITE:
				if( pme->bitcnt < 8 ) {
					pme->data &= ~(1<<(7-pme->bitcnt));
					pme->data |= (sda_in?1:0)<<(7-pme->bitcnt);
				}
				pme->bitcnt++;
				break;
			case	X24C02_NAK:
				pme->sda = 0xFF; // NAK
				break;
			case	X24C02_ACK:
				pme->sda = 0;	// ACK
				break;
			case	X24C02_ACK_WAIT:
				if( !sda_in ) {
					pme->next_state = X24C02_READ;
					pme->data	   = pme->pEEPDATA[pme->addr];
				}
				break;
		}
	}

	// SCL ----____ FALL
	if( scl_fall ) {
		switch( pme->now_state ) {
			case	X24C02_DEVADDR:
				if( pme->bitcnt >= 8 ) {
					if( (pme->data & 0xA0) == 0xA0 ) {
						pme->now_state  = X24C02_ACK;
						pme->rw		   = pme->data & 0x01;
						pme->sda 	   = 0xFF;
						if( pme->rw ) {
							// Now address read
							pme->next_state = X24C02_READ;
							pme->data	   = pme->pEEPDATA[pme->addr];
						} else {
							pme->next_state = X24C02_ADDRESS;
						}
						pme->bitcnt = 0;
					} else {
						pme->now_state  = X24C02_NAK;
						pme->next_state = X24C02_IDLE;
						pme->sda 	   = 0xFF;
					}
				}
				break;
			case	X24C02_ADDRESS:
				if( pme->bitcnt >= 8 ) {
					pme->now_state = X24C02_ACK;
					pme->sda 	  = 0xFF;
					if( pme->rw ) {
						// Read‚Å‚Íâ‘Î—ˆ‚È‚¢‚ª”O‚Ìˆ×
						pme->next_state = X24C02_IDLE;
					} else {
						// to Data Write
						pme->next_state = X24C02_WRITE;
					}
					pme->bitcnt = 0;
				}
				break;
			case	X24C02_READ:
				if( pme->bitcnt >= 8 ) {
					pme->now_state = X24C02_ACK_WAIT;
					pme->addr	  = (pme->addr+1)&0xFF;
				}
				break;
			case	X24C02_WRITE:
				if( pme->bitcnt >= 8 ) {
					pme->pEEPDATA[pme->addr] = pme->data;
					pme->now_state  = X24C02_ACK;
					pme->next_state = X24C02_WRITE;
					pme->addr	   = (pme->addr+1)&0xFF;
					pme->bitcnt = 0;
				}
				break;
			case	X24C02_NAK:
				pme->now_state = X24C02_IDLE;
				pme->bitcnt	  = 0;
				pme->sda 	  = 0xFF;
				break;
			case	X24C02_ACK:
				pme->now_state = pme->next_state;
				pme->bitcnt	  = 0;
				pme->sda 	  = 0xFF;
				break;
			case	X24C02_ACK_WAIT:
				pme->now_state = pme->next_state;
				pme->bitcnt	  = 0;
				pme->sda 	  = 0xFF;
				break;
		}
	}
}

BYTE	X24C02_Read(X24C02 * pme) {
	return	pme->sda;
}

// For State save
void	X24C02_Load( X24C02 * pme, LPBYTE p )
{
	pme->now_state  = *((INT*)&p[0]);
	pme->next_state = *((INT*)&p[4]);
	pme->bitcnt	   = *((INT*)&p[8]);
	pme->addr	   = p[12];
	pme->data	   = p[13];
	pme->rw		   = p[14];
	pme->sda 	   = p[15];
	pme->scl_old    = p[16];
	pme->sda_old    = p[17];
}

void	X24C02_Save( X24C02 * pme, LPBYTE p )
{
	*((INT*)&p[0]) = pme->now_state;
	*((INT*)&p[4]) = pme->next_state;
	*((INT*)&p[8]) = pme->bitcnt;
	p[12]		   = pme->addr;
	p[13]		   = pme->data;
	p[14]		   = pme->rw;
	p[15]		   = pme->sda;
	p[16]		   = pme->scl_old;
	p[17]		   = pme->sda_old;
}


