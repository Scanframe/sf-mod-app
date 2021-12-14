# JetBrains CLion

## Build Profiles

In the CLion settings **Build, Execution, Deployment** profiles can be created for different platforms.

In order to have `CMake` use the correct build directory `cmake-build-debug-???` for the targeted profile.<br>
Set the **Environment** entry should contain `SF_TARGET_SYSTEM=Linux` or `SF_TARGET_SYSTEM=Windows`.<br>
And when cross compiling for Windows the **CMake options** should contain `-DSF_CROSS_WINDOWS=ON`.

The `.idea/workspace.xml` file should contain a section like the following excerpt:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  ...
  <component name="CMakeSettings">
    <configurations>
      <configuration PROFILE_NAME="Debug MSVC" ENABLED="false" CONFIG_NAME="Debug" TOOLCHAIN_NAME="Visual Studio">
        <ADDITIONAL_GENERATION_ENVIRONMENT>
          <envs>
            <env name="SF_TARGET_SYSTEM" value="Windows" />
          </envs>
        </ADDITIONAL_GENERATION_ENVIRONMENT>
      </configuration>
      <configuration PROFILE_NAME="Debug GNU" ENABLED="true" CONFIG_NAME="Debug">
        <ADDITIONAL_GENERATION_ENVIRONMENT>
          <envs>
            <env name="SF_TARGET_SYSTEM" value="Linux" />
          </envs>
        </ADDITIONAL_GENERATION_ENVIRONMENT>
      </configuration>
      <configuration PROFILE_NAME="Debug GW" ENABLED="false" CONFIG_NAME="Debug" TOOLCHAIN_NAME="GDB MinGW" GENERATION_OPTIONS="-DSF_CROSS_WINDOWS=ON">
        <ADDITIONAL_GENERATION_ENVIRONMENT>
          <envs>
            <env name="SF_TARGET_SYSTEM" value="Linux" />
          </envs>
        </ADDITIONAL_GENERATION_ENVIRONMENT>
      </configuration>
      <configuration PROFILE_NAME="Debug MinGW" ENABLED="false" CONFIG_NAME="Debug" TOOLCHAIN_NAME="MinGW QT" GENERATION_OPTIONS="-G &quot;MinGW Makefiles&quot;">
        <ADDITIONAL_GENERATION_ENVIRONMENT>
          <envs>
            <env name="SF_TARGET_SYSTEM" value="Windows" />
          </envs>
        </ADDITIONAL_GENERATION_ENVIRONMENT>
      </configuration>
    </configurations>
  </component>
  ...
</project>
```

Representing the profiles:
* Debug MSVC
* Debug GNU
* Debug GW
* Debug MinGW

**Note:**
>The `build.sh` shell script for building all projects and subprojects takes care
> of all of this but CLion needs additional configuration.


## CMake Projects Generation Fix

CMake Build and Catch2 projects are automatically generated.
When this is not the case the **`<generated>`** tag bellow must be deleted from the **`.idea/workspace.xml`** file.
Doi this only when the project is not loaded in CLion.
Afterwards when the project is loaded the build projects are populated again.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<project version="4">
  ...
  <component name="CMakeRunConfigurationManager">
    <generated>
      ...
      <config projectName="qt-concepts" targetName="sf-misc" />
      <config projectName="qt-concepts" targetName="sf-misc-test" />
      <config projectName="qt-concepts" targetName="sf-misc-test-app-mutex" />
      <config projectName="qt-concepts" targetName="sf-gii-test-app-dialog" />
      ...
    </generated>
  </component>
  ...
</project>
```

## Build Project `manual`

To open a browser after compiling the **`manual`** project in CLion the following items must be configured.

* Open the configuration editor for the `manual` project.
* Set the `Executable` to `google-chrome` in Linux.
* Set the `Program arguments` options `--app=file://$CMakeCurrentBuildDir$/html/index.html`

