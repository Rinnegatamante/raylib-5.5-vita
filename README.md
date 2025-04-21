# raylib 5.5 PSVita port using vitaGL as backend

A raylib PSVita port using vitaGL as backend based on the PVR_PSP2 port by Quenom.

Everything apart from sound should work.

Requires [Northfear's SDL2 fork with vitaGL support](https://github.com/Northfear/SDL) and vitaGL compiled with `HAVE_GLSL_SUPPORT=1`

# Building/Installing

Run `make clean && make -j$(nproc) && make install` in `/src`
