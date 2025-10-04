meson setup build
meson compile -C build
cp data/org.natr12.wincfg-hypr.gschema.xml ~/.local/share/glib-2.0/schemas/
glib-compile-schemas ~/.local/share/glib-2.0/schemas/
./build/wincfg
