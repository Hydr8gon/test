# NXEngine64
A port of [NXEngine-evo](https://github.com/nxengine/nxengine-evo) to the N64.

### Downloads
Automatic builds of the latest NXEngine64 are provided via GitHub Actions; you can download them on the [releases page](https://github.com/Hydr8gon/NXEngine64/releases). You will also need the original Cave Story with Aeon Genesis translation; a pre-patched version can be found [here](https://www.cavestory.org/downloads/cavestoryen.zip).

### Usage
From NXEngine64, copy `NXEngine64.z64`, `nxextract.exe`, and the `data` folder to a new folder. From Cave Story, copy `Doukutsu.exe` and merge the `data` folder with the existing one. Run `nxextract.exe` to extract assets and append them to the N64 ROM. You should now be able to run `NXEngine64.z64`.

### Compiling
To compile NXEngine64, you need to have [libdragon](https://github.com/DragonMinded/libdragon) installed. Once that's set up, you can simply run `make` in the project root directory to build. To compile the nxextract tool, run `make -f Makefile.extract`.
