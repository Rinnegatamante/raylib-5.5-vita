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
set(PSVITAIP "192.168.0.170" CACHE STRING "PSVita IP (for FTP access)")
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
    vitaGL
    vitashark
    mathneon
    SceShaccCgExt
    SceShaccCg_stub
    stdc++
    taihen_stub
    SceAppMgr_stub
    SceCtrl_stub
    SceGxm_stub
    SceKernelDmacMgr_stub
    SceCommonDialog_stub
    SceLibKernel_stub
    SceAudio_stub
    SceTouch_stub
    SceHid_stub
    SceMotion_stub
    SceSysmodule_stub
    SceIofilemgr_stub
    SceNetCtl_stub
    SceNet_stub
    SceDisplay_stub
    SceAppUtil_stub
    SceAudioIn_stub
    SceIofilemgr_stub
    ScePower_stub
    SceProcessmgr_stub
    SceIme_stub
)

set(VITA_ELF_CREATE_FLAGS "${VITA_ELF_CREATE_FLAGS}")

vita_create_self(eboot.bin ${PROJECT_NAME} UNSAFE NOSTRIP)

vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} eboot.bin
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE module module
    FILE resources resources
)

if(true)#change to true to send eboot.bin to vita
  add_custom_command(TARGET ${PROJECT_NAME}.vpk-vpk POST_BUILD
    COMMAND echo destroy | nc ${PSVITAIP} 1338
    COMMAND curl -T eboot.bin ftp://${PSVITAIP}:1337/ux0:/app/${VITA_TITLEID}/
    COMMAND echo launch ${VITA_TITLEID} | nc ${PSVITAIP} 1338
  )
endif()