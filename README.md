# faceid\_lock

A facial recognition lockscreen for X11 and Microsoft Kinect.  Not recommended for use in security-sensitive situations, but still a fun tinker toy.

## Dependencies

- `python2.7`
  - `face_recognition` (depends on `Pillow` and `numpy`)
  - Relevant C API package
- `libfreenect`
  - `libfreenect_sync` (installed by default with standard libfreenect)
- A Microsoft Kinect
  - Tested and working with original version (for XB360)
- `libX11`
- A working C compiler and linker
  - `ld` must be able to convert any file into an object file &ndash; tested and working with GNU `ld`, `ld.lld` may work but not tested

## Building

Please refer to the "Customizing" section prior to compiling.

If everything is installed correctly,

     make

Should produce a binary named `faceid_lock`.

Optionally,

     make install

Places this binary in `~/.local/bin`.

If the binary fails to compile due to a missing library, it's possible that your build of Python was compiled without support for PyMalloc, meaning you should replace the `python2.7m/Python.h` include with `python2.7/Python.h` in `include/python.h` and `-lpython2.7m` with `-lpython2.7` in `Makefile`.

Upon starting the application, you should see a small box in the center of the screen with instructions on how to unlock &ndash; if it seems stuck on "Setting up...", you may have to enter a TTY to kill it (a todo is to solve this issue)

### `fix_kinect`

Included with the final binary is support for a mode which fixes an odd error which causes Kinect initialization to fail on first run after startup.  To activate, simply run `faceid_lock` with any command-line argument prior to running `faceid_lock` by itself.

## Customizing (/Why the optimized binary is >100kb)

If you've already compiled, run:

     make ldclean

Then drop an image of your face into the `img` folder.  It can be in any common format (JPG, PNG, etc.), but ensure that the `USERIMAGE_INPUT` variable in the `Makefile` corresponds with its filename.

This image is compiled into the binary, then accessed at runtime to avoid complications over file locations &ndash; the only downside of such an approach is that the final binary can swell significantly depending on the resolution of the chosen image.  As such, scaling your image down to a smaller resolution may be advantageous if this is a worry.
