#include "app.h"
#include "rpi_icon.h"
#include <ion/console.h>
#include <ion/rpi.h>

#include <apps/i18n.h>
#include <apps/apps_container.h>

namespace Rpi {

I18n::Message App::Descriptor::name() {
  return I18n::Message::RpiApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::RpiAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::RpiIcon;
}

App::Descriptor::ExaminationLevel App::Descriptor::examinationLevel() {
  return App::Descriptor::ExaminationLevel::Basic;
}

App * App::Snapshot::unpack(Container * container) {
  //return new (container->currentAppBuffer()) App(container, this);
  return new App(this);
}

void App::Snapshot::reset() {
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  Ion::Rpi::transferControl();
  //m_appsContainer->switchTo(m_appsContainer->appSnapshotAtIndex(0));
}

void App::willBecomeInactive() {
  ::App::willBecomeInactive();
}


bool App::processEvent(Ion::Events::Event e) {
  return ::App::processEvent(e);
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_rpiController),
  m_rpiController(this)
    //m_appsContainer((AppsContainer*) container)

{
}

}
