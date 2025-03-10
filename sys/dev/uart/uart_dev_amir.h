#ifndef _DEV_UART_DEV_ACCIO_H_
#define _DEV_UART_DEV_ACCIO_H_

//FIFO Depth
#define DASHKIN_UART_FIFO_DEPTH 8

#define DASHKIN_UART_BASE 0x10010000

//Memory offsets
#define DASHKIN_UART_CFG_OFFSET     0x00
#define DASHKIN_UART_RX_DATA_OFFSET 0x04
#define DASHKIN_UART_TX_DATA_OFFSET 0x08
#define DASHKIN_UART_RX_STAT_OFFSET 0x0C
#define DASHKIN_UART_TX_STAT_OFFSET 0x10

//Bit masks
#define DASHKIN_UART_CFG_PARITY 	0x00000001
#define DASHKIN_UART_CFG_STOP   	0x00000002
#define DASHKIN_UART_CFG_ORDER  	0x00000004
#define DASHKIN_UART_CFG_ENABLE 	0x00000008
#define DASHKIN_UART_CFG_BAUD   	0x000000E0
#define DASHKIN_UART_CFG_RX_CNT 	0x0000FF00
#define DASHKIN_UART_CFG_RX_IRQ 	0x00010000
#define DASHKIN_UART_CFG_TX_CNT 	0x0FF00000
#define DASHKIN_UART_CFG_TX_IRQ 	0x10000000

#define DASHKIN_UART_RX_STAT_ERROR  0x00000001
#define DASHKIN_UART_RX_STAT_FULL   0x00000002
#define DASHKIN_UART_RX_STAT_EMPTY  0x00000004
#define DASHKIN_UART_RX_STAT_READY  0x00000008

#define DASHKIN_UART_TX_STAT_FULL   0x00000001
#define DASHKIN_UART_TX_STAT_EMPTY  0x00000002

#define DASHKIN_UART_RX_EMPTY 		0x100
#define DASHKIN_UART_RX_DATA  		0x0FF



/* Memory-mapped I/O access macros for RISC-V */
#define UART_REG(base, reg) (*((volatile uint8_t *)((base) + (reg))))
#define UART_RBR 0x00 /* Receiver Buffer Register */
#define UART_THR 0x00 /* Transmitter Holding Register */
#define UART_LSR 0x05 /* Line Status Register */
#define UART_LSR_DR 0x01 /* Data Ready */
#define UART_LSR_THRE 0x20 /* Transmit Holding Register Empty */

/*
 * High-level UART interface.
 */
struct amir_softc {
	struct uart_softc base;

	uint32_t baudrate;
	uint8_t  parity;
	uint8_t  stop_bits;
	uint8_t  data_bits;
	uint8_t  flow_ctrl;
};

extern struct uart_ops uart_amir_ops;

int amir_bus_attach(struct uart_softc *);
int amir_bus_detach(struct uart_softc *);
int amir_bus_flush(struct uart_softc *, int);
int amir_bus_getsig(struct uart_softc *);
int amir_bus_ioctl(struct uart_softc *, int, intptr_t);
int amir_bus_ipend(struct uart_softc *);
int amir_bus_param(struct uart_softc *, int, int, int, int);
int amir_bus_probe(struct uart_softc *);
int amir_bus_receive(struct uart_softc *);
int amir_bus_setsig(struct uart_softc *, int);
int amir_bus_transmit(struct uart_softc *);
void amir_bus_grab(struct uart_softc *);
void amir_bus_ungrab(struct uart_softc *);

#endif /* _DEV_UART_DEV_ACCIO_H_ */
