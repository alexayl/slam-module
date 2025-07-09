# ------------------
#  Configurables
# ------------------
APP_NAME         := "slam-module"
CMAKE_ARGS      := "-- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
ESP_BOARD       := "esp32s3_devkitc"
NUCLEO_BOARD    := "nucleo_f302r8"
# Change this to match your ESP32-S3 serial port
ESP_PORT        := "/dev/tty.usbserial-210"

# ------------------
#  Tasks
# ------------------
init:
    sh ./scripts/setup.sh

# --- ESP32-S3 Tasks ---
build-esp32: clean
    (cd .. \
    && west build \
        -p always \
        -b "{{ESP_BOARD}}/esp32s3/procpu" \
        -d "{{APP_NAME}}/build" \
        {{APP_NAME}}/app \
        {{CMAKE_ARGS}} \
        -DEXTRA_DTC_OVERLAY_FILE={{justfile_directory()}}/app/boards/{{ESP_BOARD}}.overlay)

flash-esp32:
    west flash --build-dir build

monitor-esp32:
    python -m serial.tools.miniterm "{{ESP_PORT}}" 115200

run-esp32: build-esp32 flash-esp32 monitor-esp32

# --- Nucleo F302R8 Tasks ---
build-nucleo: clean
    (cd .. && west build \
        -p always \
        -b {{NUCLEO_BOARD}} \
        -d "{{APP_NAME}}/build" \
        {{APP_NAME}}/app \
        {{CMAKE_ARGS}} \
        -DEXTRA_DTC_OVERLAY_FILE={{justfile_directory()}}/app/boards/{{NUCLEO_BOARD}}.overlay)

# --- Generic Tasks ---
build-all: build-esp32 build-nucleo

# --- Default Tasks ---
run: run-esp32
build: build-esp32
flash: flash-esp32
monitor: monitor-esp32

clean: 
    rm -rf "build"