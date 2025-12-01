# p2mark

This is a lightweight CLI utility for converting marker information from Panasonic's P2
(stands for *Professional Plug-In*) metadata into the XMP format that can be understood by Adobe Premiere and
Adobe After Effects.

## Why do we need this

This tool tackles a very specific Adobe-related problem that it doesn't parse the clip's
XML metadata (unlike Grass Valley's Edius) and doesn't display the so-called 'Text Memo' objects as markers
on the clip's timeline. Text Memos are essentially markers the camera operator can place at any point
of a video clip that was shot using Panasonic's professional camcorders (like AG-HPX600 or newer models,
but only in P2 mode) by pressing a button. This can be useful during the subsequent editing in an NLE
like Adobe Premiere, for instance where these text memos can be displayed as markers.

I developed this because our broadcasting ingest and editing pipelines use Panasonic's P2 format extensively
(although Panasonic has began to gradually deprecate it). If you shoot on the most recent Panasonic's hand-held
professional camcorders like CX350 or CX370 in MOV or AVCHD, then this wouldn't work for you, because these media formats
have abandoned the P2 structure altogether. No idea about the newest shoulder-mounted camcorders though.

## How to use

Be aware that this is a command-line interface tool, therefore it doesn't have a graphical interface, because it doesn't
need one. This means that this program needs to be run from the Windows' command line (`Win+R > cmd.exe`).

The tool requires you to specify one mandatory argument and this is a path to the `CONTENTS` directory
for your shoot, without the trailing slash.

You can also supply an optional `-l` parameter to only view the information about markers for the shoot,
without generating the XMP files straight away.

Type `-h` to get the extended usage information.

## Usage notes

You can either run this before the ingest process on the P2 card when it's in the card-reader or after you had
ingested your media onto a computer or a media server, but generally straight after the copying process finishes
is a good idea.

If you open the media files in Premiere first, and then run `p2mark`, it's fine, because the tool is intelligent
enough to open already existing Adobe's XMP files and fill in the needed marker information.

**Note**: the tool will skip writing an XMP file if there are already markers in it, because these might be
important editor's markers, so generally it's best to run the tool once the ingest process finishes to
avoid this conflict.

Camcorders ignore XMP files in the `CLIP` directory, so these are fine, a card format will get rid of them, or you can
manually remove them.

## System requirements

To run `p2mark`, you'll need:

* A 64-bit Windows install (at least Windows 10, this wasn't tested with earlier versions of Windows);
* [Visual C++ 2022](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170) installed.

**Note**: 32-bit versions of Windows are not supported.

**Note**: even though this program was written with C++20 and STL, if you want a Linux or a Mac build,
this'll need some digging, because p2mark uses COM to generate UUIDs with the help of `CoCreateGuid()`,
which is a part of Windows API. This was designed as a Windows-only tool, sorry.

## Releases

Version 1.0 has been released after some testing with existing P2 shoots on the 1<sup>st</sup> of December 2025 and it was
confirmed to be working with Adobe Premiere Pro 2025 (which should be fine, unless the XMP specification
changes unexpectedly or if Adobe abandons XMP entirely).

## Project dependencies

* [p-ranav/argparse](https://github.com/p-ranav/argparse): for CLI arguments parsing.
* [leethomason/tinyxml2](https://github.com/leethomason/tinyxml2): for XML parsing, manipulating and writing.

## Building yourself

I chose not to use CMake for a simple Windows CLI tool. The Visual Studio project (2022) has already been configured.

Clone the repo and open the `.sln` file in your Visual Studio. Press `Ctrl+Shift+B` to build the selected
configuration (Debug or Release).

This project already includes pre-built `tinyxml2` binaries linked as a static library.