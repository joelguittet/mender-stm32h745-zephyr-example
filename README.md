# mender-stm32h745-zephyr-example

[![CI Badge](https://github.com/joelguittet/mender-stm32h745-zephyr-example/workflows/ci/badge.svg)](https://github.com/joelguittet/mender-stm32h745-zephyr-example/actions)
[![Issues Badge](https://img.shields.io/github/issues/joelguittet/mender-stm32h745-zephyr-example)](https://github.com/joelguittet/mender-stm32h745-zephyr-example/issues)
[![License Badge](https://img.shields.io/github/license/joelguittet/mender-stm32h745-zephyr-example)](https://github.com/joelguittet/mender-stm32h745-zephyr-example/blob/master/LICENSE)

[Mender MCU client](https://github.com/joelguittet/mender-mcu-client) is an open source over-the-air (OTA) library updater for MCU devices. This demonstration project runs on STM32H7 hardware using Zephyr RTOS. Authentication of the device with Mender server is achieved using an ATECC608 secure element.


## Getting started

This project is used with a [NUCLEO-H745ZI-Q](https://www.st.com/en/evaluation-tools/nucleo-h745zi-q.html) evaluation board and an [ATECC608](https://www.microchip.com/en-us/product/atecc608b) secure element (both ATECC608A and ATECC608B are compatible, ATECC608B is recommended for new designs). The device tree overlay `nucleo_h745zi_q_m7_firmware.overlay` specifies the wiring of the secure element, as illustrated on the following image, and it is possible to adapt it to your own hardware.

![NUCLEO-H745ZI-Q and ATECC608 wiring](https://raw.githubusercontent.com/joelguittet/mender-stm32h745-zephyr-example/master/.github/docs/wiring.png)

The project is built using Zephyr RTOS v3.4.0 and Zephyr SDK >= v0.16.0. It depends on [cJSON](https://github.com/DaveGamble/cJSON) and a specific fork of [Microchip cryptoauthlib](https://github.com/joelguittet/cryptoauthlib) that support Zephyr integration. There is no other dependencies.

To start using Mender, we recommend that you begin with the Getting started section in [the Mender documentation](https://docs.mender.io).

To start using Zephyr, we recommend that you begin with the Getting started section in [the Zephyr documentation](https://docs.zephyrproject.org/latest/develop/getting_started/index.html). It is highly recommended to be familiar with Zephyr environment and tools to use this example.

### Open the project

Clone the project and retrieve submodules using `git submodule update --init --recursive`.

Then open a new Zephyr virtual environment terminal.

### Configuration of the application

The example application should first be configured to set at least:
- `CONFIG_MENDER_SERVER_TENANT_TOKEN` to set the Tenant Token of your account on "https://hosted.mender.io" server.

You may want to customize few interesting settings:
- `CONFIG_MENDER_SERVER_HOST` if using your own Mender server instance. Tenant Token is not required in this case.
- `CONFIG_MENDER_CLIENT_AUTHENTICATION_POLL_INTERVAL` is the interval to retry authentication on the mender server.
- `CONFIG_MENDER_CLIENT_UPDATE_POLL_INTERVAL` is the interval to check for new deployments.
- `CONFIG_MENDER_CLIENT_INVENTORY_REFRESH_INTERVAL` is the interval to publish inventory data.
- `CONFIG_MENDER_CLIENT_CONFIGURE_REFRESH_INTERVAL` is the interval to refresh device configuration.

Other settings are available in the Kconfig. You can also refer to the mender-mcu-client API and configuration keys.

Particularly, it is possible to activate the Device Troubleshoot add-on that will permit to display the Zephyr console of the device directly on the Mender interface as shown on the following screenshot.

![Troubleshoot console](https://raw.githubusercontent.com/joelguittet/mender-stm32h745-zephyr-example/master/.github/docs/troubleshoot.png)

In order to get the Device Troubleshoot add-on working, the following configuration keys must be defined in the `prj.conf` file:

```
CONFIG_HEAP_MEM_POOL_SIZE=1500
CONFIG_SHELL_BACKEND_SERIAL=n
CONFIG_SHELL_AUTOSTART=n
CONFIG_SHELL_STACK_SIZE=3072
```

### Building and flashing the application

The application relies on mcuboot and requires to build a signed binary file to be flashed on the evaluation board.

Use the following commands to build and flash mcuboot (please adapt the paths to your own installation):

```
west build -s $HOME/zephyrproject/bootloader/mcuboot/boot/zephyr -d build-mcuboot -b nucleo_h745zi_q/stm32h745xx/m7 -- -DDTC_OVERLAY_FILE=path/to/mender-stm32h745-zephyr-example/nucleo_h745zi_q_m7_mcuboot.overlay -DCONFIG_BOOT_SWAP_USING_SCRATCH=y -DCONFIG_BOOT_MAX_IMG_SECTORS=3
west flash -d build-mcuboot
```

Use the following command to build, sign and flash the application (please adapt the paths to your own installation):

```
west build -b nucleo_h745zi_q/stm32h745xx/m7 path/to/mender-stm32h745-zephyr-example
west flash --hex-file build/zephyr/zephyr.signed.hex
```

### Execution of the application

After flashing the application on the NUCLEO-H745ZI-Q evaluation board and displaying logs, press RESET button on the evaluation board (the target is halted by default) and you should be able to see the following:

```
*** Booting Zephyr OS build zephyr-v3.4.0 ***
I: Starting bootloader
I: Primary image: magic=good, swap_type=0x1, copy_done=0x3, image_ok=0x3
I: Scratch: magic=bad, swap_type=0x1, copy_done=0x2, image_ok=0x2
I: Boot source: primary slot
I: Image index: 0, Swap type: none
I: Bootloader chainload address offset: 0x20000
I: Jumping to the first image slot


*** Booting Zephyr OS build zephyr-v3.4.0 ***
[00:00:02.003,000] <inf> mender_stm32h745_zephyr_example: Your address: 192.168.1.183
[00:00:02.014,000] <inf> mender_stm32h745_zephyr_example: Lease time: 43200 seconds
[00:00:02.024,000] <inf> mender_stm32h745_zephyr_example: Subnet: 255.255.255.0
[00:00:02.034,000] <inf> mender_stm32h745_zephyr_example: Router: 192.168.1.1
[00:00:02.048,000] <inf> mender_stm32h745_zephyr_example: MAC address of the device '00:80:e1:7c:10:70'
[00:00:02.060,000] <inf> mender_stm32h745_zephyr_example: Running project 'mender-stm32h745-zephyr-example' version '0.1'
[00:00:02.077,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/tls/generic/cryptoauthlib/src/mender-tls.c (82): Secure element revision information: '00006002'
[00:00:02.096,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/tls/generic/cryptoauthlib/src/mender-tls.c (84): Secure element is an ATECC608A
[00:00:02.117,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/tls/generic/cryptoauthlib/src/mender-tls.c (94): Secure element serial number is: '012348ea3966ea9eee'
[00:00:02.140,000] <inf> mender_stm32h745_zephyr_example: Mender client initialized
[00:00:02.150,000] <inf> mender: CMAKE_SOURCE_DIR/src/mender-storage.c (179): Device configuration not available
[00:00:02.163,000] <inf> mender_stm32h745_zephyr_example: Mender configure initialized
[00:00:02.173,000] <inf> mender_stm32h745_zephyr_example: Mender inventory initialized
[00:00:02.184,000] <inf> mender_stm32h745_zephyr_example: Device configuration retrieved
[00:00:02.244,000] <inf> mender: CMAKE_SOURCE_DIR/src/mender-storage.c (113): OTA ID not available
[00:00:03.593,000] <err> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-api.c (854): [401] Unauthorized: dev auth: unauthorized
[00:00:03.609,000] <inf> mender_stm32h745_zephyr_example: Mender client authentication failed (1/3)
```

You now have to accept your device on the mender interface. Once it is accepted on the mender interface the following will be displayed:

```
[00:01:03.485,000] <inf> mender_stm32h745_zephyr_example: Mender client authenticated
[00:01:03.496,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/ota/zephyr/src/mender-ota.c (146): Application has been mark valid and rollback canceled
[00:01:03.514,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-client.c (433): Checking for deployment...
[00:01:04.756,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-client.c (441): No deployment available
```

Congratulation! Your device is connected to the mender server. Device type is `mender-stm32h745-zephyr-example` and the current software version is displayed.

### Create a new deployment

First retrieve [mender-artifact](https://docs.mender.io/downloads#mender-artifact) tool.

Change `VERSION.txt` file to `0.2`, rebuild and sign the firmware using the following commands. We previously used `hex` file because it is required to flash the device, but we now use `bin` file that is required for the mender-mcu-client:

```
west build -b nucleo_h745zi_q/stm32h745xx/m7 path/to/mender-stm32h745-zephyr-example
```

Then create a new artifact using the following command line:

```
path/to/mender-artifact write rootfs-image --compression none --device-type mender-stm32h745-zephyr-example --artifact-name mender-stm32h745-zephyr-example-v$(head -n1 path/to/mender-stm32h745-zephyr-example/VERSION.txt) --output-path build/zephyr/mender-stm32h745-zephyr-example-v$(head -n1 path/to/mender-stm32h745-zephyr-example/VERSION.txt).mender --file build/zephyr/zephyr.signed.bin
```

Upload the artifact `mender-stm32h745-zephyr-example-v0.2.mender` to the mender server and create a new deployment.

The device checks for the new deployment, downloads the artifact and installs it on the `slot1_partition`. Then it reboots to apply the update:

```
[00:05:02.140,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-client.c (433): Checking for deployment...
[00:05:03.425,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-client.c (453): Downloading deployment artifact with id 'ef70bb87-58ad-4699-a377-c4d7a66afbe7', artifact na-
[00:05:04.720,000] <inf> mender_stm32h745_zephyr_example: Deployment status is 'downloading'
[00:05:07.210,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-artifact.c (380): Artifact has valid version
[00:05:07.225,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/ota/zephyr/src/mender-ota.c (42): Start flashing OTA artifact 'zephyr.signed.bin' with size 393216
[00:05:31.034,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-client.c (463): Download done, installing artifact
[00:05:32.285,000] <inf> mender_stm32h745_zephyr_example: Deployment status is 'installing'
[00:05:32.348,000] <inf> mender_stm32h745_zephyr_example: Deployment status is 'rebooting'
[00:05:32.428,000] <inf> mender_stm32h745_zephyr_example: Restarting system
uart:~$ ** Booting Zephyr OS build zephyr-v3.4.0 ***
I: Starting bootloader
I: Primary image: magic=good, swap_type=0x1, copy_done=0x3, image_ok=0x1
I: Scratch: magic=bad, swap_type=0x1, copy_done=0x2, image_ok=0x2
I: Boot source: primary slot
I: Image index: 0, Swap type: test
I: Starting swap using scratch algorithm.
I: Bootloader chainload address offset: 0x20000
I: Jumping to the first image slot

*** Booting Zephyr OS build zephyr-v3.4.0 ***
[00:00:00.503,000] <inf> mender_stm32h745_zephyr_example: Your address: 192.168.1.183
[00:00:00.513,000] <inf> mender_stm32h745_zephyr_example: Lease time: 43200 seconds
[00:00:00.523,000] <inf> mender_stm32h745_zephyr_example: Subnet: 255.255.255.0
[00:00:00.533,000] <inf> mender_stm32h745_zephyr_example: Router: 192.168.1.1
[00:00:00.547,000] <inf> mender_stm32h745_zephyr_example: MAC address of the device '00:80:e1:7c:10:70'
[00:00:00.559,000] <inf> mender_stm32h745_zephyr_example: Running project 'mender-stm32h745-zephyr-example' version '0.2'
[00:00:00.576,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/tls/generic/cryptoauthlib/src/mender-tls.c (82): Secure element revision information: '00006002'
[00:00:00.595,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/tls/generic/cryptoauthlib/src/mender-tls.c (84): Secure element is an ATECC608A
[00:00:00.616,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/tls/generic/cryptoauthlib/src/mender-tls.c (94): Secure element serial number is: '012348ea3966ea9eee'
[00:00:00.639,000] <inf> mender_stm32h745_zephyr_example: Mender client initialized
[00:00:00.649,000] <inf> mender: CMAKE_SOURCE_DIR/src/mender-storage.c (179): Device configuration not available
[00:00:00.662,000] <inf> mender_stm32h745_zephyr_example: Mender configure initialized
[00:00:00.673,000] <inf> mender_stm32h745_zephyr_example: Mender inventory initialized
[00:00:00.683,000] <inf> mender_stm32h745_zephyr_example: Device configuration retrieved
[00:00:02.061,000] <inf> mender_stm32h745_zephyr_example: Mender client authenticated
[00:00:02.071,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/platform/ota/zephyr/src/mender-ota.c (146): Application has been mark valid and rollback canceled
[00:00:03.332,000] <inf> mender_stm32h745_zephyr_example: Deployment status is 'success'
[00:00:03.343,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-client.c (433): Checking for deployment...
[00:00:04.617,000] <inf> mender: CMAKE_SOURCE_DIR/components/mender-mcu-client/core/src/mender-client.c (441): No deployment available
```

Congratulation! You have updated the device. Mender server displays the success of the deployment.

### Failure or wanted rollback

In case of failure to connect and authenticate to the server the current example application performs a rollback to the previous release.
You can customize the behavior of the example application to add your own checks and perform the rollback in case the tests fail.

### Using an other zephyr evaluation board

The zephyr integration into the mender-mcu-client is generic and it is not limited to STM32 MCUs.
Several points discussed below should be taken into consideration to use an other hardware, including evaluation boards with other MCU families.

#### Flash sectors size

The flash sector size is an important criteria to select an evaluation board. The STM32H745ZI MCU has 128KB sectors, which is not convenient at all to define a custom layout.

In the current example, this constraints the flash layout to a single sector to store the bootloader, and 3 sectors for each slots. The last sector is not used, loosing 128KB of memory.

What remains important is that partitions are aligned on sectors. Moreover `slot0_partition` and `slot1_partition` must have the same size.

#### Configuration of the storage partition

NVS module from Zephyr requires sectors to be 64KB or less in size, which is not compatible with the STM32H745ZI MCU. Moreover it requires at least 3 sectors.

In the current example, authentication keys are not saved in the mender storage area because they are managed by the ATECC608 secure element. Instead, only ID and artifact name are used when an update is done, and device configuration if it is defined.

Implementation has been done using the backup sram available in the STM32H745ZI MCU. Because this is specific and not supported by the mender-mcu-client, `CONFIG_MENDER_PLATFORM_STORAGE_TYPE_WEAK` has been defined and project specific implementation has been done. See `src/mender-storage.c` for more information.

### Using Microchip cryptoauthlib

The ATECC608 is used within the Microchip cryptoauthlib library. It is supposed the secure element is provisioned before starting the application.

I have personally used the [esp_cryptoauth_utility](https://github.com/espressif/esp-cryptoauthlib/tree/master/esp_cryptoauth_utility) to provisioned the device.

Finally note that a fork of the official Microchip cryptoauthlib is used to have up to date Zephyr integration. This is done until the following Pull Requests to the original repository are pending:
- https://github.com/MicrochipTech/cryptoauthlib/pull/354
- https://github.com/MicrochipTech/cryptoauthlib/pull/355

### Using an other mender instance

The communication with the server is done using HTTPS. To get it working, the Root CA that is providing the server certificate should be integrated and registered in the application (see `tls_credential_add` in the `src/main.c` file). Format of the expected Root CA certificate is DER.
In this example we are using the `https://hosted.mender.io` server. While checking the details of the server certificate in your browser, you will see that is is provided by Amazon Root CA 1. Thus the Amazon Root CA 1 certificate `AmazonRootCA1.cer` retrieved at `https://www.amazontrust.com/repository` is integrated in the application.
