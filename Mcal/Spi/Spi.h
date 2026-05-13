#ifndef SPI_H_
#define SPI_H_

#include "Elevator_Types.h"

/* Initialize SPI peripheral as Master (pins/config left for board-specific wiring) */
void Spi_InitMaster(void);

/* Initialize SPI peripheral as Slave (preload TX register behavior) */
void Spi_InitSlave(void);

/* Exchange one 8-byte IPC frame. Blocking convenience wrapper used by scheduler.
 * tx: frame to send
 * rx: buffer to receive into
 */
void Spi_ExchangeFrame(const Ipc_SpiFrame_t* tx, Ipc_SpiFrame_t* rx);

#endif /* SPI_H_ */
