rm -r timing
mkdir timing
cd timing
../init --plat pc99 --tut hello-camkes-1 --solution
ninja
xterm -hold -e ./simulate &
sleep 100 && kill $(ps aux | grep '[q]emu-system-x86_64 -cpu Nehalem,-vme,+pdpe1gb,-xsave,-xsaveopt,-xsavec,-fsgsbase,-invpcid,enforce -nographic -serial mon:stdio -m size=512M -kernel '$(pwd)'/images/kernel-x86_64-pc99 -initrd '$(pwd)'/images/capdl-loader-image-x86_64-pc99' | awk '{print $2}')
