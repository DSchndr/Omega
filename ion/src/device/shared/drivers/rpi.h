#ifndef ION_DEVICE_RPI_H
#define ION_DEVICE_RPI_H

#ifdef __cplusplus
#include <ion/rpi.h>
#include <regs/regs.h>

extern "C" void rpi_isr();

namespace Ion {
namespace Device {
namespace Rpi {

using namespace Regs;

/*  Pin | Role              | Mode                  | Function
 * -----+-------------------+-----------------------+----------
 *  PB9 | Power Enable      | Output                |
 *  PA5 | SPI Clock         | Input                 | AF5
 *  PA6 | SPI MISO          | Output                | AF5
 *  PA7 | SPI MOSI          | Input                 | AF5
 * 
 * in case of n0110
 *  PB9 | Power Enable      | Output                |
 *  PC10| SPI Clock         | Input                 | AF6
 *  PC11| SPI MISO          | Output                | AF6
 *  PC12| SPI MOSI          | Input                 | AF6
 */

void init();
void shutdown();

constexpr GPIO ChipSelectGPIO = GPIOC;
constexpr uint8_t ChipSelectPin = 11;

constexpr static GPIOPin PowerPin = GPIOPin(GPIOB, 9);
constexpr static DMA DMAEngine = DMA2;
constexpr static int DMAStream = 2;

}
}
}

#else
//void __attribute__((interrupt, noinline)) rpi_isr();
void rpi_isr();
#endif

#endif
