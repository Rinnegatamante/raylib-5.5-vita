cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(raylib_basic)

include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "RAYLIB basic")
set(PSVITAIP "192.168.0.170" CACHE STRING "PSVita IP (for FTP access)") [make sure to edit the ip of ps vita] (requires vitacompanion)
set(VITA_TITLEID  "RAYL00001")
set(VITA_VERSION  "01.00")
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d ATTRIBUTE2=12")

# Find SDL2 (Static)
find_library(SDL2_LIB SDL2 HINTS $ENV{VITASDK}/arm-vita-eabi/lib REQUIRED)


include_directories(
    $ENV{VITASDK}/arm-vita-eabi/include
    $ENV{VITASDK}/arm-vita-eabi/include/raylib
    $ENV{VITASDK}/arm-vita-eabi/include/SDL2
)

add_executable(${PROJECT_NAME}
    main.c
)

# Link Libraries (Correct Order)
target_link_libraries(${PROJECT_NAME}
    raylib
    SDL2
    OpenSLES
    m
    c
    taihen_stub.a
    SceAppMgr_stub.a
    SceCtrl_stub.a
    SceGxm_stub.a
    SceCommonDialog_stub.a
    SceLibKernel_stub
    SceAudio_stub.a
    SceTouch_stub.a
    SceHid_stub.a
    SceMotion_stub.a
    SceSysmodule_stub.a
    SceIofilemgr_stub.a
    SceNetCtl_stub.a
    SceNet_stub.a
    SceDisplay_stub.a
    SceAppUtil_stub.a
    SceAudioIn_stub.a
    SceIofilemgr_stub.a
    ScePower_stub.a
    SceProcessmgr_stub.a
    SceIme_stub.a
    liblibIMGEGL_stub_weak.a
    liblibgpu_es4_ext_stub_weak.a
    liblibGLESv2_stub_weak.a
    $ENV{VITASDK}/arm-vita-eabi/lib/libraylib.a
)

set(VITA_ELF_CREATE_FLAGS "${VITA_ELF_CREATE_FLAGS}")

vita_create_self(eboot.bin ${PROJECT_NAME} UNSAFE NOSTRIP)

vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} eboot.bin
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE module module
)

if(false)#change to true to send eboot.bin to vita [make sure to edit the ip of ps vita] (requires vitacompanion)
  add_custom_command(TARGET ${PROJECT_NAME}.vpk-vpk POST_BUILD
    COMMAND echo destroy | nc ${PSVITAIP} 1338
    COMMAND curl -T eboot.bin ftp://${PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/
    COMMAND echo launch ${VITA_TITLEID} | nc ${PSVITAIP} 1338
  )
endif()