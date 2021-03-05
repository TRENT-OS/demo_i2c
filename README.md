# Chaser Light Demo for the Sabre Lite (i.MX6)

This chaser light demo utilizes the I2C driver for the Sabre Lite platform. The
demo was specifically created for the Sabre Lite board. Nevertheless, the
general demo should also work with other platforms as long as the underlying
I2C driver would be adapted towards the platform.

The entire setup requires a MCP23017 port expander to drive the LED chaser
light. For the Sabre Lite board, we use the J7 pins which refer to I2C3. Thus,
there is also the need for a Molex 53047-0710 connector cable in order to hook
up the board to the port expander. For other boards, the setup might be
different.

While SEOS-1905 created the demo and was specifically meant for the Sabre Lite,
SEOS-2133 then made some cosmetic changes and prepared the demo and its I2C
component to be more platform independent. This is achieved with several `plat`
directories spread across the file tree.

Future work might include providing the Demo for different platforms or
extracting the I2C component and make it usable with different platforms.

For more information, refer to the corresponding wiki article:
<https://wiki.hensoldt-cyber.systems/display/HEN/TRENTOS-M+I2C+%28Chaser+Light%29+Demo+for+the+BD-SL-i.MX6>

## Build instructions

The executable demo application for the Sabre Lite can be built with the
following command:

```bash
seos_tests/seos_sandbox/scripts/open_trentos_build_env.sh \
seos_tests/seos_sandbox/build-system.sh \
seos_tests/src/demos/demo_i2c \
sabre \
build-sabre-Debug-demo_i2c \
-DCMAKE_BUILD_TYPE=Debug
```

## Disclaimer

It should be said that for the Sabre Lite board, it was not possible to simply
use the `libplatsupport` files provided by Data61. The platform-specific I2C
source file had to be adapted in order for the board to correctly address the
port expander.
As SEOS-2133 makes some structural changes, it might be confusing to start
working with that state when work on a real I2C component is picked up in the
future. You can revert back to a simpler version of the code (commit:
`1191e29902a69c0c253fcb1a83988c011ebf89a0`).
