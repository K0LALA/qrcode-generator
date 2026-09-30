# QR-Code Generator

This QR-Code generator, written in C, was built with ease-of-use and adaptability in mind.

There is no GUI and there is none planned at the moment.

# Usage

## Building

First, clone this repo:

`git clone https://github.com/K0LALA/qrcode-generator.git`

Enter the directory and build using Make:
```sh
cd qrcode-generator
make
```

Note: On Windows, you may need to change the build commands.

This will produce a symlink called gen in the root of the project folder pointing to the executable located in build/gen


## Running

To generate a QR-Code for the given message, just run:

`./gen MESSAGE`

MESSAGE will then be encoded using the most fitting encoding (NUMERIC/ALPHANUMERIC/BYTE at the moment).

The console outputs the QR-Code using black and white characters but every terminal may not be supported, therefore a qr.bmp file is output, with 1 pixel represented 1 module (it may appear blurry for that reason, you can upscale it on any program of your choice).


# TODO

- Error handling
- Version > 1
- Arguments for EC, version, encoding
- Upscale the output image if specified
- UTF-8 and other encoding