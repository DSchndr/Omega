apps += Rpi::App
app_headers += apps/rpi/app.h

apps_src += $(addprefix apps/rpi/,\
  app.cpp \
  rpi_controller.cpp \
  rpi_view.cpp \
)

app_images += apps/rpi/rpi_icon.png

i18n_files += $(call i18n_with_universal_for,probability/base)

$(eval $(call depends_on_image,apps/rpi/app.cpp,apps/rpi/rpi_icon.png))
