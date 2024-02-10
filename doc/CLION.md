# JetBrains CLion

## Build Project `document`

To open a browser after compiling the **`document`** project in CLion the following items must be configured.

* Open the configuration editor for the `manual` project.
* Set the `Executable` to `google-chrome` in Linux.
* Set the `Program arguments` options `--app=file://$CMakeCurrentBuildDir$/html/index.html`

