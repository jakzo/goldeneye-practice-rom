image := "goldeneye"

_default:
    just -l

# cleans up all files related to building the rom
clean:
    docker run --rm -v $(pwd):/home/dev {{ image }} make nuke
    docker run --rm -v $(pwd):/home/dev {{ image }} ./scripts/extract_baserom.u.sh

# cleans everything to start from a clean slate
clean_all: clean
    docker rmi {{ image }}

# run this to setup your local machine for development
setup:
    docker build -t {{ image }} .

# build the rom
make: setup
    docker run --rm -v "$(pwd):/home/dev" {{ image }} make -j$(nproc) COMPARE=0 FINAL=YES

# builds the rom and uploads it to a connected summercart64
sc64: make
    sc64deployer sd mkdir /CUSTOM || true
    sc64deployer sd upload build/u/ge007.u.z64 /CUSTOM/practice-rom_latest.z64
