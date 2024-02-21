# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = yes            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = yes        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

AUDIO_SUPPORTED = no        # Audio is not supported
RGB_MATRIX_SUPPORTED = yes  # RGB matrix is supported and enabled by default
RGBLIGHT_SUPPORTED = yes    # RGB underglow is supported, but not enabled by default
RGB_MATRIX_ENABLE = yes     # Enable keyboard RGB matrix functionality

SPLIT_KEYBOARD = yes

POINTING_DEVICE_ENABLE = yes # Enable trackball
POINTING_DEVICE_DRIVER = pmw3360


SERIAL_DRIVER = vendor
SERIAL_ENABLE = yes

SPI_DRIVER = vendor
I2C_DRIVER = vendor

SERIAL_DRIVER_REQUIRED = yes
SPI_DRIVER_REQUIRED = yes
I2C_DRIVER_REQUIRED = yes
