#include "opt_acpi.h"
#include "opt_platform.h"
#include "opt_uart.h"

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/conf.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <machine/bus.h>

#include <dev/uart/uart.h>
#include <dev/uart/uart_cpu.h>
#include <dev/uart/uart_bus.h>
#include <dev/uart/uart_dev_amir.h>

#include <dev/ic/ns16550.h>
#include "uart_if.h"

#define DEFAULT_RCLK 115200

static int amir_probe(struct uart_bas *bas);
static void amir_init(struct uart_bas *bas, int, int, int, int);
static void amir_term(struct uart_bas *bas);
static void amir_putc(struct uart_bas *bas, int);
static int amir_rxready(struct uart_bas *bas);
static int amir_getc(struct uart_bas *bas, struct mtx *);

struct uart_ops uart_amir_ops = {
	.probe = amir_probe,
	.init = amir_init,
	.term = amir_term,
	.putc = amir_putc,
	.rxready = amir_rxready,
	.getc = amir_getc,
};

static int amir_probe(struct uart_bas *bas) {
	return 0;
}

static void amir_init(struct uart_bas *bas, int baudrate, int databits, int stopbits, int parity) {
	uint32_t dashkin_cfg = 0;

	dashkin_cfg |= 1 << 3; //enable

	dashkin_cfg |= 0x0; //even parity
	dashkin_cfg |= 0x0 << 1; //1 stop bit
	dashkin_cfg |= 0b011 << 5; //9600

	dashkin_cfg &= ~(DASHKIN_UART_CFG_RX_IRQ); //disable rx interrupt
	dashkin_cfg &= ~(DASHKIN_UART_CFG_TX_IRQ); //disable tx interrupt

	dashkin_cfg |= 1 << 8; //FIFO 8
	dashkin_cfg |= 1 << 20; //FIFO 8

	uart_setreg(bas, DASHKIN_UART_CFG_OFFSET, dashkin_cfg);
	uart_barrier(bas);

}

static void amir_term(struct uart_bas *bas) {
	/* Clean up UART if necessary */
}

static void amir_putc(struct uart_bas *bas, int c) {
	/* Wait until the transmitter is empty */
	uint32_t data = -1;
	while ((data & uart_getreg(bas, DASHKIN_UART_TX_STAT_OFFSET)) != 0) {
		data = uart_getreg(bas, DASHKIN_UART_TX_STAT_OFFSET);
	}

	uart_setreg(bas, DASHKIN_UART_TX_DATA_OFFSET, c);
	uart_barrier(bas);
}

static int amir_rxready(struct uart_bas *bas) {
	return (uart_getreg(bas, DASHKIN_UART_RX_STAT_OFFSET) & DASHKIN_UART_RX_EMPTY) ? 1 : 0;
}

static int amir_getc(struct uart_bas *bas, struct mtx *hwmtx) {
	uint32_t data;

	uart_lock(hwmtx);

	while((data & uart_getreg(bas, DASHKIN_UART_RX_STAT_OFFSET)) != 0)  {
		uart_unlock(hwmtx);
		DELAY(4);
		uart_lock(hwmtx);
	}

	data = uart_getreg(bas, DASHKIN_UART_RX_DATA_OFFSET);

	uart_unlock(hwmtx);
	
	return data;
}

static kobj_method_t amir_methods[] = {
	KOBJMETHOD(uart_attach,		amir_bus_attach),
	KOBJMETHOD(uart_detach,		amir_bus_detach),
	KOBJMETHOD(uart_flush,		amir_bus_flush),
	KOBJMETHOD(uart_getsig,		amir_bus_getsig),
	KOBJMETHOD(uart_ioctl,		amir_bus_ioctl),
	KOBJMETHOD(uart_ipend,		amir_bus_ipend),
	KOBJMETHOD(uart_param,		amir_bus_param),
	KOBJMETHOD(uart_probe,		amir_bus_probe),
	KOBJMETHOD(uart_receive,	amir_bus_receive),
	KOBJMETHOD(uart_setsig,		amir_bus_setsig),
	KOBJMETHOD(uart_transmit,	amir_bus_transmit),
	KOBJMETHOD(uart_grab,		amir_bus_grab),
	KOBJMETHOD(uart_ungrab,		amir_bus_ungrab),
	{ 0, 0 }
};

struct uart_class uart_amir_class = {
	"amir_uart",
	amir_methods,
	sizeof(struct amir_softc),
	.uc_ops = &uart_amir_ops,
	.uc_range = 8,
	.uc_rclk = DEFAULT_RCLK,
	.uc_rshift = 0
};

/* Additional bus operations */
int amir_bus_attach(struct uart_softc *sc) {
	struct amir_softc *amir = (struct amir_softc*)sc;
	struct uart_bas *bas;
	unsigned int ivar;

	bas = &sc->sc_bas;

	if (!resource_int_value("uart", device_get_unit(sc->sc_dev), "flags", &ivar)) {

	}



	return 0;
}

int amir_bus_detach(struct uart_softc *sc) {
	return 0;
}

int amir_bus_flush(struct uart_softc *sc, int what) {
	return 0;
}

int amir_bus_getsig(struct uart_softc *sc) {
	return 0;
}

int amir_bus_ioctl(struct uart_softc *sc, int request, intptr_t data) {
	return 0;
}

int amir_bus_ipend(struct uart_softc *sc) {
	return 0;
}

int amir_bus_param(struct uart_softc *sc, int baudrate, int databits, int stopbits, int parity) {
	return 0;
}

int amir_bus_receive(struct uart_softc *sc) {
	struct uart_bas *bas = &sc->sc_bas;
	uart_lock(sc->sc_hwmtx);
	while (amir_rxready(bas)) {
		int c = amir_getc(bas, sc->sc_hwmtx);
		uart_rx_put(sc, c);
	}
	uart_unlock(sc->sc_hwmtx);
	return 0;
}

int amir_bus_transmit(struct uart_softc *sc) {
	struct uart_bas *bas = &sc->sc_bas;
	uart_lock(sc->sc_hwmtx);
	while (sc->sc_txdatasz > 0) {
		amir_putc(bas, *sc->sc_txbuf);
		sc->sc_txbuf++;
		sc->sc_txdatasz--;
	}
	uart_unlock(sc->sc_hwmtx);
	return 0;
}

void amir_bus_grab(struct uart_softc *sc) {

}

void amir_bus_ungrab(struct uart_softc *sc) {

}
