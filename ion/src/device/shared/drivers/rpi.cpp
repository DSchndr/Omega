//FOR n0110!
//TODO: Implement n0110 and n0100 in one branch

#include <ion.h>
#include <ion/console.h>
#include "../regs/regs.h"
#include "rpi.h"
#include "display.h"
#include "led.h"

namespace Ion {
namespace Device {
namespace Rpi {
using namespace Regs;

static void enableDisplay() {
  EXTI.IMR()->set(Ion::Device::Rpi::ChipSelectPin, true);
}

static void setOn() {
  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Output);
  PowerPin.group().ODR()->set(PowerPin.pin(), false);
  Ion::Device::Rpi::enableDisplay();
}

static void disableDisplay() {
  EXTI.IMR()->set(Ion::Device::Rpi::ChipSelectPin, false);
  SPI3.CR1()->setSSI(true);
}

static void setOff() {
  Ion::Device::Rpi::disableDisplay();
  PowerPin.group().MODER()->setMode(PowerPin.pin(), GPIO::MODER::Mode::Analog);
  PowerPin.group().PUPDR()->setPull(PowerPin.pin(), GPIO::PUPDR::Pull::None);
}

void init() {
  Ion::Device::Rpi::setOff();

//TODO: Find right GPIO group
  // SPI GPIO
  //Clock
  GPIOC.MODER()->setMode(10, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.AFR()->setAlternateFunction(10, GPIO::AFR::AlternateFunction::AF6);
  //MISO
  GPIOC.MODER()->setMode(11, GPIO::MODER::Mode::Input);
  //GPIOC.PUPDR()->setPull(11, GPIO::PUPDR::Pull::Up);
  //MOSI
  GPIOC.MODER()->setMode(12, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.AFR()->setAlternateFunction(12, GPIO::AFR::AlternateFunction::AF6);

  // SPI
  SPI3.CR1()->setRXONLY(true);
  SPI3.CR1()->setSSI(true); // Software chip select
  SPI3.CR1()->setSSM(true); // Software chip select mode
  SPI3.CR1()->setDFF(true); // 16 bits
  SPI3.CR1()->setSPE(true); // enable
  SPI3.CR2()->setRXDMAEN(true); // enable DMA requests

  // DMA
  DMAEngine.SPAR(DMAStream)->set((uint32_t)SPI3.DR()); // Source
  DMAEngine.SM0AR(DMAStream)->set((uint32_t)Ion::Device::Display::DataAddress); // Destination
  DMAEngine.SNDTR(DMAStream)->set(1); // Number of items
  DMAEngine.SCR(DMAStream)->setCHSEL(3); // SPI Channel
  DMAEngine.SCR(DMAStream)->setDIR(DMA::SCR::Direction::PeripheralToMemory);
  DMAEngine.SCR(DMAStream)->setMSIZE(DMA::SCR::DataSize::HalfWord);
  DMAEngine.SCR(DMAStream)->setPSIZE(DMA::SCR::DataSize::HalfWord);
  DMAEngine.SCR(DMAStream)->setCIRC(true); // Circular
  DMAEngine.SCR(DMAStream)->setEN(true); // Enable

  // ISR
  SYSCFG.EXTICR2()->setEXTI(Ion::Device::Rpi::ChipSelectPin, Ion::Device::Rpi::ChipSelectGPIO); //CHECKME
  //EXTI.IMR()->set(Ion::Rpi::Device::ChipSelectPin, true);
  EXTI.RTSR()->set(Ion::Device::Rpi::ChipSelectPin, true);
  EXTI.FTSR()->set(Ion::Device::Rpi::ChipSelectPin, true);
  NVIC.NVIC_ISER0()->set(23, true); //CHECKME
}

void shutdown() {
  Ion::Device::Rpi::setOff();

  // Certainly not the right way !
  NVIC.NVIC_ISER0()->set(23, false); //CHECKME
  SPI3.CR2()->setRXDMAEN(false); //CHECKME
  DMAEngine.SCR(DMAStream)->setEN(false);

  // SPI GPIO
  GPIOC.MODER()->setMode(10, GPIO::MODER::Mode::Analog);
  GPIOC.PUPDR()->setPull(10, GPIO::PUPDR::Pull::None);
  GPIOC.MODER()->setMode(11, GPIO::MODER::Mode::Analog);
  GPIOC.PUPDR()->setPull(11, GPIO::PUPDR::Pull::None);
  GPIOC.MODER()->setMode(12, GPIO::MODER::Mode::Analog);
  GPIOC.PUPDR()->setPull(12, GPIO::PUPDR::Pull::None);
}


}}}

void Ion::Rpi::transferControl() {
  Ion::Display::pushRectUniform(KDRect(0,0,320,240), KDColor::RGB24(0x808080));
  Ion::Device::Rpi::setOn();
  #define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)
  uint64_t scan, lastScan = 0;
  char buf[18];
  while((scan = Ion::Keyboard::scan()) != 0x40) { // HOME
    if(scan != lastScan) {
      buf[17] = '\0';
      buf[16] = TO_HEX(((scan >> 0) & 0x0f));
      buf[15] = TO_HEX(((scan >> 4) & 0x0f));
      buf[14] = TO_HEX(((scan >> 8) & 0x0f));
      buf[13] = TO_HEX(((scan >> 12) & 0x0f));
      buf[12] = TO_HEX(((scan >> 16) & 0x0f));
      buf[11] = TO_HEX(((scan >> 20) & 0x0f));
      buf[10] = TO_HEX(((scan >> 24) & 0x0f));
      buf[9] = TO_HEX(((scan >> 28) & 0x0f));
      buf[8] = TO_HEX(((scan >> 32) & 0x0f));
      buf[7] = TO_HEX(((scan >> 36) & 0x0f));
      buf[6] = TO_HEX(((scan >> 40) & 0x0f));
      buf[5] = TO_HEX(((scan >> 44) & 0x0f));
      buf[4] = TO_HEX(((scan >> 48) & 0x0f));
      buf[3] = TO_HEX(((scan >> 52) & 0x0f));
      buf[2] = TO_HEX(((scan >> 56) & 0x0f));
      buf[1] = TO_HEX(((scan >> 60) & 0x0f));
      buf[0] = ':';
      Ion::Console::writeLine(buf);
    }
    lastScan = scan;
  }
  #undef TO_HEX
  Ion::Device::Rpi::disableDisplay();
}

void rpi_isr() {
  using namespace Ion::Device::Regs;
  EXTI.PR()->set(Ion::Device::Rpi::ChipSelectPin, true);

  if(GPIOC.IDR()->get(11)) {
    SPI3.CR1()->setSSI(true);
  } else {
    Ion::Device::Display::setDrawingArea(KDRect(0,0,320,240), Ion::Device::Display::Orientation::Landscape);
    *Ion::Device::Display::CommandAddress = Ion::Device::Display::Command::MemoryWrite;
    SPI3.CR1()->setSSI(false);
  }
}